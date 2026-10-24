#!/usr/bin/env bash
#
# build-all.sh <output-dir>
#
# Reads doc/src/site/versions.txt and builds every listed documentation version
# into <output-dir>/<label>/ by delegating to build-version.sh.
#
# The first (default) version is built with the site root as its canonical base
# URL, because assemble-site.sh also publishes it there.
#
# Every listed version is built on every run, because publishing replaces the
# whole site: a version that is not rebuilt here disappears from it.
#
set -euo pipefail

SITE_URL="https://docs.omnetpp.org/"

OUT="${1:?usage: build-all.sh <output-dir>}"
SITE_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"   # doc/src/site
VERSIONS_FILE="$SITE_DIR/versions.txt"

mkdir -p "$OUT"

count=0

# Read "<label> <ref>" lines, ignoring blanks and # comments.
while read -r label ref _rest; do
    [ -z "${label:-}" ] && continue
    case "$label" in \#*) continue ;; esac
    if [ -z "${ref:-}" ]; then
        echo "build-all: malformed line (missing ref): '$label'" >&2
        exit 2
    fi

    echo "==> building version '$label' from ref '$ref'"
    baseurl=""
    [ "$count" -eq 0 ] && baseurl="$SITE_URL"
    BASEURL="$baseurl" "$SITE_DIR/deploy/build-version.sh" "$label" "$ref" "$OUT"
    count=$((count + 1))
done < "$VERSIONS_FILE"

if [ "$count" -eq 0 ]; then
    echo "build-all: no versions found in $VERSIONS_FILE" >&2
    exit 2
fi
echo "build-all: built $count version(s)"
