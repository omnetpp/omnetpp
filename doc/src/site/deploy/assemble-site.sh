#!/usr/bin/env bash
#
# assemble-site.sh <output-dir>
#
# Adds the site root to a staged docs tree that already contains one or more
# <label>/ version directories (produced by build-all.sh): an index.html
# redirecting to the default version, rendered from templates/index-redirect.html.
#
# The default version is the first non-comment line of versions.txt, read here
# directly so this script can be run on its own.
#
# Nothing else belongs in the root. GitHub Pages is fed straight from the deploy
# workflow ("Source: GitHub Actions"), so the tree is served verbatim: the custom
# domain comes from the repository's Pages settings rather than a CNAME file, and
# no Jekyll step runs that a .nojekyll would have to suppress.
#
set -euo pipefail

OUT="${1:?usage: assemble-site.sh <output-dir>}"
SITE_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"   # doc/src/site

DEFAULT_VERSION="$(awk '!/^[[:space:]]*(#|$)/ { print $1; exit }' "$SITE_DIR/versions.txt")"
if [ -z "$DEFAULT_VERSION" ]; then
    echo "assemble-site: no versions listed in $SITE_DIR/versions.txt" >&2
    exit 2
fi
if [ ! -d "$OUT/$DEFAULT_VERSION" ]; then
    echo "assemble-site: $OUT/$DEFAULT_VERSION not found (run build-all.sh first)" >&2
    exit 2
fi

echo "--> root redirect -> /$DEFAULT_VERSION/"
sed "s/@DEFAULT_VERSION@/$DEFAULT_VERSION/g" \
    "$SITE_DIR/templates/index-redirect.html" > "$OUT/index.html"

echo "assemble-site: root assembled (default='$DEFAULT_VERSION')"
