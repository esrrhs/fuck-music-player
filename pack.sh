#!/usr/bin/env bash
# Package the Windows runtime snapshot under fuck/ plus project docs.
set -euo pipefail

ROOT="$(cd "$(dirname "$0")" && pwd)"
cd "$ROOT"

VERSION="$(tr -d '[:space:]' < VERSION)"
if [ -z "${VERSION}" ]; then
  echo "VERSION is empty" >&2
  exit 1
fi

NAME="fuck-music-player-${VERSION}-windows-x86"
OUT_DIR="pack/${NAME}"

rm -rf pack
mkdir -p "${OUT_DIR}"

cp -a fuck/. "${OUT_DIR}/"
cp -a README.md README_CN.md LICENSE VERSION "${OUT_DIR}/"

(
  cd pack
  rm -f "${NAME}.zip"
  zip -r -q "${NAME}.zip" "${NAME}"
)

rm -f pack.zip
(
  cd pack
  zip -q ../pack.zip "${NAME}.zip"
)

echo "Created pack/${NAME}.zip and pack.zip (version ${VERSION})"
ls -lh "pack/${NAME}.zip" pack.zip
