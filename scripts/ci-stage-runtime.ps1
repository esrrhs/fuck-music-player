# Stage a runnable fuck/ tree from Release outputs + third-party runtime DLLs.
# musicengine.dll / fmodex.dll are intentionally omitted (proprietary FMOD).
$ErrorActionPreference = "Stop"
$root = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)
Set-Location $root

$fuck = Join-Path $root "fuck"
if (-not (Test-Path $fuck)) {
  throw "fuck/ runtime dir missing"
}

function Copy-IfExists($src, $destDir) {
  if (Test-Path $src) {
    Copy-Item -Force $src $destDir
    Write-Host "  + $(Split-Path -Leaf $src)"
    return $true
  }
  Write-Host "  - missing: $src"
  return $false
}

Write-Host "=== Staging plugin / host binaries into fuck/ ==="
$pluginDlls = @(
  "frameadapter.dll", "thread.dll", "process.dll", "logo.dll",
  "main.dll", "filefinder.dll"
)
foreach ($d in $pluginDlls) {
  $src = Join-Path $root "lib\Release\$d"
  if (-not (Copy-IfExists $src $fuck)) {
    throw "Required plugin DLL missing: $d"
  }
}
# musicengine is optional
Copy-IfExists (Join-Path $root "lib\Release\musicengine.dll") $fuck | Out-Null

$exeSrc = Join-Path $root "bin\Release\FuckMPlayer.exe"
if (-not (Copy-IfExists $exeSrc $fuck)) {
  throw "FuckMPlayer.exe missing under bin\Release"
}

Write-Host "=== Staging CEGUI Release DLLs ==="
$ceguiDir = Join-Path $root "lib\Release\cegui"
if (Test-Path $ceguiDir) {
  Get-ChildItem $ceguiDir -Filter "*.dll" | ForEach-Object {
    Copy-Item -Force $_.FullName $fuck
    Write-Host "  + $($_.Name)"
  }
} else {
  Write-Host "  warning: $ceguiDir missing"
}

Write-Host "=== Staging libzmq ==="
Copy-IfExists (Join-Path $root "lib\Release\libzmq.dll") $fuck | Out-Null

Write-Host "=== Staging wxWidgets DLLs ==="
$wxDirs = @(
  (Join-Path $root "lib\Release\wx"),
  (Join-Path $root "wxWidgets-2.9.1\lib\vc_dll")
)
foreach ($wd in $wxDirs) {
  if (Test-Path $wd) {
    Get-ChildItem $wd -Filter "wx*.dll" -ErrorAction SilentlyContinue | ForEach-Object {
      Copy-Item -Force $_.FullName $fuck
      Write-Host "  + $($_.Name) (from $wd)"
    }
  }
}

Write-Host "=== Staging CEGUI-DEPS / freeglut / lua runtime ==="
$depsBin = Join-Path $root "CEGUI-0.7.5\dependencies\bin"
if (Test-Path $depsBin) {
  foreach ($name in @("freeglut.dll", "lua.dll", "tolua++.dll", "SILLY.dll", "DevIL.dll", "ILU.dll", "FreeImage.dll", "xerces-c_3_1.dll")) {
    Copy-IfExists (Join-Path $depsBin $name) $fuck | Out-Null
  }
  # Copy any remaining dependency DLLs that plugins may load
  Get-ChildItem $depsBin -Filter "*.dll" -ErrorAction SilentlyContinue | ForEach-Object {
    $dest = Join-Path $fuck $_.Name
    if (-not (Test-Path $dest)) {
      Copy-Item -Force $_.FullName $fuck
      Write-Host "  + $($_.Name) (extra deps)"
    }
  }
}

Write-Host "=== Staging OpenGL GLUT runtime (optional) ==="
foreach ($g in @("glut32.dll", "glut.dll", "freeglut.dll")) {
  Copy-IfExists (Join-Path $root "opengl\lib\$g") $fuck | Out-Null
}

