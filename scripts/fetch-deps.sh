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
# GitHub source tags ship setup0.h only; MSVC expects include/wx/msw/setup.h.
if [ ! -f wxWidgets-2.9.1/include/wx/msw/setup.h ] && [ -f wxWidgets-2.9.1/include/wx/msw/setup0.h ]; then
  cp -a wxWidgets-2.9.1/include/wx/msw/setup0.h wxWidgets-2.9.1/include/wx/msw/setup.h
  echo "    Copied setup0.h -> include/wx/msw/setup.h"
fi
# CustomBuild copies setup.h into lib/vc_{lib,dll}/mswu*/wx/ but does not mkdir;
# solution maps Release -> DLL Release, so seed vc_dll/mswu (and common variants).
if [ -f wxWidgets-2.9.1/include/wx/msw/setup.h ]; then
  for cfg in vc_dll/mswu vc_dll/mswud vc_lib/mswu vc_lib/mswud; do
    mkdir -p "wxWidgets-2.9.1/lib/$cfg/wx/msw"
    cp -a wxWidgets-2.9.1/include/wx/msw/setup.h "wxWidgets-2.9.1/lib/$cfg/wx/setup.h"
    if [ -f wxWidgets-2.9.1/include/wx/msw/rcdefs.h ]; then
      cp -a wxWidgets-2.9.1/include/wx/msw/rcdefs.h "wxWidgets-2.9.1/lib/$cfg/wx/msw/rcdefs.h"
    elif [ -f wxWidgets-2.9.1/include/wx/msw/genrcdefs.h ]; then
      # Minimal stub; genrcdefs is normally preprocessed by cl.exe during CustomBuild.
      printf '%s\n' '/* generated stub for CI */' > "wxWidgets-2.9.1/lib/$cfg/wx/msw/rcdefs.h"
    fi
  done
  echo "    Seeded wx/setup.h under lib/vc_{lib,dll}/mswu{,d}/ for MSVC"
fi

# wx 2.9.1 datetime.cpp falls through to POSIX `timezone` on desktop MSVC;
# UCRT no longer provides that name. Use _get_timezone for all __VISUALC__.
if [ -f wxWidgets-2.9.1/src/common/datetime.cpp ]; then
  python3 - <<'PY'
from pathlib import Path
p = Path("wxWidgets-2.9.1/src/common/datetime.cpp")
t = p.read_text()
old = """    #elif defined(__WXWINCE__) && defined(__VISUALC8__)
        // _timezone is not present in dynamic run-time library
        #if 0
        // Solution (1): use the function equivalent of _timezone
        static long wxGetTimeZone()
        {
            long t;
            _get_timezone(& t);
            return t;
        }
        #define WX_TIMEZONE wxGetTimeZone()
        #elif 1
        // Solution (2): using GetTimeZoneInformation
        static long wxGetTimeZone()
        {
            TIME_ZONE_INFORMATION tzi;
            ::GetTimeZoneInformation(&tzi);
            return tzi.Bias; // x 60
        }
        #define WX_TIMEZONE wxGetTimeZone()
        #else
        // Old method using _timezone: this symbol doesn't exist in the dynamic run-time library (i.e. using /MD)
        #define WX_TIMEZONE _timezone
        #endif
    #else // unknown platform - try timezone
        #define WX_TIMEZONE timezone
    #endif"""
new = """    #elif defined(__VISUALC__)
        // Prefer CRT _get_timezone (seconds). POSIX `timezone` is gone from UCRT;
        // _timezone is awkward with /MD. Works for WinCE and desktop MSVC.
        static long wxGetTimeZone()
        {
            long t = 0;
            _get_timezone(&t);
            return t;
        }
        #define WX_TIMEZONE wxGetTimeZone()
    #else // unknown platform - try timezone
        #define WX_TIMEZONE timezone
    #endif"""
if "elif defined(__VISUALC__)" in t and "_get_timezone(&t)" in t:
    print("    datetime.cpp already patched for MSVC timezone")
elif old not in t:
    raise SystemExit("error: unexpected wxWidgets datetime.cpp timezone block")
else:
    p.write_text(t.replace(old, new, 1))
    print("    Patched wxWidgets datetime.cpp: MSVC uses _get_timezone")
PY
fi

# Silence "Please update wx/platform.h" for VS2012+; map to a late-known token.
if [ -f wxWidgets-2.9.1/include/wx/platform.h ]; then
  python3 - <<'PY'
from pathlib import Path
p = Path("wxWidgets-2.9.1/include/wx/platform.h")
t = p.read_text()
old = """#   elif __VISUALC__ < 1700
#       define __VISUALC10__
#   else
#       pragma message("Please update wx/platform.h to recognize this VC++ version")
#   endif"""
new = """#   elif __VISUALC__ < 1700
#       define __VISUALC10__
#   elif __VISUALC__ < 1800
#       define __VISUALC11__
#   elif __VISUALC__ < 1900
#       define __VISUALC12__
#   else
        /* VS2015+ (1900+) and later; enough for wx 2.9.1 feature tests */
#       define __VISUALC14__
#   endif"""
if "__VISUALC14__" in t:
    print("    platform.h already recognizes VS2015+")
