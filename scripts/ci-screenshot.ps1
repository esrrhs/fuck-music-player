# Best-effort: launch FuckMPlayer.exe from fuck/, wait for painted CEGUI/D3D UI, capture PNG.
# Soft-fail by default (exit 0 with warning) so flaky GUI on windows-latest
# does not turn the required MSBuild job red. Set SCREENSHOT_STRICT=1 to hard-fail.
$ErrorActionPreference = "Continue"
$root = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)
Set-Location $root

$strict = ($env:SCREENSHOT_STRICT -eq "1")
$outDir = Join-Path $root "ci-artifacts"
New-Item -ItemType Directory -Force -Path $outDir | Out-Null
$pngPath = Join-Path $outDir "player-shell.png"
$diagPath = Join-Path $outDir "screenshot-diag.txt"

function Write-Diag([string]$msg) {
  $line = "[{0}] {1}" -f (Get-Date -Format "o"), $msg
  Write-Host $line
  Add-Content -Path $diagPath -Value $line
}

function Soft-Exit([int]$code, [string]$msg) {
  Write-Diag $msg
  # Always scoop runtime logs for diagnosis
  foreach ($f in @("CEGUI.log", "trace.txt", "error.txt", "d3d-init.txt")) {
    $src = Join-Path (Join-Path $root "fuck") $f
    if (Test-Path $src) {
      Copy-Item -Force $src (Join-Path $outDir $f)
      Write-Diag ("Copied {0} ({1} bytes)" -f $f, (Get-Item $src).Length)
    }
  }
  if ($strict) {
    exit $code
  }
  Write-Diag "Soft-fail: screenshot step will not fail the job (set SCREENSHOT_STRICT=1 to hard-fail)."
  exit 0
}

"" | Set-Content $diagPath
Write-Diag "Starting CI screenshot (strict=$strict)"

$fuck = Join-Path $root "fuck"
$exe = Join-Path $fuck "FuckMPlayer.exe"
if (-not (Test-Path $exe)) {
  Soft-Exit 1 "FuckMPlayer.exe not found at $exe — run ci-stage-runtime.ps1 first"
}

Write-Diag "Working directory: $fuck"
Write-Diag "Listing fuck/ DLLs:"
Get-ChildItem $fuck -Filter "*.dll" | ForEach-Object { Write-Diag ("  DLL {0}" -f $_.Name) }
Write-Diag "Configs:"
foreach ($c in @("fuck.cfg","process.cfg","main.cfg","filefinder.cfg","logo.cfg")) {
  $p = Join-Path $fuck $c
  if (Test-Path $p) {
    Write-Diag ("  --- {0} ---" -f $c)
    Get-Content $p | ForEach-Object { Write-Diag ("    {0}" -f $_) }
  }
}

Add-Type @"
using System;
using System.Runtime.InteropServices;
using System.Text;
public static class NativeWin {
  public delegate bool EnumProc(IntPtr hWnd, IntPtr lParam);
  [DllImport("user32.dll")] public static extern bool EnumWindows(EnumProc lpEnumFunc, IntPtr lParam);
  [DllImport("user32.dll")] public static extern bool IsWindowVisible(IntPtr hWnd);
  [DllImport("user32.dll")] public static extern int GetWindowText(IntPtr hWnd, StringBuilder lpString, int nMaxCount);
  [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr hWnd, out uint lpdwProcessId);
  [DllImport("user32.dll")] public static extern bool GetClientRect(IntPtr hWnd, out RECT lpRect);
  [DllImport("user32.dll")] public static extern bool GetWindowRect(IntPtr hWnd, out RECT lpRect);
  [DllImport("user32.dll")] public static extern bool PrintWindow(IntPtr hwnd, IntPtr hdcBlt, uint nFlags);
  [DllImport("user32.dll")] public static extern bool ClientToScreen(IntPtr hWnd, ref POINT lpPoint);
  [DllImport("user32.dll")] public static extern IntPtr GetDC(IntPtr hWnd);
  [DllImport("user32.dll")] public static extern int ReleaseDC(IntPtr hWnd, IntPtr hDC);
  [DllImport("gdi32.dll")] public static extern IntPtr CreateCompatibleDC(IntPtr hdc);
  [DllImport("gdi32.dll")] public static extern IntPtr CreateCompatibleBitmap(IntPtr hdc, int nWidth, int nHeight);
  [DllImport("gdi32.dll")] public static extern IntPtr SelectObject(IntPtr hdc, IntPtr hgdiobj);
  [DllImport("gdi32.dll")] public static extern bool BitBlt(IntPtr hdcDest, int x, int y, int cx, int cy, IntPtr hdcSrc, int x1, int y1, uint rop);
  [DllImport("gdi32.dll")] public static extern bool DeleteObject(IntPtr hObject);
  [DllImport("gdi32.dll")] public static extern bool DeleteDC(IntPtr hdc);
  [StructLayout(LayoutKind.Sequential)]
  public struct RECT { public int Left, Top, Right, Bottom; }
  [StructLayout(LayoutKind.Sequential)]
  public struct POINT { public int X, Y; }
  public const uint SRCCOPY = 0x00CC0020;
  public const uint PW_RENDERFULLCONTENT = 0x00000002;
  public const uint PW_CLIENTONLY = 0x00000001;
}
"@

