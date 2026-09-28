# Do not execute this file directly. It must be sourced from build-omnetpp-dist

cd $(dirname $0)/..
OMNETPP_ROOT=$(pwd)

CONTAINER_NAME="omnetpp-build-$$"

cleanup() {
    echo "Cleaning up container..."
    docker rm -f "$CONTAINER_NAME" 2>/dev/null || true
}
trap cleanup EXIT INT TERM

docker run --name "$CONTAINER_NAME" --network none \
       -v "$OMNETPP_ROOT":/root/omnetpp-repo \
       -e what -e GIT_VERSION -e BUILD_DOC \
       -e BUILD_CORE_DISTRO -e BUILD_LINUX_X86_64_DISTRO -e BUILD_LINUX_AARCH64_DISTRO \
       -e BUILD_WIN32_X86_64_DISTRO -e BUILD_MACOS_X86_64_DISTRO -e BUILD_MACOS_AARCH64_DISTRO \
       ghcr.io/omnetpp/distrobuild:eclipse26_09-tools250612-260928

EXIT_CODE=$?

if [ $EXIT_CODE -eq 0 ]; then
    docker cp "$CONTAINER_NAME":/root/omnetpp/out/dist/ releng
else
    echo "Build failed with exit code $EXIT_CODE"
    exit $EXIT_CODE
fi
