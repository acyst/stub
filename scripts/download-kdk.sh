#!/bin/bash
#
# download-kdk.sh - Automatically download and install an Apple Kernel
# Development Kit (KDK) from the community mirror.
#
# Apple requires a developer login to download KDKs, which makes them
# impossible to fetch in CI.  This script instead resolves the requested KDK
# from the dortania/KdkSupportPkg mirror (a GitHub-hosted archive) and
# installs it under /Library/Developer/KDKs, where the Makefile auto-detects
# it.
#
# Usage:
#   ./scripts/download-kdk.sh [VERSION]     # default: 26.5.2
#
# Environment:
#   KDK_CACHE_DIR   Directory to cache the .dmg (default: ~/.cache/kdk)
#

set -euo pipefail

VERSION="${1:-26.5.2}"
MANIFEST_URL="https://raw.githubusercontent.com/dortania/KdkSupportPkg/gh-pages/manifest.json"
CACHE_DIR="${KDK_CACHE_DIR:-$HOME/.cache/kdk}"
KDK_ROOT="/Library/Developer/KDKs"

if command -v python3 >/dev/null 2>&1; then
    :
else
    echo "ERROR: python3 is required" >&2
    exit 1
fi

echo ">> Resolving KDK ${VERSION} from manifest ..."

read -r url build sha256 <<< "$(curl -fsSL "${MANIFEST_URL}" | python3 -c '
import json, sys
version = sys.argv[1]
for k in json.load(sys.stdin):
    if k["version"] == version:
        print(k["url"], k["build"], k["sha256sum"])
        break
else:
    sys.exit("KDK {} not found in manifest".format(version))
' "${VERSION}")"

if [ -z "${url}" ] || [ -z "${build}" ]; then
    echo "ERROR: KDK ${VERSION} not found in mirror manifest" >&2
    exit 1
fi

echo ">> KDK ${VERSION} -> build ${build}"

# Short-circuit if already installed.
if ls -d "${KDK_ROOT}"/KDK_*_"${build}".kdk >/dev/null 2>&1; then
    echo ">> Already installed: $(ls -d "${KDK_ROOT}"/KDK_*_"${build}".kdk)"
    exit 0
fi

mkdir -p "${CACHE_DIR}"
dmg="${CACHE_DIR}/$(basename "${url}")"

if [ ! -f "${dmg}" ]; then
    echo ">> Downloading ${url} ..."
    curl -fL --retry 3 --retry-delay 5 -o "${dmg}.part" "${url}"
    mv "${dmg}.part" "${dmg}"
fi

echo ">> Verifying sha256 ..."
actual="$(shasum -a 256 "${dmg}" | awk '{print $1}')"
if [ "${actual}" != "${sha256}" ]; then
    echo "ERROR: checksum mismatch (expected ${sha256}, got ${actual})" >&2
    rm -f "${dmg}"
    exit 1
fi

mountpoint="/tmp/kdk-${build}"
rm -rf "${mountpoint}"
mkdir -p "${mountpoint}"

echo ">> Mounting ${dmg} ..."
hdiutil attach -nobrowse -readonly -mountpoint "${mountpoint}" "${dmg}" >/dev/null

cleanup() {
    hdiutil detach "${mountpoint}" -quiet >/dev/null 2>&1 || true
}
trap cleanup EXIT

pkg="$(find "${mountpoint}" -maxdepth 1 -name '*.pkg' -print -quit)"
if [ -z "${pkg}" ]; then
    echo "ERROR: no .pkg found inside the DMG" >&2
    exit 1
fi

echo ">> Installing ${pkg} ..."
sudo installer -pkg "${pkg}" -target /

echo ">> Installed KDK ${VERSION} into ${KDK_ROOT}"
ls -d "${KDK_ROOT}"/KDK_*_"${build}".kdk
