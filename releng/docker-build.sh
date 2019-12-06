# Do not execute this file directly. It must be sourced from build-omnetpp-in-docker

export what=omnetpp
export BUILD_CORE_DISTRO=true
export BUILD_LINUX_DISTRO=true
export BUILD_WIN64_DISTRO=true
export BUILD_MACOSX_DISTRO=true
export GIT_VERSION=omnest-5.x

cd `dirname $0`/..
OMNETPP_ROOT=`pwd`

docker run --network none -v "$OMNETPP_ROOT/.git":/root/omnetpp-repo -e what -e GIT_VERSION \
       -e BUILD_CORE_DISTRO -e BUILD_LINUX_DISTRO -e BUILD_WIN64_DISTRO -e BUILD_MACOSX_DISTRO \
       omnetpp/distrobuild:eclipse4.13-tools180301-191212 \
       /bin/sh -c 'cd .. && rm -rf $OMNETPP_DIR && git clone $OMNETPP_REPO $OMNETPP_DIR && cd $OMNETPP_DIR && git checkout -f $GIT_VERSION && _scripts/build-omnetpp-src-dist' && \
docker cp $(docker ps -lq):/root/omnetpp/out/dist/ releng && \
docker rm $(docker ps -lq)
