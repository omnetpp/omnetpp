FROM omnetpp/distrobuild-base:ubuntu20.04-macos11.3-210713
LABEL maintainer="Rudolf Hornig <rudi@omnetpp.org>"

# during docker image builds, the 'tools' directory in the context dir should contain the Windows and macOS toolchains (i.e. ./tools/170717/...)
SHELL ["/bin/bash", "-c"]
ENV TOOLS_DIR /root/tools
ENV TOOLS_VERSION 220331
ENV OMNETPP_DIR /root/omnetpp

# clone the directory
WORKDIR /root

# clone the repository either remotely or from a local repo in the context directory
#RUN git clone -n https://github.com/omnetpp/omnetpp.git $OMNETPP_DIR
# optionally you can use local bare repo, called 'omnetpp-repo' from the context dir
# use 'git clone -n ../../../.git omnetpp-repo' to create it
COPY omnetpp-repo/ $OMNETPP_DIR

# copy the windows/macOS tools into the build image
COPY tools/$TOOLS_VERSION $TOOLS_DIR/$TOOLS_VERSION

# update the eclipse build's MAVEN cache for faster builds
WORKDIR $OMNETPP_DIR

# getting only the dependnecies does not work correctly (not everything is cached unfortunately, so a full build is required)
#RUN git checkout master && (cd $OMNETPP_DIR/ui && (mvn dependency:go-offline -Dwhat=omnetpp -fn || true)) ; rm -rf $OMNETPP_DIR

# we dummy build the IDE and then delete the whole OMNeT++ source, so as a side effect the maven cache will be updated
RUN git checkout master && \
    cp configure.user.dist configure.user && \
    source ./setenv -f && \
    ./configure WITH_QTENV=no WITH_OSG=no WITH_OSGEARTH=no && \
    make MODE=release ui -j$(nproc) && \
    OFFLINE=no releng/ide/java/build-ide-java && \
    rm -rf $OMNETPP_DIR

# while running this image you should either map a local OMNeT++ repo from the host to the /root/omnetpp-repo dir
# or specify a remote OMNeT++ repo URL in the OMNETPP_REPO environment variable.
# default variables controlling the OMNeT++ build

ENV OMNETPP_REPO /root/omnetpp-repo
ENV GIT_VERSION origin/master
ENV WHAT omnetpp
ENV BUILD_CORE_DISTRO true
ENV BUILD_LINUX_X86_64_DISTRO true
ENV BUILD_LINUX_AARCH64_DISTRO true
ENV BUILD_WIN32_X86_64_DISTRO true
ENV BUILD_MACOS_X86_64_DISTRO true
ENV BUILD_MACOS_AARCH64_DISTRO true
CMD git clone -n $OMNETPP_REPO $OMNETPP_DIR && cd $OMNETPP_DIR && git checkout -f $GIT_VERSION && releng/makedist
