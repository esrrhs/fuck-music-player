#!/usr/bin/env bash
# Fetch third-party dependency trees into the paths expected by FuckMPlayer.sln.
# Open-source archives are downloaded; FMOD (proprietary) must be supplied manually.
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"
CACHE="${DEPS_CACHE:-$ROOT/.deps-cache}"
mkdir -p "$CACHE"

download() {
  local url="$1" out="$2"
  if [ -f "$out" ]; then
    echo "Using cached $(basename "$out")"
    return 0
  fi
  echo "Downloading $url"
  curl -fL --retry 3 --retry-delay 2 -o "$out.partial" "$url"
  mv "$out.partial" "$out"
}

extract_tar_to() {
  local archive="$1" dest="$2" strip_components="${3:-1}"
  rm -rf "$dest"
  mkdir -p "$dest"
  tar -xf "$archive" -C "$dest" --strip-components="$strip_components"
}

echo "==> wxWidgets 2.9.1"
WX_ARC="$CACHE/wxWidgets-2.9.1.tar.bz2"
download "https://github.com/wxWidgets/wxWidgets/releases/download/v2.9.1/wxWidgets-2.9.1.tar.bz2" "$WX_ARC"
extract_tar_to "$WX_ARC" "wxWidgets-2.9.1"

echo "==> protobuf 2.4.1"
PB_ARC="$CACHE/protobuf-2.4.1.tar.gz"
download "https://github.com/protocolbuffers/protobuf/releases/download/v2.4.1/protobuf-2.4.1.tar.gz" "$PB_ARC"
# Preserve tracked VS project overlay
PB_OVERLAY="$(mktemp -d)"
if [ -d protobuf-2.4.1/vsprojects ]; then
  cp -a protobuf-2.4.1/vsprojects "$PB_OVERLAY/"
fi
extract_tar_to "$PB_ARC" "protobuf-2.4.1"
if [ -d "$PB_OVERLAY/vsprojects" ]; then
  rm -rf protobuf-2.4.1/vsprojects
  cp -a "$PB_OVERLAY/vsprojects" protobuf-2.4.1/
fi
rm -rf "$PB_OVERLAY"

echo "==> Boost 1.46.1"
BOOST_ARC="$CACHE/boost_1_46_1.tar.bz2"
download "https://archives.boost.io/release/1.46.1/source/boost_1_46_1.tar.bz2" "$BOOST_ARC"
BOOST_TMP="$(mktemp -d)"
tar -xf "$BOOST_ARC" -C "$BOOST_TMP"
# Keep tracked boost.vcxproj + ex/; replace headers and libs
mkdir -p boost
rm -rf boost/boost boost/libs
cp -a "$BOOST_TMP/boost_1_46_1/boost" boost/
cp -a "$BOOST_TMP/boost_1_46_1/libs" boost/
rm -rf "$BOOST_TMP"
if [ ! -f boost/boost.vcxproj ]; then
  echo "error: boost/boost.vcxproj missing (project overlay)" >&2
  exit 1
fi

echo "==> ZeroMQ 2.1.7"
ZMQ_ARC="$CACHE/zeromq-2.1.7.tar.gz"
download "https://github.com/zeromq/zeromq2-x/archive/refs/tags/v2.1.7.tar.gz" "$ZMQ_ARC"
ZMQ_OVERLAY="$(mktemp -d)"
if [ -d zeromq-2.1.7/builds ]; then
  cp -a zeromq-2.1.7/builds "$ZMQ_OVERLAY/"
fi
extract_tar_to "$ZMQ_ARC" "zeromq-2.1.7"
if [ -d "$ZMQ_OVERLAY/builds" ]; then
  rm -rf zeromq-2.1.7/builds
  cp -a "$ZMQ_OVERLAY/builds" zeromq-2.1.7/
fi
rm -rf "$ZMQ_OVERLAY"