elif old not in t:
    raise SystemExit("error: unexpected wx/platform.h VC version block")
else:
    p.write_text(t.replace(old, new, 1))
    print("    Patched wx/platform.h: recognize VS2012+ without pragma noise")
PY
fi

# pbt.h was dropped from Windows SDK 8+; PBT_* macros live in winuser.h.
# Upstream removed this include after 2.9.1 (r69863).
if [ -f wxWidgets-2.9.1/src/msw/window.cpp ]; then
  python3 - <<'PY'
from pathlib import Path
p = Path("wxWidgets-2.9.1/src/msw/window.cpp")
t = p.read_text()
old = """#if !defined __WXWINCE__ && !defined NEED_PBT_H
    #include <pbt.h>
#endif"""
new = """/* pbt.h removed from modern Windows SDKs; PBT_* are in winuser.h (r69863). */
#if 0 && !defined __WXWINCE__ && !defined NEED_PBT_H
    #include <pbt.h>
#endif"""
if "pbt.h removed from modern Windows SDKs" in t:
    print("    window.cpp already skips pbt.h")
elif old not in t:
    raise SystemExit("error: unexpected window.cpp pbt.h include block")
else:
    p.write_text(t.replace(old, new, 1))
    print("    Patched wxWidgets window.cpp: skip missing pbt.h")
PY
fi



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
download "https://github.com/zeromq/zeromq2-x/releases/download/v2.1.7/zeromq-2.1.7.tar.gz" "$ZMQ_ARC"
extract_archive "$ZMQ_ARC" "zeromq-2.1.7"
# ZeroMQ 2.1.7 stdint.hpp typedefs clash with MSVC UCRT <stdint.h> (int8_t = __int8 vs signed char).
if [ -f zeromq-2.1.7/src/stdint.hpp ]; then
  python3 - <<'PY'
from pathlib import Path
p = Path("zeromq-2.1.7/src/stdint.hpp")
t = p.read_text()
old = """#elif defined _MSC_VER

#ifndef int8_t
typedef __int8 int8_t;
#endif
#ifndef int16_t
typedef __int16 int16_t;
#endif
#ifndef int32_t
typedef __int32 int32_t;
#endif
#ifndef int64_t
typedef __int64 int64_t;
#endif
#ifndef uint8_t
typedef unsigned __int8 uint8_t;
#endif
#ifndef uint16_t
typedef unsigned __int16 uint16_t;
#endif
#ifndef uint32_t
typedef unsigned __int32 uint32_t;
#endif
#ifndef uint64_t
typedef unsigned __int64 uint64_t;
#endif

#else"""
new = """#elif defined _MSC_VER && _MSC_VER >= 1600

#include <stdint.h>

#elif defined _MSC_VER

#ifndef int8_t
typedef __int8 int8_t;
#endif
#ifndef int16_t
typedef __int16 int16_t;
#endif
#ifndef int32_t
typedef __int32 int32_t;
#endif
#ifndef int64_t
typedef __int64 int64_t;
#endif
#ifndef uint8_t
typedef unsigned __int8 uint8_t;
#endif
#ifndef uint16_t
typedef unsigned __int16 uint16_t;
#endif
#ifndef uint32_t
typedef unsigned __int32 uint32_t;
#endif
#ifndef uint64_t
typedef unsigned __int64 uint64_t;
#endif

#else"""
if "MSC_VER >= 1600" in t:
    print("    stdint.hpp already patched for modern MSVC")
elif old not in t:
    raise SystemExit("error: unexpected zeromq stdint.hpp layout")
else:
    p.write_text(t.replace(old, new, 1))
    print("    Patched zeromq stdint.hpp: use <stdint.h> on MSVC >= 1600")
PY
fi

echo "==> CEGUI 0.7.5"
CEGUI_ARC="$CACHE/CEGUI-0.7.5.tar.gz"
download "https://sourceforge.net/projects/crayzedsgui/files/CEGUI%20Mk-2/0.7.5/CEGUI-0.7.5.tar.gz/download" "$CEGUI_ARC"
extract_archive "$CEGUI_ARC" "CEGUI-0.7.5"

echo "==> freeglut 3.0.0 -> opengl/ (headers; Win32 libs from CEGUI-DEPS + msvc-overlays)"
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
echo "    Headers installed. Import libs: freeglut from CEGUI-DEPS (below);"
echo "    historical glaux/glut32 for logo.dll come from msvc-overlays/opengl/."

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

