#!/usr/bin/env bash
#
# assemble-site.sh <output-dir>
#
# Turns a staged docs tree that already contains one <label>/ directory per
# version (produced by build-all.sh) into the published site:
#
#   - the default version (the first non-comment line of versions.txt) is copied
#     to the site root as well. The root URLs are its canonical ones (build-all.sh
#     builds it with that base URL), and they are the URLs of the documentation
#     before it was versioned, so old links keep working and always show the
#     newest version. Its <label>/ copy keeps links to that version working once
#     it is no longer the default.
#   - 404.html, rendered from templates/404.html, which GitHub Pages serves for
#     any missing path.
#
# versions.txt is read here directly so this script can be run on its own.
#
# GitHub Pages is fed straight from the deploy workflow ("Source: GitHub
# Actions"), so the tree is served verbatim: the custom domain comes from the
# repository's Pages settings rather than a CNAME file, and no Jekyll step runs
# that a .nojekyll would have to suppress.
#
set -euo pipefail

OUT="${1:?usage: assemble-site.sh <output-dir>}"
SITE_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"   # doc/src/site

LABELS=()
while read -r label _rest; do
    LABELS+=("$label")
done < <(awk '!/^[[:space:]]*(#|$)/ { print $1 }' "$SITE_DIR/versions.txt")
if [ "${#LABELS[@]}" -eq 0 ]; then
    echo "assemble-site: no versions listed in $SITE_DIR/versions.txt" >&2
    exit 2
fi
DEFAULT_VERSION="${LABELS[0]}"

for label in "${LABELS[@]}"; do
    if [ ! -d "$OUT/$label" ]; then
        echo "assemble-site: $OUT/$label not found (run build-all.sh first)" >&2
        exit 2
    fi
    # The version directories and the default version's content share the root.
    if [ -e "$OUT/$DEFAULT_VERSION/$label" ]; then
        echo "assemble-site: version label '$label' collides with '$label' in the documentation root" >&2
        exit 2
    fi
done

echo "--> default version '$DEFAULT_VERSION' -> site root"
cp -a "$OUT/$DEFAULT_VERSION/." "$OUT/"

echo "--> 404.html"
links=""
json=""
for label in "${LABELS[@]}"; do
    if [ "$label" = "$DEFAULT_VERSION" ]; then
        links+="    <li><a href=\"/\">$label</a> (current)</li>"$'\n'
    else
        links+="    <li><a href=\"/$label/\">$label</a></li>"$'\n'
    fi
    json+="${json:+, }\"$label\""
done
LINKS="${links%$'\n'}" LABELS_JSON="[$json]" DEFAULT_VERSION="$DEFAULT_VERSION" python3 -c '
import os, sys
page = sys.stdin.read()
for token, env in (("@VERSION_LINKS@", "LINKS"), ("@VERSION_LABELS@", "LABELS_JSON"), ("@DEFAULT_VERSION@", "DEFAULT_VERSION")):
    page = page.replace(token, os.environ[env])
sys.stdout.write(page)
' < "$SITE_DIR/templates/404.html" > "$OUT/404.html"

echo "assemble-site: site assembled (default='$DEFAULT_VERSION', versions: ${LABELS[*]})"