Write-Host "=== Staging D3DX9 runtime (required by CEGUIDirect3D9Renderer) ==="
# Without d3dx9_43.dll, LoadLibrary(main.dll) fails and the player shell stays blank grey.
$d3dxOk = $false
foreach ($src in @(
  (Join-Path $root "CEGUI-0.7.5\dependencies\bin\d3dx9_43.dll"),
  (Join-Path $root "CEGUI-0.7.5\dependencies\bin\D3DX9_43.dll")
)) {
  if (Copy-IfExists $src $fuck) { $d3dxOk = $true; break }
}
Copy-IfExists (Join-Path $root "CEGUI-0.7.5\dependencies\bin\D3DCompiler_43.dll") $fuck | Out-Null
if (-not $d3dxOk) {
  throw "d3dx9_43.dll missing — run scripts/fetch-deps.sh (copies from microsoft.dxsdk.d3dx nupkg)"
}

Write-Host "=== Staging VC++ 2010 CRT (MSVCR100 for lua.dll from CEGUI-DEPS) ==="
# lua.dll (CEGUI-DEPS vc10) imports MSVCR100.dll; windows-latest often lacks VC++ 2010 x86.
$msvcrDest = Join-Path $fuck "msvcr100.dll"
if (-not (Test-Path $msvcrDest)) {
  $candidates = @(
    (Join-Path $env:SystemRoot "SysWOW64\msvcr100.dll"),
    (Join-Path $env:SystemRoot "System32\msvcr100.dll"),
    (Join-Path $root "msvc-overlays\runtime\msvcr100.dll")
  )
  $found = $false
  foreach ($c in $candidates) {
    if (Test-Path $c) {
      Copy-Item -Force $c $msvcrDest
      Write-Host "  + msvcr100.dll (from $c)"
      $found = $true
      break
    }
  }
  if (-not $found) {
    Write-Host "  MSVCR100 not on runner; downloading VC++ 2010 SP1 x86 redistributable..."
    $vc = Join-Path $env:RUNNER_TEMP "vcredist_x86_2010.exe"
    if (-not $env:RUNNER_TEMP) { $vc = Join-Path $root "vcredist_x86_2010.exe" }
    $uri = "https://download.microsoft.com/download/1/6/5/165255E7-1014-4D0A-B094-B6A430A6BFFC/vcredist_x86.exe"
    Invoke-WebRequest -Uri $uri -OutFile $vc -UseBasicParsing
    $extract = Join-Path $env:RUNNER_TEMP "vcredist2010-x86"
    if (-not $env:RUNNER_TEMP) { $extract = Join-Path $root "vcredist2010-x86" }
    New-Item -ItemType Directory -Force -Path $extract | Out-Null
    # Silent install puts MSVCR100 into SysWOW64; also try 7z extract as fallback.
    $p = Start-Process -FilePath $vc -ArgumentList "/q","/norestart" -Wait -PassThru
    Write-Host "  vcredist exit=$($p.ExitCode)"
    foreach ($c in @(
      (Join-Path $env:SystemRoot "SysWOW64\msvcr100.dll"),
      (Join-Path $env:SystemRoot "System32\msvcr100.dll")
    )) {
      if (Test-Path $c) {
        Copy-Item -Force $c $msvcrDest
        Write-Host "  + msvcr100.dll (from $c after vcredist)"
        $found = $true
        break
      }
    }
    if (-not $found) {
      # 7z can unpack the exe; search for the DLL
      $seven = Get-Command 7z -ErrorAction SilentlyContinue
      if ($seven) {
        & 7z x -y "-o$extract" $vc | Out-Null
        $hit = Get-ChildItem -Path $extract -Recurse -Filter "msvcr100.dll" -ErrorAction SilentlyContinue | Select-Object -First 1
        if ($hit) {
          Copy-Item -Force $hit.FullName $msvcrDest
          Write-Host "  + msvcr100.dll (extracted from vcredist)"
          $found = $true
        }
      }
    }
  }
  if (-not (Test-Path $msvcrDest)) {
    throw "msvcr100.dll missing after VC++ 2010 install/extract — lua.dll cannot load"
  }
} else {
  Write-Host "  msvcr100.dll already in fuck/"
}

