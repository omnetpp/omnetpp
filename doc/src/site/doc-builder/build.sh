#!/usr/bin/env bash
#
# Build (and optionally publish) the OMNeT++ documentation builder image.
#
# Usage:  site/doc-builder/build.sh [--push]
# Env:    IMAGE   image reference (default: ghcr.io/omnetpp/doc-builder:latest,
#                 i.e. exactly what deploy-docs.yml pulls)
#         ENGINE  podman (default) or docker
#
# --push additionally tags the image with the current commit's short sha, so a
# deployment can be pinned to an exact toolchain instead of the moving :latest.
# Log in first:  echo "$GHCR_TOKEN" | podman login ghcr.io -u <user> --password-stdin
#
set -euo pipefail

IMAGE="${IMAGE:-ghcr.io/omnetpp/doc-builder:latest}"
ENGINE="${ENGINE:-podman}"
PUSH=0
[ "${1:-}" = "--push" ] && PUSH=1

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
CONTEXT="$(cd "$HERE/../.." && pwd)"   # doc/src (holds pyproject.toml / uv.lock)

# GHCR links the package to the repository named by this label, which is what
# lets a workflow's GITHUB_TOKEN pull the image. Derive it from origin so the
# link points at whatever repository actually publishes.
ORIGIN="$(git -C "$CONTEXT" remote get-url origin 2>/dev/null || true)"
case "$ORIGIN" in
    git@github.com:*)     SOURCE="https://github.com/${ORIGIN#git@github.com:}" ;;
    https://github.com/*) SOURCE="$ORIGIN" ;;
    *)                    SOURCE="" ;;
esac
SOURCE="${SOURCE%.git}"

label_args=()
[ -n "$SOURCE" ] && label_args=(--label "org.opencontainers.image.source=$SOURCE")

echo "Building $IMAGE with $ENGINE (context: $CONTEXT, source: ${SOURCE:-unset})"
"$ENGINE" build --platform linux/amd64 -t "$IMAGE" "${label_args[@]}" \
    -f "$HERE/Containerfile" "$CONTEXT"

if [ "$PUSH" = "1" ]; then
    SHA_TAG="${IMAGE%:*}:$(git -C "$CONTEXT" rev-parse --short HEAD)"
    echo "Tagging $SHA_TAG; pushing both references"
    "$ENGINE" tag "$IMAGE" "$SHA_TAG"
    "$ENGINE" push "$IMAGE"
    "$ENGINE" push "$SHA_TAG"
fi
