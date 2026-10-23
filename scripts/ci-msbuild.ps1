# Build FuckMPlayer.sln (Release|Win32) on windows-latest.
# Skips musicengine (needs proprietary FMOD) and logo (needs Windows OpenGL/GLUT import libs).
$ErrorActionPreference = "Stop"
$root = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)
Set-Location $root

$msbuild = $env:MSBUILD_PATH
if (-not $msbuild) {
  $msbuild = "msbuild"
}

# Solution-folder\ProjectName paths required for nested projects (MSB4057 otherwise).
# Build one target per invocation so /m does not fan every target into every vcxproj.
# boost / main / FuckMPlayer are top-level in the .sln.
$targets = @(
  "boost",
  "protobuf\libprotobuf",
  "zeromq\libzmq",
  "cegui\CEGUIBase",
  "cegui\CEGUIOpenGLRenderer",
  "cegui\CEGUIExpatParser",
  "cegui\CEGUIFalagardWRBase",
  "cegui\CEGUISTBImageCodec",
  "cegui\CEGUITGAImageCodec",
  "wxWidgets\base",
  "wxWidgets\core",
  "wxWidgets\adv",
  "wxWidgets\net",
  "wxWidgets\xml",
  "wxWidgets\wxexpat",
  "wxWidgets\wxjpeg",
  "wxWidgets\wxpng",
  "wxWidgets\wxregex",
  "wxWidgets\wxtiff",
  "wxWidgets\wxzlib",
  "protobuf\protobufmessage",
  "plugin\thread",
  "plugin\process",
  "plugin\filefinder",
  "plugin\frameadapter",
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
Write-Host "Targets: $($targets.Count)"

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
