# Do not execute this file directly. It must be sourced from build-omnetpp-in-docker

cd `dirname $0`/..
OMNETPP_ROOT=`pwd`

docker run --network none -v "$OMNETPP_ROOT":/root/omnetpp-repo -e what -e GIT_VERSION -e BUILD_DOC \
       -e BUILD_CORE_DISTRO -e BUILD_LINUX_DISTRO -e BUILD_WIN64_DISTRO -e BUILD_MACOSX_DISTRO \
       omnetpp/distrobuild:eclipse4.18-tools201218-201218 && \
docker cp $(docker ps -lq):/root/omnetpp/out/dist/ releng && \
docker rm $(docker ps -lq)
