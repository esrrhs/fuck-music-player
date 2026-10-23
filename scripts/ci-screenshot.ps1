# Best-effort: launch FuckMPlayer.exe from fuck/, wait for a visible window, capture PNG.
# Soft-fail by default (exit 0 with warning) so flaky GUI/OpenGL on windows-latest
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

# Prefer interactive session for GUI; Actions runners are often session 0.
Write-Diag "Working directory: $fuck"
Write-Diag "Listing fuck/ DLLs:"
Get-ChildItem $fuck -Filter "*.dll" | ForEach-Object { Write-Diag ("  DLL {0}" -f $_.Name) }

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
  public const uint SRCCOPY = 0x00CC0020;
  public const uint PW_RENDERFULLCONTENT = 0x00000002;
}
"@

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

$deadline = (Get-Date).AddSeconds(45)
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
    # Prefer the main player frame title
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

# Give splash / first paint a moment
Start-Sleep -Seconds 3

Add-Type -AssemblyName System.Drawing

function Capture-Window([IntPtr]$h, [string]$path) {
  $wr = New-Object NativeWin+RECT
  if (-not [NativeWin]::GetWindowRect($h, [ref]$wr)) {
    throw "GetWindowRect failed"
  }
  $w = [Math]::Max(1, $wr.Right - $wr.Left)
  $hgt = [Math]::Max(1, $wr.Bottom - $wr.Top)
  Write-Diag ("Window rect {0}x{1} at ({2},{3})" -f $w, $hgt, $wr.Left, $wr.Top)

  $bmp = New-Object System.Drawing.Bitmap $w, $hgt
  $g = [System.Drawing.Graphics]::FromImage($bmp)
  $hdc = $g.GetHdc()
  $ok = [NativeWin]::PrintWindow($h, $hdc, [NativeWin]::PW_RENDERFULLCONTENT)
  if (-not $ok) {
    Write-Diag "PrintWindow(PW_RENDERFULLCONTENT) failed; retrying PrintWindow(0)"
    $ok = [NativeWin]::PrintWindow($h, $hdc, 0)
  }
  $g.ReleaseHdc($hdc)
  if (-not $ok) {
    Write-Diag "PrintWindow failed; trying BitBlt from screen DC"
    $screen = [NativeWin]::GetDC([IntPtr]::Zero)
    $mem = [NativeWin]::CreateCompatibleDC($screen)
    $hbmp = [NativeWin]::CreateCompatibleBitmap($screen, $w, $hgt)
    $old = [NativeWin]::SelectObject($mem, $hbmp)
    [void][NativeWin]::BitBlt($mem, 0, 0, $w, $hgt, $screen, $wr.Left, $wr.Top, [NativeWin]::SRCCOPY)
    [void][NativeWin]::SelectObject($mem, $old)
    $bmp2 = [System.Drawing.Image]::FromHbitmap($hbmp)
    [void][NativeWin]::DeleteObject($hbmp)
    [void][NativeWin]::DeleteDC($mem)
    [void][NativeWin]::ReleaseDC([IntPtr]::Zero, $screen)
    $g.Dispose()
    $bmp.Dispose()
    $bmp2.Save($path, [System.Drawing.Imaging.ImageFormat]::Png)
    $bmp2.Dispose()
    return
  }
  $bmp.Save($path, [System.Drawing.Imaging.ImageFormat]::Png)
  $g.Dispose()
  $bmp.Dispose()
}

try {
  Capture-Window $hwnd $pngPath
} catch {
  Soft-Exit 1 ("Capture failed: {0}" -f $_.Exception.Message)
}

if (-not (Test-Path $pngPath)) {
  Soft-Exit 1 "PNG was not written"
}

$fi = Get-Item $pngPath
Write-Diag ("Wrote {0} ({1} bytes)" -f $pngPath, $fi.Length)

# Reject trivially empty / tiny captures
if ($fi.Length -lt 2048) {
  Soft-Exit 1 "PNG too small; likely a blank/failed capture"
}

# Heuristic: mostly-black frames are common on headless OpenGL — still upload but warn
try {
  $img = [System.Drawing.Image]::FromFile($pngPath)
  $bmp = New-Object System.Drawing.Bitmap $img
  $img.Dispose()
  $sampleBlack = 0
  $samples = 0
  for ($y = 0; $y -lt $bmp.Height; $y += [Math]::Max(1, [int]($bmp.Height / 16))) {
    for ($x = 0; $x -lt $bmp.Width; $x += [Math]::Max(1, [int]($bmp.Width / 16))) {
      $c = $bmp.GetPixel($x, $y)
      $samples++
      if ($c.R -lt 8 -and $c.G -lt 8 -and $c.B -lt 8) { $sampleBlack++ }
    }
  }
  $bmp.Dispose()
  $pct = if ($samples -gt 0) { [int](100 * $sampleBlack / $samples) } else { 100 }
  Write-Diag ("Blackish sample ratio ~{0}% ({1}/{2})" -f $pct, $sampleBlack, $samples)
  if ($pct -ge 98) {
    Write-Diag "WARNING: capture looks almost entirely black (common on Actions OpenGL)."
  }
} catch {
  Write-Diag ("Pixel probe skipped: {0}" -f $_.Exception.Message)
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