Add-Type -AssemblyName System.Drawing

$proc = $null
try {
  $psi = New-Object System.Diagnostics.ProcessStartInfo
  $psi.FileName = $exe
  $psi.WorkingDirectory = $fuck
  $psi.UseShellExecute = $false
  $psi.RedirectStandardOutput = $true
  $psi.RedirectStandardError = $true
  $proc = [System.Diagnostics.Process]::Start($psi)
  Write-Diag ("Started PID {0}" -f $proc.Id)
} catch {
  Soft-Exit 1 ("Failed to start exe: {0}" -f $_.Exception.Message)
}

$deadline = (Get-Date).AddSeconds(60)
$hwnd = [IntPtr]::Zero
$title = ""
while ((Get-Date) -lt $deadline) {
  if ($proc.HasExited) {
    Soft-Exit 1 ("Process exited early code={0}" -f $proc.ExitCode)
  }
  $targetPid = [uint32]$proc.Id
  $script:foundHwnd = [IntPtr]::Zero
  $script:foundTitle = ""
  $cb = [NativeWin+EnumProc]{
    param([IntPtr]$h, [IntPtr]$lp)
    if (-not [NativeWin]::IsWindowVisible($h)) { return $true }
    $wpid = 0
    [void][NativeWin]::GetWindowThreadProcessId($h, [ref]$wpid)
    if ($wpid -ne $targetPid) { return $true }
    $sb = New-Object System.Text.StringBuilder 512
    [void][NativeWin]::GetWindowText($h, $sb, $sb.Capacity)
    $t = $sb.ToString()
    if ([string]::IsNullOrWhiteSpace($t)) { return $true }
    $script:foundHwnd = $h
    $script:foundTitle = $t
    if ($t -match "FuckMPlayer|Fuck") { return $false }
    return $true
  }
  [void][NativeWin]::EnumWindows($cb, [IntPtr]::Zero)
  if ($script:foundHwnd -ne [IntPtr]::Zero) {
    $hwnd = $script:foundHwnd
    $title = $script:foundTitle
    Write-Diag ("Found window hwnd=0x{0:X} title='{1}'" -f $hwnd.ToInt64(), $title)
    break
  }
  Start-Sleep -Milliseconds 500
}

if ($hwnd -eq [IntPtr]::Zero) {
  Soft-Exit 1 "Timed out waiting for a visible top-level window from FuckMPlayer.exe"
}

function Capture-BitBlt([IntPtr]$h, [string]$path) {
  $wr = New-Object NativeWin+RECT
  if (-not [NativeWin]::GetWindowRect($h, [ref]$wr)) { throw "GetWindowRect failed" }
  $w = [Math]::Max(1, $wr.Right - $wr.Left)
  $hgt = [Math]::Max(1, $wr.Bottom - $wr.Top)
  Write-Diag ("BitBlt window rect {0}x{1} at ({2},{3})" -f $w, $hgt, $wr.Left, $wr.Top)
  $screen = [NativeWin]::GetDC([IntPtr]::Zero)
  $mem = [NativeWin]::CreateCompatibleDC($screen)
  $hbmp = [NativeWin]::CreateCompatibleBitmap($screen, $w, $hgt)
  $old = [NativeWin]::SelectObject($mem, $hbmp)
  [void][NativeWin]::BitBlt($mem, 0, 0, $w, $hgt, $screen, $wr.Left, $wr.Top, [NativeWin]::SRCCOPY)
  [void][NativeWin]::SelectObject($mem, $old)
  $bmp = [System.Drawing.Image]::FromHbitmap($hbmp)
  [void][NativeWin]::DeleteObject($hbmp)
  [void][NativeWin]::DeleteDC($mem)
  [void][NativeWin]::ReleaseDC([IntPtr]::Zero, $screen)
  $bmp.Save($path, [System.Drawing.Imaging.ImageFormat]::Png)
  $bmp.Dispose()
}