# CI overlay: empty musicengine filename so main skips FMOD without waiting on LoadLibrary.
$ciMainCfg = Join-Path $fuck "main.cfg"
if (Test-Path $ciMainCfg) {
  # Omit modules=/musicengine_= lines: common/Config.h tokenizer UB on empty "key=" values.
  @"
fps=30
filefinder_filename=filefinder.dll
filefinder_name=filefinder
"@ | Set-Content -Path $ciMainCfg -Encoding ascii
  Write-Host "  Rewrote main.cfg for FMOD-free CI (no modules/musicengine keys)"
}

# Skip OpenGL logo splash on CI:
# 1) Logo SetPixelFormat(OpenGL) on the wx HWND poisons later D3D9 CreateDevice.
# 2) Screenshot PrintWindow cannot see GL pixels (solid grey client area).
# Jump straight to main.dll so CEGUI/D3D UI can paint on a clean window.
$ciProcessCfg = Join-Path $fuck "process.cfg"
@"
modules=main.dll
module_names=main
"@ | Set-Content -Path $ciProcessCfg -Encoding ascii
Write-Host "  Rewrote process.cfg to skip logo.dll (main only) for CI UI capture"

# Local music scan dir so filefinder finishes quickly and loading/wheel UI has a path.
# Several distinctly named stubs so the wheel list shows multiple rows (not one title).
$ciMusic = Join-Path $fuck "ci-music"
New-Item -ItemType Directory -Force -Path $ciMusic | Out-Null
# Wipe prior stubs so the list is exactly what we stage (avoid stale single-file dirs).
Get-ChildItem $ciMusic -File -ErrorAction SilentlyContinue | Remove-Item -Force
$stubId3 = [byte[]](0x49,0x44,0x33)  # "ID3" - filefinder only matches extension
# Enough stubs to fill the 5-slot wheel (2 above + current + 2 below) after scrolling.
$ciTracks = @(
  "aurora-dreams.mp3",
  "blue-horizon.mp3",
  "crystal-lake.wav",
  "dusk-waltz.mp3",
  "echo-station.mp3",
  "forest-path.wav",
  "golden-hour.mp3",
  "harbor-lights.mp3",
  "ivory-keys.wav",
  "jade-garden.mp3",
  "kinetic-pulse.mp3",
  "lunar-tide.wav",
  "midnight-express.mp3",
  "neon-alley.mp3",
  "orbit-waltz.wav"
)
foreach ($name in $ciTracks) {
  [System.IO.File]::WriteAllBytes((Join-Path $ciMusic $name), $stubId3)
  Write-Host "  + ci-music/$name"
}
$ciFinderCfg = Join-Path $fuck "filefinder.cfg"
@"
dir=$ciMusic
filter=.mp3;.wav
sleep_time=0
thread_num=2
"@ | Set-Content -Path $ciFinderCfg -Encoding ascii
Write-Host "  Pointed filefinder.cfg at fuck/ci-music ($($ciTracks.Count) stub tracks)"

