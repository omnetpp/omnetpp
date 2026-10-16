#!/usr/bin/env bash
#
# Build (and optionally publish) the OMNeT++ documentation builder images: the
# HTML toolchain as $IMAGE, and its PDF variant (+ TeX Live) as $IMAGE-pdf.
#
# Usage:  site/doc-builder/build.sh [--push]
# Env:    IMAGE          image reference (default: ghcr.io/omnetpp/doc-builder:latest,
#                        i.e. exactly what deploy-docs.yml pulls)
#         ENGINE         podman (default) or docker
#         OPP_TOOLS_TGZ  the @omnetpp/opp-tools npm tarball to bundle (default: the
#                        one in <repo>/vscode/build/, made by `bun run package:cli`)
#
# --push additionally tags the images with the current commit's short sha (:<sha>,
# :<sha>-pdf), so a deployment can be pinned to an exact toolchain instead of the
# moving :latest.
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

# opp-tools is not published to npm yet, so the image bundles a locally built
# tarball. It is staged into the build context under a fixed name for the
# Containerfile to bind-mount.
REPO="$(git -C "$CONTEXT" rev-parse --show-toplevel)"
VSCODE_REPO="$REPO/vscode"
OPP_TOOLS_COMMIT=""
if [ -z "${OPP_TOOLS_TGZ:-}" ]; then
    shopt -s nullglob
    candidates=("$VSCODE_REPO"/build/omnetpp-opp-tools-*.tgz)
    shopt -u nullglob
    if [ "${#candidates[@]}" -eq 0 ]; then
        echo "error: no opp-tools tarball in $VSCODE_REPO/build/" >&2
        echo "  build it with:  (cd $VSCODE_REPO && bun run package:cli)" >&2
        echo "  or point OPP_TOOLS_TGZ at one" >&2
        exit 1
    elif [ "${#candidates[@]}" -gt 1 ]; then
        echo "error: several opp-tools tarballs in $VSCODE_REPO/build/; set OPP_TOOLS_TGZ to one:" >&2
        printf '  %s\n' "${candidates[@]}" >&2
        exit 1
    fi
    OPP_TOOLS_TGZ="${candidates[0]}"
    OPP_TOOLS_COMMIT="$(git -C "$VSCODE_REPO" rev-parse --short HEAD 2>/dev/null || true)"
    head_time="$(git -C "$VSCODE_REPO" log -1 --format=%ct 2>/dev/null || echo 0)"
    if [ "$(stat -c %Y "$OPP_TOOLS_TGZ")" -lt "$head_time" ]; then
        echo "warning: $OPP_TOOLS_TGZ is older than vscode HEAD ($OPP_TOOLS_COMMIT);" \
             "the commit label may not match its contents - rebuild it with bun run package:cli" >&2
    fi
fi
[ -f "$OPP_TOOLS_TGZ" ] || { echo "error: $OPP_TOOLS_TGZ not found" >&2; exit 1; }
OPP_TOOLS_VERSION="$(tar -xzOf "$OPP_TOOLS_TGZ" package/package.json \
    | sed -n 's/^ *"version": *"\([^"]*\)".*/\1/p')"
[ -n "$OPP_TOOLS_VERSION" ] || { echo "error: no version in $OPP_TOOLS_TGZ" >&2; exit 1; }

STAGED="$HERE/opp-tools.tgz"
trap 'rm -f "$STAGED"' EXIT
cp "$OPP_TOOLS_TGZ" "$STAGED"

label_args=(--label "org.omnetpp.opp-tools.version=$OPP_TOOLS_VERSION")
[ -n "$OPP_TOOLS_COMMIT" ] && label_args+=(--label "org.omnetpp.opp-tools.commit=$OPP_TOOLS_COMMIT")
[ -n "$SOURCE" ] && label_args+=(--label "org.opencontainers.image.source=$SOURCE")

PDF_IMAGE="$IMAGE-pdf"

echo "Building $IMAGE and $PDF_IMAGE with $ENGINE (context: $CONTEXT, source: ${SOURCE:-unset})"
echo "Bundling opp-tools $OPP_TOOLS_VERSION${OPP_TOOLS_COMMIT:+ (vscode $OPP_TOOLS_COMMIT)} from $OPP_TOOLS_TGZ"
for target in base pdf; do
    tag="$IMAGE"
    [ "$target" = "pdf" ] && tag="$PDF_IMAGE"
    "$ENGINE" build --platform linux/amd64 --target "$target" -t "$tag" "${label_args[@]}" \
        -f "$HERE/Containerfile" "$CONTEXT"
done

if [ "$PUSH" = "1" ]; then
    SHA_TAG="${IMAGE%:*}:$(git -C "$CONTEXT" rev-parse --short HEAD)"
    echo "Tagging $SHA_TAG and $SHA_TAG-pdf; pushing all four references"
    "$ENGINE" tag "$IMAGE" "$SHA_TAG"
    "$ENGINE" tag "$PDF_IMAGE" "$SHA_TAG-pdf"
    for ref in "$IMAGE" "$SHA_TAG" "$PDF_IMAGE" "$SHA_TAG-pdf"; do
        "$ENGINE" push "$ref"
    done
fi
