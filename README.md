# fuck-music-player

[![License: MIT](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![Build Status](https://github.com/esrrhs/fuck-music-player/actions/workflows/ci.yml/badge.svg?branch=master)](https://github.com/esrrhs/fuck-music-player/actions)
[![GitHub release](https://img.shields.io/github/v/release/esrrhs/fuck-music-player)](https://github.com/esrrhs/fuck-music-player/releases)
[![GitHub stars](https://img.shields.io/github/stars/esrrhs/fuck-music-player)](https://github.com/esrrhs/fuck-music-player/stargazers)

[English](README.md) | [中文说明](README_CN.md)

Plugin-based Windows MP3 player built with OpenGL, wxWidgets, CEGUI, FMOD, Boost, ZeroMQ, Lua, and Protocol Buffers.

This repository is a historical Visual Studio (Win32) project from around 2011. Dependencies are vendored in-tree. Treat it as an archive / study sample rather than a modern cross-platform app.

## Features

- Host shell (`FuckMPlayer.exe`) loads plugins as DLLs
- Plugin modules for frame/process/thread orchestration, logo splash, music engine (FMOD), and file finder
- OpenGL + CEGUI UI, wxWidgets windowing, ZeroMQ messaging, Lua scripting hooks
- Config-driven module graph via `fuck/*.cfg`

## Requirements

- Windows (Win32)
- Visual Studio with C++ desktop workload (original projects use VS 2010-era `ToolsVersion="4.0"`)
- Open `FuckMPlayer.sln` and build **Release | Win32** (or Debug)

Third-party trees are included: `wxWidgets-2.9.1/`, `CEGUI-0.7.5/`, `zeromq-2.1.7/`, `protobuf-2.4.1/`, `boost/`, `fmod/`, `opengl/`.

## Build

```bat
:: Open the solution in Visual Studio, then build Release|Win32
FuckMPlayer.sln
```

After a successful build, copy plugin DLLs and the exe into the runtime folder:

```bat
fuck_release.bat
```

Debug equivalent:

```bat
fuck_debug.bat
```

Runtime directory: `fuck/` (dependency DLLs, `*.cfg`, UI assets). Launch `FuckMPlayer.exe` from that folder after copying build outputs.

## Layout

| Path | Role |
|------|------|
| `FuckMPlayer.sln` / `FuckMPlayer.vcxproj` | Host application |
| `main/` | Main plugin (render / music / UI glue) |
| `plugin/` | Plugin SDK + modules (thread, process, logo, musicengine, filefinder, …) |
| `frame/` | wxWidgets main frame |
| `common/` | Shared headers / logger |
| `fuck/` | Runtime snapshot (DLLs, configs, UI) |
| `VERSION` | Release version source of truth |

## Release

GitHub Actions watches **`VERSION`** on `master`. When the version string changes and the tag does not yet exist, `release.yml` runs `./pack.sh` and publishes a GitHub Release with the Windows runtime snapshot zip.

Manual packaging:

```bash
./pack.sh
# -> pack/fuck-music-player-<version>-windows-x86.zip and pack.zip
```

## License

MIT. See [LICENSE](LICENSE).

Vendored third-party libraries (wxWidgets, CEGUI, ZeroMQ, protobuf, Boost, FMOD, etc.) keep their own licenses; FMOD in particular is proprietary and not covered by the MIT grant above.
