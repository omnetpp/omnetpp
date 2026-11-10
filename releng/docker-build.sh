# Do not execute this file directly. It must be sourced from build-omnetpp-dist

cd $(dirname $0)/..
OMNETPP_ROOT=$(pwd)

CONTAINER_NAME="omnetpp-build-$$"
DOC_DIR=""
REPO_DIR=""

cleanup() {
    echo "Cleaning up container..."
    docker rm -f "$CONTAINER_NAME" 2>/dev/null || true
    [ -z "$DOC_DIR" ] || rm -rf "$DOC_DIR"
    [ -z "$REPO_DIR" ] || rm -rf "$REPO_DIR"
}
trap cleanup EXIT INT TERM

# The build container clones a fresh clone of the repository, not the working copy's: the git of the
# image cannot read repositories with newer extensions (e.g. relativeworktrees), and the build sees
# only commits, never the working tree.
REPO_DIR=$(mktemp -d "${TMPDIR:-/tmp}/omnetpp-repo.XXXXXX")
git clone -q -n "$OMNETPP_ROOT" "$REPO_DIR" || exit 1
git -C "$REPO_DIR" cat-file -e "$GIT_VERSION^{commit}" 2>/dev/null ||
    git -C "$REPO_DIR" fetch -q origin "$GIT_VERSION" || exit 1

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
       -v "$REPO_DIR":/root/omnetpp-repo:ro $DOC_MOUNT \
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
