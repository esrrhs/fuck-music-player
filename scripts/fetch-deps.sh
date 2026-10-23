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
  if [ -f "$out" ] && [ -s "$out" ]; then
    echo "Using cached $(basename "$out")"
    return 0
  fi
  echo "Downloading $url"
  curl -fL --retry 3 --retry-delay 2 -o "$out.partial" "$url"
  mv "$out.partial" "$out"
}

# Portable tar extract: Windows Git Bash tar exits non-zero when it cannot
# create symlinks (e.g. ZeroMQ 2.1.7 doc/zmq_epgm.txt -> zmq_pgm.txt). Those
# doc links are not needed for MSBuild. Accept the extract if only link errors
# were reported and the destination is non-empty. Ubuntu/Linux tar succeeds.
extract_tar() {
  # extract_tar <archive> <dest-dir> [strip-components] [gz|bz2|none]
  local archive="$1" dest="$2" strip="${3:-0}" compression="${4:-gz}"
  local err rc=0 had_errexit=0
  case $- in *e*) had_errexit=1 ;; esac
  err="$(mktemp)"
  mkdir -p "$dest"

  set +e
  case "$compression" in
    gz)
      tar -xzf "$archive" -C "$dest" --strip-components="$strip" 2>"$err"
      rc=$?
      ;;
    bz2)
      tar -xjf "$archive" -C "$dest" --strip-components="$strip" 2>"$err"
      rc=$?
      if [ "$rc" -ne 0 ] && ! grep -Eqi 'Cannot create symlink|Cannot hard link|Cannot create hard link' "$err"; then
        # Fallback when system tar lacks bzip2 support
        if command -v bunzip2 >/dev/null 2>&1; then
          bunzip2 -c "$archive" | tar -xf - -C "$dest" --strip-components="$strip" 2>"$err"
          rc=$?
        else
          python3 - "$archive" "$dest" "$strip" <<'PY'
import sys, tarfile
arc, dest, strip = sys.argv[1], sys.argv[2], int(sys.argv[3])
with tarfile.open(arc, "r:bz2") as tf:
    for m in tf.getmembers():
        parts = m.name.split("/")
        if len(parts) <= strip:
            continue
        m.name = "/".join(parts[strip:])
        if not m.name or m.name.startswith("/") or ".." in m.name.split("/"):
            continue
        try:
            tf.extract(m, dest)
        except (OSError, tarfile.ExtractError):
            if m.issym() or m.islnk():
                continue
            raise
PY
          rc=$?
        fi
      fi
      ;;
    none)
      tar -xf "$archive" -C "$dest" --strip-components="$strip" 2>"$err"
      rc=$?
      ;;
    *)
      echo "error: unknown compression: $compression" >&2
      rm -f "$err"
      [ "$had_errexit" -eq 1 ] && set -e
      return 1
      ;;
  esac

  if [ "$rc" -ne 0 ]; then
    if grep -Eqi 'Cannot create symlink|Cannot hard link|Cannot create hard link' "$err" \
      && [ -n "$(find "$dest" -mindepth 1 -print -quit 2>/dev/null)" ]; then
      echo "warning: tar reported link creation failures (common on Windows); keeping extract of $(basename "$archive")" >&2
      grep -Ei 'Cannot create symlink|Cannot hard link|Cannot create hard link|Exiting with failure' "$err" >&2 || true
      rc=0
    else
      cat "$err" >&2
      rm -f "$err"
      [ "$had_errexit" -eq 1 ] && set -e
      return "$rc"
    fi
  fi
  rm -f "$err"
  [ "$had_errexit" -eq 1 ] && set -e
  return 0
}

