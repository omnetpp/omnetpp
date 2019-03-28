FROM omnetpp/distrobuild-base:ubuntu18.04-macos11-180705
MAINTAINER Rudolf Hornig <rudi@omnetpp.org>

# during docker image builds, the 'tools' directory in the context dir should contain the Windows and macOS toolchains (i.e. ./tools/170717/...)
ENV TOOLS_DIR /root/tools
ENV TOOLS_VERSION 180301
ENV OMNETPP_DIR /root/omnetpp
ENV PATH $PATH:$OMNETPP_DIR/bin

# clone the directory
WORKDIR /root

# clone the repository either remotely or from a local repo in the context directory
#RUN git clone -n https://github.com/omnetpp/omnetpp.git $OMNETPP_DIR
# optionally you can use local bare repo, called 'omnetpp-repo' from the context dir
# use 'git clone -n ../../../.git omnetpp-repo' to create it
COPY omnetpp-repo/ $OMNETPP_DIR

# copy the windows/macOS tools into the build image
COPY tools/$TOOLS_VERSION $TOOLS_DIR/$TOOLS_VERSION
# copy the windows platform JDK header files to a known location as they are needed for the cross compiler build on Windows
COPY tools/$TOOLS_VERSION/win64/jdk-include $TOOLS_DIR/jdk-include

# update the eclipse build's MAVEN cache for faster builds
WORKDIR $OMNETPP_DIR
# fix ubuntu 18.04 invalid cert format. This may be removed if the base image is based on 18.04.1
# see: https://gist.github.com/mikaelhg/527204e746984cf9a33f7910bb8b4cb6
RUN /usr/bin/printf '\xfe\xed\xfe\xed\x00\x00\x00\x02\x00\x00\x00\x00\xe2\x68\x6e\x45\xfb\x43\xdf\xa4\xd9\x92\xdd\x41\xce\xb6\xb2\x1c\x63\x30\xd7\x92' > /etc/ssl/certs/java/cacerts && \
    /var/lib/dpkg/info/ca-certificates-java.postinst configure

# getting only the dependnecies does not work correctly (not everything is cached unfortunately, so a full build is required)
#RUN git checkout 4f05766d && (cd $OMNETPP_DIR/ui && (mvn dependency:go-offline -Dwhat=omnetpp -fn || true)) ; rm -rf $OMNETPP_DIR

# we dummy build the IDE and then delete the whole OMNeT++ source, so as a side effect the maven cache will be updated
RUN git checkout 4f05766d && \
    cp configure.user.dist configure.user && \
    ./configure WITH_QTENV=no WITH_OSG=no WITH_OSGEARTH=no && \
    make MODE=release ui -j$(nproc) && \
    releng/ide/java/build-ide-java && \
    rm -rf $OMNETPP_DIR

# while running this image you should either map a local OMNeT++ repo from the host to the /root/omnetpp-repo dir
# or specify a remote OMNeT++ repo URL in the OMNETPP_REPO environment variable.
# default variables controlling the OMNeT++ build

ENV OMNETPP_REPO /root/omnetpp-repo
ENV GIT_VERSION origin/master
ENV WHAT omnetpp
ENV BUILD_CORE_DISTRO true
ENV BUILD_LINUX_DISTRO true
ENV BUILD_WIN64_DISTRO true
ENV BUILD_MACOSX_DISTRO true
CMD git clone -n $OMNETPP_REPO $OMNETPP_DIR && cd $OMNETPP_DIR && git checkout -f $GIT_VERSION && releng/makedist
