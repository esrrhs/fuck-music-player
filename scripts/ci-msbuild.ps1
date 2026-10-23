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
  "cegui\tolua++",
  "cegui\CEGUILuaScriptModule",
  "cegui\CEGUIDirect3D9Renderer",
  # Builtin image/compression libs before base/core (wx_base links wxzlib/wxregexu/wxexpat).
  "wxWidgets\wxzlib",
  "wxWidgets\wxregex",
  "wxWidgets\wxexpat",
  "wxWidgets\wxjpeg",
  "wxWidgets\wxpng",
  "wxWidgets\wxtiff",
  "wxWidgets\base",
  "wxWidgets\net",
  "wxWidgets\xml",
  "wxWidgets\core",
  "wxWidgets\adv",
  "protobuf\protobufmessage",
  "plugin\thread",
  "plugin\process",
  "plugin\filefinder",
  "plugin\frameadapter"
)

# Built as vcxproj (not solution /t) so we do not pull sln ProjectDependencies:
# optional CEGUI codecs, musicengine (FMOD), logo (GLUT import libs).
$directProjects = @(
  "main\main.vcxproj",
  "FuckMPlayer.vcxproj"
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
Write-Host "Targets: $($targets.Count) + $($directProjects.Count) direct"

foreach ($t in $targets) {
  Write-Host ""
  Write-Host "=== Building $t ==="
  & $msbuild "FuckMPlayer.sln" @common "/t:$t"
  if ($LASTEXITCODE -ne 0) {
    Write-Host "MSBuild failed on target $t (exit $LASTEXITCODE)"
    exit $LASTEXITCODE
  }
}

# vcxproj-direct builds do not inherit SolutionDir from the .sln; projects
# use $(SolutionDir)... for IncludePath / AdditionalLibraryDirectories.
$solutionDir = $root
if (-not $solutionDir.EndsWith('\') -and -not $solutionDir.EndsWith('/')) {
  $solutionDir = $solutionDir + '\'
}

foreach ($proj in $directProjects) {
  Write-Host ""
  Write-Host "=== Building $proj (direct, no sln ProjectDependencies) ==="
  & $msbuild $proj @common "/p:SolutionDir=$solutionDir"
  if ($LASTEXITCODE -ne 0) {
    Write-Host "MSBuild failed on $proj (exit $LASTEXITCODE)"
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