function Capture-PrintWindow([IntPtr]$h, [string]$path) {
  $wr = New-Object NativeWin+RECT
  if (-not [NativeWin]::GetWindowRect($h, [ref]$wr)) { throw "GetWindowRect failed" }
  $w = [Math]::Max(1, $wr.Right - $wr.Left)
  $hgt = [Math]::Max(1, $wr.Bottom - $wr.Top)
  $bmp = New-Object System.Drawing.Bitmap $w, $hgt
  $g = [System.Drawing.Graphics]::FromImage($bmp)
  $hdc = $g.GetHdc()
  $ok = [NativeWin]::PrintWindow($h, $hdc, [NativeWin]::PW_RENDERFULLCONTENT)
  if (-not $ok) {
    Write-Diag "PrintWindow(PW_RENDERFULLCONTENT) failed; retrying PrintWindow(0)"
    $ok = [NativeWin]::PrintWindow($h, $hdc, 0)
  }
  $g.ReleaseHdc($hdc)
  $g.Dispose()
  if (-not $ok) {
    $bmp.Dispose()
    throw "PrintWindow failed"
  }
  $bmp.Save($path, [System.Drawing.Imaging.ImageFormat]::Png)
  $bmp.Dispose()
}

# Analyze client-area diversity. Reject solid grey/black GDI backdrops from failed GL/D3D capture.
function Test-UiPainted([string]$path) {
  $img = [System.Drawing.Image]::FromFile($path)
  $bmp = New-Object System.Drawing.Bitmap $img
  $img.Dispose()
  $w = $bmp.Width
  $h = $bmp.Height
  # Skip ~title-bar band (~30px) and thin chrome border
  $y0 = [Math]::Min([int](30), $h - 1)
  $x0 = 4
  $x1 = [Math]::Max($x0 + 1, $w - 4)
  $y1 = [Math]::Max($y0 + 1, $h - 4)

  $colors = @{}
  $samples = 0
  $greyish = 0
  $blackish = 0
  $whiteish = 0
  $chroma = 0
  # Dense grid: CEGUI text glyphs are thin; a 24x24 grid often misses them on a black clear.
  $stepX = [Math]::Max(1, [int](($x1 - $x0) / 48))
  $stepY = [Math]::Max(1, [int](($y1 - $y0) / 48))
  for ($y = $y0; $y -lt $y1; $y += $stepY) {
    for ($x = $x0; $x -lt $x1; $x += $stepX) {
      $c = $bmp.GetPixel($x, $y)
      $key = "{0},{1},{2}" -f $c.R, $c.G, $c.B
      if (-not $colors.ContainsKey($key)) { $colors[$key] = 0 }
      $colors[$key]++
      $samples++
      $maxc = [Math]::Max($c.R, [Math]::Max($c.G, $c.B))
      $minc = [Math]::Min($c.R, [Math]::Min($c.G, $c.B))
      # wx default client grey ~ (171,171,171) and near-greys
      if (($maxc - $minc) -le 8 -and $c.R -ge 140 -and $c.R -le 200) { $greyish++ }
      if ($c.R -lt 12 -and $c.G -lt 12 -and $c.B -lt 12) { $blackish++ }
      # Empty wx client / BitBlt of unpainted window is mostly white
      if ($c.R -ge 245 -and $c.G -ge 245 -and $c.B -ge 245) { $whiteish++ }
      # TaharezLook music labels use green/red/blue text; chrome is tinted beige
      if (($maxc - $minc) -ge 30) { $chroma++ }
    }
  }
  $bmp.Dispose()
  $unique = $colors.Count
  $greyPct = if ($samples -gt 0) { [int](100 * $greyish / $samples) } else { 100 }
  $blackPct = if ($samples -gt 0) { [int](100 * $blackish / $samples) } else { 100 }
  $whitePct = if ($samples -gt 0) { [int](100 * $whiteish / $samples) } else { 100 }
  $chromaPct = if ($samples -gt 0) { [int](100 * $chroma / $samples) } else { 0 }
  Write-Diag ("Paint probe: unique~{0} greyish~{1}% blackish~{2}% whiteish~{3}% chroma~{4}% chromaN={5} samples={6} size={7}x{8}" -f $unique, $greyPct, $blackPct, $whitePct, $chromaPct, $chroma, $samples, $w, $h)
  # Player clears to black; real CEGUI chrome is sparse (red/green/blue labels + Taharez beige).
  # Reject empty grey/white GDI clients and pure-black clears with zero chroma.
  $hasChrome = ($chroma -ge 3) -or ($chromaPct -ge 1 -and $unique -ge 10)
  $ok = $hasChrome -and ($greyPct -lt 60) -and ($whitePct -lt 60) -and ($unique -ge 8)
  return @{ Ok = $ok; Unique = $unique; GreyPct = $greyPct; BlackPct = $blackPct; WhitePct = $whitePct; ChromaPct = $chromaPct; Chroma = $chroma }
}

