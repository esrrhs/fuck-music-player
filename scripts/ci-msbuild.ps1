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
# Build one solution project at a time: MSBuild /m + /t:A;B;C incorrectly
# forwards every target name into each .vcxproj (MSB4057).
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
)

$common = @(
  "/m",
  "/p:Configuration=Release",
  "/p:Platform=Win32",
  "/p:PlatformToolset=v143",
  "/p:WindowsTargetPlatformVersion=10.0",
  "/v:minimal"
)

Write-Host "MSBuild: $msbuild"
Write-Host "Targets: $($targets -join ', ')"

foreach ($t in $targets) {
  Write-Host ""
  Write-Host "=== Building $t ==="
  & $msbuild "FuckMPlayer.sln" @common "/t:$t"
  if ($LASTEXITCODE -ne 0) {
    Write-Host "MSBuild failed on target $t (exit $LASTEXITCODE)"
    exit $LASTEXITCODE
  }
}

$exe = Get-ChildItem -Recurse -Filter "FuckMPlayer.exe" -ErrorAction SilentlyContinue |
  Where-Object { $_.FullName -match "Release|fuck|bin" } |
  Select-Object -First 5
Write-Host "Built executables:"
$exe | ForEach-Object { Write-Host " - $($_.FullName)" }

# GUI Win32 app: no meaningful headless smoke run in Actions.
Write-Host "Compile finished. Skipping app launch (GUI-only; no headless smoke)."
