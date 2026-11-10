#!/usr/bin/env bash
#
# Runs a target of doc/src/Makefile in the doc-builder image, the only environment the documentation is
# built in. The image is the one pinned in doc-builder.image. The OMNeT++ tree that contains this
# directory is mounted into the container at the same path, so the outputs land in it as with a
# local make.
#
# Usage:  doc/src/build.sh [make-target] [make-arguments...]   (default target: ci-html)
#         e.g.  doc/src/build.sh all
#               doc/src/build.sh site-server          (then open http://localhost:8000)
# Env:    IMAGE    image reference (default: the one in doc-builder.image)
#         ENGINE   podman or docker (default: podman if installed, otherwise docker)
#         VERSION  documentation version label
#         WHAT     omnetpp or omnest (also taken from $what)
#         BUILDID  build identifier shown in the pages (default: the short hash of HEAD, in a git repo)
#         BASEURL  base URL of the pages, e.g. http://localhost:8000/ to serve doc/src/out there: the
#                  Manual (pdf) entry of the navigation leads to it (default: the live version's)
#         PORT     the port on localhost that site-server is published on (default: 8000)
# Unset identity variables are left to the defaults of the Makefile.
#
set -euo pipefail

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd -P)"
ROOT="$(cd "$HERE/../.." && pwd -P)"
TARGET="${1:-ci-html}"
[ $# -eq 0 ] || shift

IMAGE="${IMAGE:-$(grep -v '^[[:space:]]*\(#\|$\)' "$HERE/doc-builder.image" | head -n 1)}"
[ -n "$IMAGE" ] || { echo "build.sh: no image reference in $HERE/doc-builder.image" >&2; exit 1; }

if [ -z "${ENGINE:-}" ]; then
    if command -v podman >/dev/null; then ENGINE=podman; else ENGINE=docker; fi
fi

WHAT="${WHAT:-${what:-}}"
if [ -z "${BUILDID:-}" ] && git -C "$ROOT" rev-parse --git-dir >/dev/null 2>&1; then
    BUILDID="$(git -C "$ROOT" rev-parse --short HEAD)"
fi

make_args=()
[ -z "${VERSION:-}" ] || make_args+=(VERSION="$VERSION")
[ -z "$WHAT" ] || make_args+=(what="$WHAT")
[ -z "${BUILDID:-}" ] || make_args+=(BUILDID="$BUILDID")
[ -z "${BASEURL:-}" ] || make_args+=(BASEURL="$BASEURL")

run_opts=(--rm --init --platform linux/amd64 --security-opt label=disable)
# A rootless engine maps the container's root to the calling user; a rootful one has to be told the
# user, or the outputs would belong to root.
if ! "$ENGINE" info 2>/dev/null | grep -Eqi 'name=rootless|rootless: true'; then
    run_opts+=(--user "$(id -u):$(id -g)")
fi
[ ! -t 0 ] || run_opts+=(-it)
[ "$TARGET" != site-server ] || run_opts+=(-p "127.0.0.1:${PORT:-8000}:8000")

echo "build.sh: running 'make $TARGET' in $IMAGE with $ENGINE"
exec "$ENGINE" run "${run_opts[@]}" \
    -v "$ROOT:$ROOT" -w "$HERE" -e HOME=/tmp \
    "$IMAGE" \
    make "$TARGET" "${make_args[@]}" "$@"