echo "==> CEGUI 0.7.5"
CEGUI_ARC="$CACHE/CEGUI-0.7.5.tar.gz"
download "https://sourceforge.net/projects/crayzedsgui/files/CEGUI%20Mk-2/0.7.5/CEGUI-0.7.5.tar.gz/download" "$CEGUI_ARC"
CEGUI_OVERLAY="$(mktemp -d)"
if [ -d CEGUI-0.7.5/projects ]; then
  cp -a CEGUI-0.7.5/projects "$CEGUI_OVERLAY/"
fi
extract_tar_to "$CEGUI_ARC" "CEGUI-0.7.5"
if [ -d "$CEGUI_OVERLAY/projects" ]; then
  rm -rf CEGUI-0.7.5/projects
  cp -a "$CEGUI_OVERLAY/projects" CEGUI-0.7.5/
fi
rm -rf "$CEGUI_OVERLAY"

echo "==> freeglut 3.0.0 -> opengl/ (headers; place Windows libs manually if needed)"
FG_ARC="$CACHE/freeglut-3.0.0.tar.gz"
download "https://github.com/freeglut/freeglut/releases/download/v3.0.0/freeglut-3.0.0.tar.gz" "$FG_ARC"
FG_TMP="$(mktemp -d)"
tar -xf "$FG_ARC" -C "$FG_TMP"
rm -rf opengl
mkdir -p opengl/include opengl/lib
# Map freeglut GL headers into the historical opengl/include layout when present
if [ -d "$FG_TMP/freeglut-3.0.0/include/GL" ]; then
  cp -a "$FG_TMP/freeglut-3.0.0/include/GL/." opengl/include/
fi
rm -rf "$FG_TMP"
echo "    Note: historical tree also shipped Windows GLUT/OpenGL import libs under opengl/lib."
echo "    Build freeglut for Win32 or copy libs into opengl/lib/ as needed."

echo "==> protoc 2.4.1 (Windows) -> protobufmessage/protoc.exe"
PROTOC_ARC="$CACHE/protoc-2.4.1-win32.zip"
download "https://github.com/protocolbuffers/protobuf/releases/download/v2.4.1/protoc-2.4.1-win32.zip" "$PROTOC_ARC"
PROTOC_TMP="$(mktemp -d)"
unzip -qo "$PROTOC_ARC" -d "$PROTOC_TMP"
if [ -f "$PROTOC_TMP/protoc.exe" ]; then
  cp -a "$PROTOC_TMP/protoc.exe" protobufmessage/protoc.exe
elif [ -f "$PROTOC_TMP/bin/protoc.exe" ]; then
  cp -a "$PROTOC_TMP/bin/protoc.exe" protobufmessage/protoc.exe
else
  echo "warning: protoc.exe not found in archive layout; extract manually from $PROTOC_ARC" >&2
fi
rm -rf "$PROTOC_TMP"

echo "==> FMOD Ex (proprietary)"
if [ -n "${FMOD_SDK:-}" ] && [ -d "$FMOD_SDK" ]; then
  rm -rf fmod
  mkdir -p fmod
  cp -a "$FMOD_SDK"/. fmod/
  echo "    Copied FMOD SDK from FMOD_SDK=$FMOD_SDK"
else
  mkdir -p fmod/inc fmod/lib
  cat > fmod/README.fetch.md << 'FMOD'
# FMOD Ex SDK (not redistributed)

This project historically used FMOD Ex (`fmodex.dll` / `fmodex_vc.lib`, headers under `inc/`).

FMOD is proprietary. Obtain an SDK from Firelight Technologies, then either:

- Set `FMOD_SDK` to the unpacked SDK path and re-run `scripts/fetch-deps.sh`, or
- Copy headers into `fmod/inc/`, the import lib into `fmod/lib/`, and `fmodex.dll` next to the runtime.
FMOD
  echo "    Skipped: set FMOD_SDK to an unpacked FMOD Ex SDK directory to install into fmod/"
fi

echo "==> CELayoutEditor 0.7.1 (optional tool, not required by FuckMPlayer.sln)"
echo "    Skipped by default. Install separately if you need the CEGUI layout editor."

echo ""
echo "Done. Dependency trees are in place for a local Visual Studio Win32 build."
echo "Cache: $CACHE"