echo "==> CEGUI MSVC dependencies (lua, freeglut, codecs, …)"
CEGUI_DEPS_ARC="$CACHE/CEGUI-DEPS-0.7.x-r3-vc10.zip"
download "https://downloads.sourceforge.net/project/crayzedsgui/CEGUI%20Mk-2%20MSVC%2B%2B%20Dependencies/0.7.x/CEGUI-DEPS-0.7.x-r3-vc10.zip" "$CEGUI_DEPS_ARC"
# Archive root is dependencies/; merge into CEGUI-0.7.5/dependencies/
DEPS_TMP="$(mktemp -d)"
unzip -qo "$CEGUI_DEPS_ARC" -d "$DEPS_TMP"
if [ -d "$DEPS_TMP/dependencies" ]; then
  mkdir -p CEGUI-0.7.5/dependencies
  cp -a "$DEPS_TMP/dependencies"/. CEGUI-0.7.5/dependencies/
  echo "    Merged CEGUI-DEPS into CEGUI-0.7.5/dependencies/"
else
  echo "error: CEGUI-DEPS archive missing dependencies/ root" >&2
  rm -rf "$DEPS_TMP"
  exit 1
fi
rm -rf "$DEPS_TMP"
test -f CEGUI-0.7.5/dependencies/include/lua.h
test -f CEGUI-0.7.5/dependencies/lib/dynamic/lua.lib
test -f CEGUI-0.7.5/dependencies/lib/dynamic/freeglut.lib
# Stage freeglut import lib + runtime DLL where logo/main expect OpenGL deps
mkdir -p opengl/lib
cp -a CEGUI-0.7.5/dependencies/lib/dynamic/freeglut.lib opengl/lib/freeglut.lib
if [ -f CEGUI-0.7.5/dependencies/bin/freeglut.dll ]; then
  cp -a CEGUI-0.7.5/dependencies/bin/freeglut.dll opengl/lib/freeglut.dll
fi
echo "    Copied freeglut.lib (+ dll if present) into opengl/lib/"

echo "==> Legacy DirectX D3DX9 + DxErr (headers/libs for CEGUI Direct3D9)"
D3DX_NUPKG="$CACHE/microsoft.dxsdk.d3dx.9.29.952.8.nupkg"
download "https://api.nuget.org/v3-flatcontainer/microsoft.dxsdk.d3dx/9.29.952.8/microsoft.dxsdk.d3dx.9.29.952.8.nupkg" "$D3DX_NUPKG"
DXERR_NUPKG="$CACHE/dxsdk_missing.1.1.2.nupkg"
download "https://api.nuget.org/v3-flatcontainer/dxsdk_missing/1.1.2/dxsdk_missing.1.1.2.nupkg" "$DXERR_NUPKG"
DX_TMP="$(mktemp -d)"
unzip -qo "$D3DX_NUPKG" -d "$DX_TMP/d3dx"
unzip -qo "$DXERR_NUPKG" -d "$DX_TMP/dxerr"
mkdir -p CEGUI-0.7.5/dependencies/include CEGUI-0.7.5/dependencies/lib/dynamic
# Headers (d3dx9* from official Microsoft package; DxErr.h from DXSDK_Missing)
cp -a "$DX_TMP/d3dx/build/native/include/." CEGUI-0.7.5/dependencies/include/
cp -a "$DX_TMP/dxerr/Include/DxErr.h" CEGUI-0.7.5/dependencies/include/
# Win32 import libs into the path CEGUI vcxproj already searches
cp -a "$DX_TMP/d3dx/build/native/release/lib/x86/d3dx9.lib" CEGUI-0.7.5/dependencies/lib/dynamic/d3dx9.lib
cp -a "$DX_TMP/dxerr/Lib/native/x86/DxErr.lib" CEGUI-0.7.5/dependencies/lib/dynamic/dxerr.lib
rm -rf "$DX_TMP"
test -f CEGUI-0.7.5/dependencies/include/d3dx9.h
test -f CEGUI-0.7.5/dependencies/lib/dynamic/d3dx9.lib
test -f CEGUI-0.7.5/dependencies/lib/dynamic/dxerr.lib
echo "    Installed d3dx9.h / DxErr.h / d3dx9.lib / dxerr.lib into CEGUI dependencies"

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
# Sources #include "platform.hpp"; ensure it exists under src/ (PreBuild also copies it).
if [ -f zeromq-2.1.7/builds/msvc/platform.hpp ]; then
  cp -a zeromq-2.1.7/builds/msvc/platform.hpp zeromq-2.1.7/src/platform.hpp
fi

# logo.dll links glaux.lib (BMP via auxDIBImageLoad); overlays should have placed it.
if [ -f opengl/lib/glaux.lib ] && [ -f opengl/include/glaux.h ]; then
  echo "    opengl glaux.h + glaux.lib present (logo plugin)"
else
  echo "warning: opengl/lib/glaux.lib or include/glaux.h missing; plugin\\logo will not link" >&2
fi

echo ""
echo "Done. Dependency trees are in place for a local Visual Studio Win32 build."
echo "Cache: $CACHE"
