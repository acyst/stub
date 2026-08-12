#!/bin/sh
#
# Build (if needed) and load stubfs.kext.
# Usage: ./scripts/load.sh
#

set -e

PROJECT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
BUNDLE="${PROJECT_DIR}/stubfs.kext"
BUNDLE_ID="com.example.stubfs"

cd "${PROJECT_DIR}"

if [ ! -d "${BUNDLE}" ]; then
	echo ">> Building ${BUNDLE} ..."
	make
fi

echo ">> Fixing ownership/permissions ..."
sudo chown -R root:wheel "${BUNDLE}"
sudo chmod -R 755 "${BUNDLE}"

echo ">> Loading ${BUNDLE_ID} ..."
sudo kextutil -v "${BUNDLE}"

echo ">> Registered filesystem types:"
kextstat | grep -i stubfs || true
sysctl vfs.generic.conf 2>/dev/null | grep -i stubfs || true
