# Build FuckMPlayer.sln (Release|Win32) on windows-latest.
# Skips musicengine (needs proprietary FMOD) and logo (needs Windows OpenGL/GLUT import libs).
$ErrorActionPreference = "Stop"
$root = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)
Set-Location $root

$msbuild = $env:MSBUILD_PATH
if (-not $msbuild) {
  $msbuild = "msbuild"
}

# Targets that do not require FMOD / GLUT import libs.
# Plugins musicengine + logo remain in the sln for local builds with those SDKs installed.
$targets = @(
  "boost",
  "libprotobuf",
  "libzmq",
  "CEGUIBase",
  "CEGUIOpenGLRenderer",
  "CEGUIExpatParser",
  "CEGUIFalagardWRBase",
  "CEGUISTBImageCodec",
  "CEGUITGAImageCodec",
  "base",
  "core",
  "adv",
  "net",
  "xml",
  "wxexpat",
  "wxjpeg",
  "wxpng",
  "wxregex",
  "wxtiff",
  "wxzlib",
  "protobufmessage",
  "thread",
  "process",
  "filefinder",
  "frameadapter",
  "main",
  "FuckMPlayer"
) -join ";"

Write-Host "MSBuild: $msbuild"
Write-Host "Targets: $targets"

& $msbuild "FuckMPlayer.sln" `
  /m `
  /p:Configuration=Release `
  /p:Platform=Win32 `
  /p:PlatformToolset=v143 `
  /p:WindowsTargetPlatformVersion=10.0 `
  /t:$targets `
  /v:minimal

if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

$exe = Get-ChildItem -Recurse -Filter "FuckMPlayer.exe" -ErrorAction SilentlyContinue |
  Where-Object { $_.FullName -match "Release|fuck|bin" } |
  Select-Object -First 5
Write-Host "Built executables:"
$exe | ForEach-Object { Write-Host " - $($_.FullName)" }

# GUI Win32 app: no meaningful headless smoke run in Actions.
Write-Host "Compile finished. Skipping app launch (GUI-only; no headless smoke)."