# Give main.dll time to create D3D + load CEGUI schemes/layouts after splash skip.
Write-Diag "Waiting for CEGUI/D3D first paint (multi-attempt)..."
Start-Sleep -Seconds 5
$captureDeadline = (Get-Date).AddSeconds(60)
$attempt = 0
$bestPath = $null
$bestScore = -1
$accepted = $false

while ((Get-Date) -lt $captureDeadline) {
  if ($proc.HasExited) {
    Soft-Exit 1 ("Process exited during paint wait code={0}" -f $proc.ExitCode)
  }
  $attempt++
  $tmpBit = Join-Path $outDir ("attempt-{0}-bitblt.png" -f $attempt)
  $tmpPw = Join-Path $outDir ("attempt-{0}-printwindow.png" -f $attempt)

  $candidates = @()
  try {
    Capture-BitBlt $hwnd $tmpBit
    $candidates += $tmpBit
  } catch {
    Write-Diag ("BitBlt attempt {0} failed: {1}" -f $attempt, $_.Exception.Message)
  }
  try {
    Capture-PrintWindow $hwnd $tmpPw
    $candidates += $tmpPw
  } catch {
    Write-Diag ("PrintWindow attempt {0} failed: {1}" -f $attempt, $_.Exception.Message)
  }

  foreach ($cand in $candidates) {
    if (-not (Test-Path $cand)) { continue }
    $fi = Get-Item $cand
    if ($fi.Length -lt 2048) {
      Write-Diag ("Skipping tiny {0} ({1} bytes)" -f $cand, $fi.Length)
      continue
    }
    $probe = Test-UiPainted $cand
    $score = [int]$probe.Unique + (10 * [int]$probe.ChromaPct) + [int]$probe.Chroma - [int]$probe.GreyPct
    if ($score -gt $bestScore) {
      $bestScore = $score
      $bestPath = $cand
    }
    if ($probe.Ok) {
      Copy-Item -Force $cand $pngPath
      Write-Diag ("Accepted painted UI from {0}" -f $cand)
      $accepted = $true
      break
    }
  }
  if ($accepted) { break }

  Write-Diag ("Attempt {0}: UI not painted yet (bestScore={1}); sleeping..." -f $attempt, $bestScore)
  Start-Sleep -Seconds 2
}

if (-not $accepted) {
  if ($bestPath -and (Test-Path $bestPath)) {
    Copy-Item -Force $bestPath $pngPath
    Write-Diag ("No painted UI accepted; keeping best effort {0}" -f $bestPath)
  }
  Soft-Exit 1 "Timed out waiting for non-grey CEGUI/D3D client content"
}

if (-not (Test-Path $pngPath)) {
  Soft-Exit 1 "PNG was not written"
}

$fi = Get-Item $pngPath
Write-Diag ("Wrote {0} ({1} bytes)" -f $pngPath, $fi.Length)

# Copy runtime logs
foreach ($f in @("CEGUI.log", "trace.txt", "error.txt", "d3d-init.txt")) {
  $src = Join-Path $fuck $f
  if (Test-Path $src) {
    Copy-Item -Force $src (Join-Path $outDir $f)
    Write-Diag ("Copied {0} ({1} bytes)" -f $f, (Get-Item $src).Length)
  } else {
    Write-Diag ("Missing runtime log: {0}" -f $f)
  }
}

# Also copy into docs/ for optional commit-back by a follow-up step
$docs = Join-Path $root "docs"
New-Item -ItemType Directory -Force -Path $docs | Out-Null
Copy-Item -Force $pngPath (Join-Path $docs "player-shell.png")
Write-Diag "Copied to docs/player-shell.png"

try {
  if ($proc -and -not $proc.HasExited) {
    $proc.Kill()
    Write-Diag "Killed player process"
  }
} catch {
  Write-Diag ("Kill failed: {0}" -f $_.Exception.Message)
}

Write-Diag "Screenshot step succeeded"
exit 0
