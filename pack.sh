#!/usr/bin/env bash
# Package the Windows runtime snapshot under fuck/ plus project docs.
# In CI (or when FETCH_DEPS=1), fetch third-party trees first.
set -euo pipefail

ROOT="$(cd "$(dirname "$0")" && pwd)"
cd "$ROOT"

if [ "${CI:-}" = "true" ] || [ "${FETCH_DEPS:-}" = "1" ]; then
  echo "Fetching third-party dependencies..."
  chmod +x ./scripts/fetch-deps.sh
  ./scripts/fetch-deps.sh
  test -d wxWidgets-2.9.1/include
  test -d protobuf-2.4.1/src
  test -d boost/boost
  test -d zeromq-2.1.7/include
  test -d CEGUI-0.7.5/cegui
  echo "Dependency trees OK"
fi

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
