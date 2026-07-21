#!/usr/bin/env bash
#
# Build the OMNeT++ HTML documentation for the current checkout inside the
# doc-builder container. Produces doc/src/out.
#
# Usage:  site/doc-builder/run.sh [make-target]   (default target: ci-html)
# Env:    IMAGE    image reference (default: ghcr.io/omnetpp/doc-builder:latest)
#         ENGINE   podman (default) or docker
#         VERSION  documentation version label (default: 6.x)
#         WHAT     omnetpp (default) or omnest
#
set -euo pipefail

IMAGE="${IMAGE:-ghcr.io/omnetpp/doc-builder:latest}"
ENGINE="${ENGINE:-podman}"
TARGET="${1:-ci-html}"
HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO="$(git -C "$HERE" rev-parse --show-toplevel)"
VERSION="${VERSION:-6.x}"
WHAT="${WHAT:-omnetpp}"
BUILDID="$(git -C "$REPO" rev-parse --short HEAD)"
# On SELinux-enforcing hosts set MOUNTOPT=:z (shared) or :Z (private) to relabel.
MOUNTOPT="${MOUNTOPT:-}"

echo "Running '$TARGET' in $IMAGE against $REPO (VERSION=$VERSION)"
exec "$ENGINE" run --rm \
    -v "$REPO":/work${MOUNTOPT} \
    -w /work/doc/src \
    "$IMAGE" \
    make "$TARGET" PIPRUN= VENVDEP= VERSION="$VERSION" BUILDID="$BUILDID" what="$WHAT"
