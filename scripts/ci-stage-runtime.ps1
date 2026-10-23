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

# CI overlay: empty musicengine filename so main skips FMOD without waiting on LoadLibrary.
$ciMainCfg = Join-Path $fuck "main.cfg"
if (Test-Path $ciMainCfg) {
  $cfg = Get-Content -Raw $ciMainCfg
  if ($cfg -match "musicengine_filename=") {
    $cfg = $cfg -replace "musicengine_filename=.*", "musicengine_filename="
    $cfg = $cfg -replace "musicengine_name=.*", "musicengine_name="
    Set-Content -Path $ciMainCfg -Value $cfg.TrimEnd() -NoNewline
    Add-Content -Path $ciMainCfg -Value ""
    Write-Host "  Cleared musicengine_* in main.cfg for FMOD-free CI run"
  }
}

Write-Host "=== fuck/ staging summary ==="
Get-ChildItem $fuck -File | Sort-Object Name | ForEach-Object {
  Write-Host ("  {0,12}  {1}" -f $_.Length, $_.Name)
}
Write-Host "Staging done."
