# Do not execute this file directly. It must be sourced from build-omnetpp-dist

cd $(dirname $0)/..
OMNETPP_ROOT=$(pwd)

CONTAINER_NAME="omnetpp-build-$$"
DOC_DIR=""

cleanup() {
    echo "Cleaning up container..."
    docker rm -f "$CONTAINER_NAME" 2>/dev/null || true
    [ -z "$DOC_DIR" ] || rm -rf "$DOC_DIR"
}
trap cleanup EXIT INT TERM

# One build identifier for the archives and their documentation.
export BUILDID=${BUILDID:-$(date +%y%m%d)-$(git rev-parse --short "$GIT_VERSION")}

# The documentation is built in the doc-builder image here, because the build container has no network
# and no container engine; makedist copies it into the tree from the mount below.
DOC_MOUNT=""
if [ "$BUILD_DOC" = "true" ]; then
    DOC_DIR=$(mktemp -d "${TMPDIR:-/tmp}/omnetpp-doc.XXXXXX")
    releng/build-omnetpp-doc "$GIT_VERSION" "$DOC_DIR" || exit 1
    DOC_MOUNT="-v $DOC_DIR:/root/omnetpp-doc:ro"
fi

docker run --name "$CONTAINER_NAME" --network none \
       -v "$OMNETPP_ROOT":/root/omnetpp-repo $DOC_MOUNT \
       -e what -e GIT_VERSION -e BUILDID -e BUILD_DOC \
       -e BUILD_CORE_DISTRO -e BUILD_LINUX_X86_64_DISTRO -e BUILD_LINUX_AARCH64_DISTRO \
       -e BUILD_WIN32_X86_64_DISTRO -e BUILD_MACOS_X86_64_DISTRO -e BUILD_MACOS_AARCH64_DISTRO \
       ghcr.io/omnetpp/distrobuild:eclipse26_09-tools250612-261110

EXIT_CODE=$?

if [ $EXIT_CODE -eq 0 ]; then
    docker cp "$CONTAINER_NAME":/root/omnetpp/out/dist/ releng
else
    echo "Build failed with exit code $EXIT_CODE"
    exit $EXIT_CODE
fi
