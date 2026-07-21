#!/usr/bin/env bash
#
# build-version.sh <label> <ref> <output-dir>
#
# Builds the OMNeT++ documentation for a single version and stages the resulting
# HTML under <output-dir>/<label>/.
#
# The <ref> is checked out into a throwaway git worktree so the build always uses
# that version's own doc sources and build system (doc/src, site/, tex2rst, ...),
# independent of the branch that drives the pipeline. The worktree is removed
# afterwards.
#
# Environment:
#   WHAT   'omnetpp' (default) or 'omnest'
#
set -euo pipefail

LABEL="${1:?usage: build-version.sh <label> <ref> <output-dir>}"
REF="${2:?missing <ref>}"
OUT="${3:?missing <output-dir>}"
WHAT="${WHAT:-omnetpp}"

REPO="$(git rev-parse --show-toplevel)"
OUT_ABS="$(cd "$OUT" && pwd)"
WORKTREE="$(mktemp -d "${TMPDIR:-/tmp}/oppdoc-${LABEL//\//_}.XXXXXX")"

cleanup() {
    git -C "$REPO" worktree remove --force "$WORKTREE" 2>/dev/null || true
    rm -rf "$WORKTREE" 2>/dev/null || true
}
trap cleanup EXIT

# Resolve the ref to a commit, fetching it from origin when the local clone does
# not have it (CI checkouts typically contain only the triggering branch).
if git -C "$REPO" rev-parse --verify --quiet "${REF}^{commit}" >/dev/null; then
    COMMIT="$REF"
else
    echo "--> ref '$REF' not present locally; fetching from origin"
    git -C "$REPO" fetch --no-tags --depth=1 origin "$REF"
    COMMIT=FETCH_HEAD
fi

echo "--> checking out '$REF' into worktree $WORKTREE"
git -C "$REPO" worktree add --detach "$WORKTREE" "$COMMIT"

BUILDID="$(git -C "$WORKTREE" rev-parse --short HEAD)"
DOCSRC="$WORKTREE/doc/src"

if [ ! -f "$DOCSRC/site/check-conversion.py" ]; then
    echo "build-version: ref '$REF' does not contain the Sphinx site build system" >&2
    echo "               (doc/src/site is missing) - only future versions are supported." >&2
    exit 3
fi

echo "--> building HTML (VERSION=$LABEL BUILDID=$BUILDID what=$WHAT)"
# If the Sphinx toolchain is available globally (e.g. inside the doc-builder
# container), bypass uv; otherwise fall back to the uv-managed virtualenv.
make_args=(VERSION="$LABEL" BUILDID="$BUILDID" what="$WHAT")
if command -v sphinx-build >/dev/null 2>&1 && command -v doxysphinx >/dev/null 2>&1; then
    make_args+=(PIPRUN= VENVDEP=)
fi
make -C "$DOCSRC" ci-html "${make_args[@]}"

# The unified Sphinx `site` target builds with `sphinx-build -b html . out`, so the
# HTML root is doc/src/out itself: manual, guides, tutorials, articles, the doxygen
# APIs (api/, nedxml-api/, parsim-api/) and the omnetpp.scave Python API.
HTML="$DOCSRC/out"
if [ ! -f "$HTML/index.html" ]; then
    echo "build-version: expected build output not found at $HTML/index.html" >&2
    exit 4
fi

DEST="$OUT_ABS/$LABEL"
echo "--> staging into $DEST"
rm -rf "$DEST"
mkdir -p "$DEST"
cp -a "$HTML/." "$DEST/"
# The conversion audit report is a build artifact, not part of the published site.
rm -f "$DEST/conversion-report.md"

echo "build-version: version '$LABEL' staged ($(du -sh "$DEST" | cut -f1))"