extract_archive() {
  # extract_archive <archive> <dest-dir> [strip-components]
  local archive="$1" dest="$2" strip="${3:-1}"
  rm -rf "$dest"
  mkdir -p "$dest"
  case "$archive" in
    *.tar.gz|*.tgz)
      extract_tar "$archive" "$dest" "$strip" gz
      ;;
    *.tar.bz2|*.tbz2)
      extract_tar "$archive" "$dest" "$strip" bz2
      ;;
    *.zip)
      local tmp
      tmp="$(mktemp -d)"
      unzip -qo "$archive" -d "$tmp"
      # Move single top-level directory contents into dest
      local top
      top="$(find "$tmp" -mindepth 1 -maxdepth 1 -type d | head -1)"
      if [ -n "$top" ] && [ "$(find "$tmp" -mindepth 1 -maxdepth 1 | wc -l)" -eq 1 ]; then
        shopt -s dotglob
        mv "$top"/* "$dest"/
        shopt -u dotglob
      else
        shopt -s dotglob
        mv "$tmp"/* "$dest"/
        shopt -u dotglob
      fi
      rm -rf "$tmp"
      ;;
    *)
      echo "error: unsupported archive: $archive" >&2
      exit 1
      ;;
  esac
}

echo "==> wxWidgets 2.9.1"
WX_ARC="$CACHE/wxWidgets-2.9.1.tar.gz"
download "https://github.com/wxWidgets/wxWidgets/archive/refs/tags/v2.9.1.tar.gz" "$WX_ARC"
extract_archive "$WX_ARC" "wxWidgets-2.9.1"

echo "==> protobuf 2.4.1"
PB_ARC="$CACHE/protobuf-2.4.1.tar.gz"
download "https://github.com/protocolbuffers/protobuf/releases/download/v2.4.1/protobuf-2.4.1.tar.gz" "$PB_ARC"
extract_archive "$PB_ARC" "protobuf-2.4.1"
# protobuf 2.4 + modern MSVC: message.cc needs complete iostream types (not iosfwd).
if [ -f protobuf-2.4.1/src/google/protobuf/message.cc ] && \
   ! grep -q '#include <iostream>' protobuf-2.4.1/src/google/protobuf/message.cc; then
  python3 - <<'PY'
from pathlib import Path
p = Path("protobuf-2.4.1/src/google/protobuf/message.cc")
t = p.read_text()
needle = "#include <google/protobuf/message.h>\n"
if needle not in t:
    raise SystemExit("error: message.h include not found for iostream patch")
p.write_text(t.replace(needle, "#include <iostream>\n" + needle, 1))
print("    Patched protobuf message.cc: #include <iostream> (MSVC)")
PY
fi

echo "==> Boost 1.46.1"
BOOST_ARC="$CACHE/boost_1_46_1.tar.gz"
download "https://archives.boost.io/release/1.46.1/source/boost_1_46_1.tar.gz" "$BOOST_ARC"
BOOST_TMP="$(mktemp -d)"
extract_tar "$BOOST_ARC" "$BOOST_TMP" 0 gz
rm -rf boost
mkdir -p boost
cp -a "$BOOST_TMP/boost_1_46_1/boost" boost/
cp -a "$BOOST_TMP/boost_1_46_1/libs" boost/
rm -rf "$BOOST_TMP"
# Boost 1.46 enum TIME_UTC conflicts with the C11 macro on modern MSVC/UCRT.
if [ -f boost/boost/thread/xtime.hpp ]; then
  python3 - <<'PY'
from pathlib import Path
p = Path("boost/boost/thread/xtime.hpp")
t = p.read_text()
if "TIME_UTC_=1" in t:
    print("    xtime.hpp already uses TIME_UTC_")
else:
    t2 = t.replace("TIME_UTC=1", "TIME_UTC_=1").replace(
        "clock_type == TIME_UTC", "clock_type == TIME_UTC_"
    )
    if t2 == t:
        raise SystemExit("error: failed to patch TIME_UTC in xtime.hpp")
    p.write_text(t2)
    print("    Patched boost/thread/xtime.hpp: TIME_UTC -> TIME_UTC_ (MSVC C11)")
PY
fi

echo "==> ZeroMQ 2.1.7"
ZMQ_ARC="$CACHE/zeromq-2.1.7-release.tar.gz"
download "https://download.zeromq.org/zeromq-2.1.7.tar.gz" "$ZMQ_ARC"
extract_archive "$ZMQ_ARC" "zeromq-2.1.7"

echo "==> CEGUI 0.7.5"
CEGUI_ARC="$CACHE/CEGUI-0.7.5.tar.gz"
download "https://sourceforge.net/projects/crayzedsgui/files/CEGUI%20Mk-2/0.7.5/CEGUI-0.7.5.tar.gz/download" "$CEGUI_ARC"
extract_archive "$CEGUI_ARC" "CEGUI-0.7.5"

echo "==> freeglut 3.0.0 -> opengl/ (headers; place Windows libs manually if needed)"
FG_ARC="$CACHE/freeglut-3.0.0.tar.gz"
download "https://github.com/freeglut/freeglut/releases/download/v3.0.0/freeglut-3.0.0.tar.gz" "$FG_ARC"
FG_TMP="$(mktemp -d)"
extract_tar "$FG_ARC" "$FG_TMP" 0 gz
rm -rf opengl
mkdir -p opengl/include opengl/lib
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

- Set `FMOD_SDK` to an unpacked SDK path and re-run `scripts/fetch-deps.sh`, or
- Copy headers into `fmod/inc/`, the import lib into `fmod/lib/`, and `fmodex.dll` next to the runtime.
FMOD
  echo "    Skipped: set FMOD_SDK to an unpacked FMOD Ex SDK directory to install into fmod/"
fi

echo "==> CELayoutEditor 0.7.1 (optional tool, not required by FuckMPlayer.sln)"
echo "    Skipped by default. Install separately if you need the CEGUI layout editor."

echo "==> Apply MSVC project overlays from msvc-overlays/"
if [ -d msvc-overlays ]; then
  # Copy overlay files into fetched trees (vcxproj + config headers the sln expects)
  (cd msvc-overlays && find . -type f | while read -r f; do
    f="${f#./}"
    mkdir -p "$(dirname "$ROOT/$f")"
    cp -a "$ROOT/msvc-overlays/$f" "$ROOT/$f"
  done)
  echo "    Overlays applied."
else
  echo "    warning: msvc-overlays/ missing; FuckMPlayer.sln may not load vendor .vcxproj paths" >&2
fi

# ZeroMQ Windows build expects builds/msvc/platform.hpp (shipped in upstream tarball)
if [ ! -f zeromq-2.1.7/builds/msvc/platform.hpp ] && [ -f zeromq-2.1.7/builds/msvc/platform.hpp.in ]; then
  cp -a zeromq-2.1.7/builds/msvc/platform.hpp.in zeromq-2.1.7/builds/msvc/platform.hpp
fi

echo ""
echo "Done. Dependency trees are in place for a local Visual Studio Win32 build."
echo "Cache: $CACHE"
