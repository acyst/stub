#!/bin/sh
#
# Unload stubfs.kext.
# Usage: ./scripts/unload.sh
#

set -e

BUNDLE_ID="com.example.stubfs"

if kextstat | grep -q "${BUNDLE_ID}"; then
	echo ">> Unloading ${BUNDLE_ID} ..."
	sudo kextunload -b "${BUNDLE_ID}"
else
	echo ">> ${BUNDLE_ID} is not loaded"
fi
