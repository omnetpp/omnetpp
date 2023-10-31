# Do not execute this file directly. It must be sourced from build-omnetpp-in-docker

cd $(dirname $0)/..
OMNETPP_ROOT=$(pwd)

docker run --network none -v "$OMNETPP_ROOT":/root/omnetpp-repo -e what -e GIT_VERSION -e BUILD_DOC \
       -e BUILD_CORE_DISTRO -e BUILD_LINUX_X86_64_DISTRO -e BUILD_LINUX_AARCH64_DISTRO \
       -e BUILD_WIN32_X86_64_DISTRO -e BUILD_MACOS_X86_64_DISTRO -e BUILD_MACOS_AARCH64_DISTRO \
       ghcr.io/omnetpp/distrobuild:eclipse4.29-tools220810-231031 && \
docker cp $(docker ps -lq):/root/omnetpp/out/dist/ releng && \
docker rm $(docker ps -lq)
