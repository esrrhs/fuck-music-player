# fuck-music-player

[![License: MIT](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![Build Status](https://github.com/esrrhs/fuck-music-player/actions/workflows/ci.yml/badge.svg?branch=master)](https://github.com/esrrhs/fuck-music-player/actions)
[![GitHub release](https://img.shields.io/github/v/release/esrrhs/fuck-music-player)](https://github.com/esrrhs/fuck-music-player/releases)
[![GitHub stars](https://img.shields.io/github/stars/esrrhs/fuck-music-player)](https://github.com/esrrhs/fuck-music-player/stargazers)

[English](README.md) | [中文说明](README_CN.md)

基于插件架构的 Windows MP3 播放器，技术栈包括 OpenGL、wxWidgets、CEGUI、FMOD、Boost、ZeroMQ、Lua、Protocol Buffers。

本仓库是约 2011 年的 Visual Studio（Win32）历史项目，依赖随仓库内置。更适合作为归档 / 学习样本，而不是现代跨平台应用。

## 特性

- 宿主程序（`FuckMPlayer.exe`）以 DLL 方式加载插件
- 插件覆盖帧/进程/线程编排、Logo 开屏、音乐引擎（FMOD）、文件扫描等
- OpenGL + CEGUI 界面，wxWidgets 窗口，ZeroMQ 消息，Lua 脚本钩子
- 通过 `fuck/*.cfg` 配置模块依赖图

## 环境要求

- Windows（Win32）
- 安装了 C++ 桌面开发工作负载的 Visual Studio（原工程为 VS 2010 时代的 `ToolsVersion="4.0"`）
- 打开 `FuckMPlayer.sln`，构建 **Release | Win32**（或 Debug）

第三方源码/库已内置：`wxWidgets-2.9.1/`、`CEGUI-0.7.5/`、`zeromq-2.1.7/`、`protobuf-2.4.1/`、`boost/`、`fmod/`、`opengl/`。

## 构建

```bat
:: 用 Visual Studio 打开解决方案，然后构建 Release|Win32
FuckMPlayer.sln
```

构建成功后，把插件 DLL 与 exe 拷贝到运行目录：

```bat
fuck_release.bat
```

Debug 对应：

```bat
fuck_debug.bat
```

运行目录为 `fuck/`（依赖 DLL、`*.cfg`、UI 资源）。拷贝构建产物后，在该目录启动 `FuckMPlayer.exe`。

## 目录结构

| 路径 | 作用 |
|------|------|
| `FuckMPlayer.sln` / `FuckMPlayer.vcxproj` | 宿主程序 |
| `main/` | 主插件（渲染 / 音乐 / UI 粘合） |
| `plugin/` | 插件 SDK 与模块（thread、process、logo、musicengine、filefinder 等） |
| `frame/` | wxWidgets 主窗口 |
| `common/` | 公共头文件 / 日志 |
| `fuck/` | 运行时快照（DLL、配置、UI） |
| `VERSION` | 发版版本号来源 |

## 发布

GitHub Actions 在 `master` 上监视 **`VERSION`**。当版本号字符串变化且对应 tag 尚不存在时，`release.yml` 会执行 `./pack.sh` 并发布 GitHub Release（Windows 运行时快照 zip）。

本地打包：

```bash
./pack.sh
# -> pack/fuck-music-player-<version>-windows-x86.zip 以及 pack.zip
```

## 许可证

MIT，见 [LICENSE](LICENSE)。

随仓库内置的第三方库（wxWidgets、CEGUI、ZeroMQ、protobuf、Boost、FMOD 等）各自保留其原有许可证；其中 FMOD 为专有软件，不在上述 MIT 授权范围内。
