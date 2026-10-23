# Build FuckMPlayer.sln (Release|Win32) on windows-latest.
# Skips musicengine (needs proprietary FMOD) and logo (needs Windows OpenGL/GLUT import libs).
$ErrorActionPreference = "Stop"
$root = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)
Set-Location $root

$msbuild = $env:MSBUILD_PATH
if (-not $msbuild) {
  $msbuild = "msbuild"
}

# Build .vcxproj files directly. Solution /t:Name fails for projects nested in
# solution folders on modern MSBuild (MSB4057), and /t:A;B with /m forwards
# every target into each project. Order matters for dependents.
$projects = @(
  "boost\boost.vcxproj",
  "protobuf-2.4.1\vsprojects\libprotobuf.vcxproj",
  "zeromq-2.1.7\builds\msvc\libzmq\libzmq.vcxproj",
  "CEGUI-0.7.5\projects\premake\BaseSystem\CEGUIBase.vcxproj",
  "CEGUI-0.7.5\projects\premake\RendererModules\OpenGL\CEGUIOpenGLRenderer.vcxproj",
  "CEGUI-0.7.5\projects\premake\XMLParserModules\expatParser\CEGUIExpatParser.vcxproj",
  "CEGUI-0.7.5\projects\premake\WindowRendererSets\Falagard\CEGUIFalagardWRBase.vcxproj",
  "CEGUI-0.7.5\projects\premake\ImageCodecModules\STBImageCodec\CEGUISTBImageCodec.vcxproj",
  "CEGUI-0.7.5\projects\premake\ImageCodecModules\TGAImageCodec\CEGUITGAImageCodec.vcxproj",
  "wxWidgets-2.9.1\build\msw\wx_base.vcxproj",
  "wxWidgets-2.9.1\build\msw\wx_core.vcxproj",
  "wxWidgets-2.9.1\build\msw\wx_adv.vcxproj",
  "wxWidgets-2.9.1\build\msw\wx_net.vcxproj",
  "wxWidgets-2.9.1\build\msw\wx_xml.vcxproj",
  "wxWidgets-2.9.1\build\msw\wx_wxexpat.vcxproj",
  "wxWidgets-2.9.1\build\msw\wx_wxjpeg.vcxproj",
  "wxWidgets-2.9.1\build\msw\wx_wxpng.vcxproj",
  "wxWidgets-2.9.1\build\msw\wx_wxregex.vcxproj",
  "wxWidgets-2.9.1\build\msw\wx_wxtiff.vcxproj",
  "wxWidgets-2.9.1\build\msw\wx_wxzlib.vcxproj",
  "protobufmessage\protobufmessage.vcxproj",
  "plugin\build\thread.vcxproj",
  "plugin\build\process.vcxproj",
  "plugin\build\filefinder.vcxproj",
  "plugin\build\frameadapter.vcxproj",
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
Write-Host "Projects: $($projects.Count)"

foreach ($proj in $projects) {
  if (-not (Test-Path $proj)) {
    Write-Host "Missing project file: $proj"
    exit 1
  }
  Write-Host ""
  Write-Host "=== Building $proj ==="
  & $msbuild $proj @common
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