Write-Host "=== fuck/ staging summary ==="
Get-ChildItem $fuck -File | Sort-Object Name | ForEach-Object {
  Write-Host ("  {0,12}  {1}" -f $_.Length, $_.Name)
}
Write-Host "=== Probe LoadLibrary(main.dll) via 32-bit PowerShell ==="
# Host pwsh on windows-latest is 64-bit; loading Win32 main.dll yields ERROR_BAD_EXE_FORMAT (193).
$wow = "$env:SystemRoot\SysWOW64\WindowsPowerShell\v1.0\powershell.exe"
if (-not (Test-Path $wow)) {
  Write-Host "  warning: SysWOW64 powershell missing; skip 32-bit LoadLibrary probe"
} else {
  $probeScript = @"
`$ErrorActionPreference = 'Continue'
Add-Type -Namespace CiProbe -Name Native -MemberDefinition @'
  [System.Runtime.InteropServices.DllImport("kernel32", SetLastError=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)]
  public static extern System.IntPtr LoadLibraryW(string path);
  [System.Runtime.InteropServices.DllImport("kernel32")]
  public static extern uint GetLastError();
  [System.Runtime.InteropServices.DllImport("kernel32", SetLastError=true)]
  public static extern bool FreeLibrary(System.IntPtr h);
  [System.Runtime.InteropServices.DllImport("kernel32", CharSet=System.Runtime.InteropServices.CharSet.Unicode)]
  public static extern bool SetDllDirectory(string path);
'@
[CiProbe.Native]::SetDllDirectory('$fuck') | Out-Null
`$h = [CiProbe.Native]::LoadLibraryW('$(Join-Path $fuck "main.dll")')
`$err = [CiProbe.Native]::GetLastError()
if (`$h -eq [IntPtr]::Zero) {
  Write-Output "FAIL `$(`$err)"
  exit 1
} else {
  Write-Output ("OK 0x{0:X}" -f `$h.ToInt64())
  [void][CiProbe.Native]::FreeLibrary(`$h)
  exit 0
}
"@
  $out = & $wow -NoProfile -Command $probeScript 2>&1
  Write-Host "  32-bit probe: $out"
  if ($LASTEXITCODE -ne 0) {
    Write-Host "  Diagnosing missing dependency (32-bit LoadLibrary per DLL)..."
    $deps = @("d3dx9_43.dll","lua.dll","tolua++.dll","CEGUIBase.dll","CEGUIDirect3D9Renderer.dll","CEGUILuaScriptModule.dll","libzmq.dll","msvcr100.dll","main.dll")
    $diag = @"
Add-Type -Namespace CiProbe2 -Name Native -MemberDefinition @'
  [System.Runtime.InteropServices.DllImport("kernel32", SetLastError=true, CharSet=System.Runtime.InteropServices.CharSet.Unicode)]
  public static extern System.IntPtr LoadLibraryW(string path);
  [System.Runtime.InteropServices.DllImport("kernel32")]
  public static extern uint GetLastError();
  [System.Runtime.InteropServices.DllImport("kernel32", SetLastError=true)]
  public static extern bool FreeLibrary(System.IntPtr h);
  [System.Runtime.InteropServices.DllImport("kernel32", CharSet=System.Runtime.InteropServices.CharSet.Unicode)]
  public static extern bool SetDllDirectory(string path);
'@
[CiProbe2.Native]::SetDllDirectory('$fuck') | Out-Null
foreach (`$d in @($(($deps | ForEach-Object { "'$_'" }) -join ','))) {
  `$path = Join-Path '$fuck' `$d
  if (-not (Test-Path `$path)) { Write-Output ("MISSING_FILE `$(`$d)"); continue }
  [System.Runtime.InteropServices.Marshal]::GetLastWin32Error() | Out-Null
  `$h = [CiProbe2.Native]::LoadLibraryW(`$path)
  `$err = [CiProbe2.Native]::GetLastError()
  if (`$h -eq [IntPtr]::Zero) { Write-Output ("FAIL `$(`$d) err=`$(`$err)") }
  else { Write-Output ("OK `$(`$d)"); [void][CiProbe2.Native]::FreeLibrary(`$h) }
}
"@
    $dout = & $wow -NoProfile -Command $diag 2>&1
    $dout | ForEach-Object { Write-Host "    $_" }
    throw "32-bit LoadLibrary(main.dll) failed: $out"
  }
}

Write-Host "Staging done."

