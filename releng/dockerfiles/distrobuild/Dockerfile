FROM omnetpp/distrobuild-base:ubuntu18.04-macos11-180705
MAINTAINER Rudolf Hornig <rudi@omnetpp.org>

# provide the github ssh keys in the context dir that can access the OMNeT++ repo (./is_rsa, ./id_rsa.pub) - no password please!
# the tools directory should contain the Windows and macOS toolchains (i.e. ./tools/170717/...)

# omnetpp specific
ENV TOOLS_DIR /root/tools
ENV TOOLS_VERSION 180301
ENV OMNETPP_DIR /root/omnetpp
ENV PATH $PATH:$OMNETPP_DIR/bin

# fix ubuntu 18.04 invalid cert format. This may be removed if the base image is based on 18.04.1
# see: https://gist.github.com/mikaelhg/527204e746984cf9a33f7910bb8b4cb6
RUN /usr/bin/printf '\xfe\xed\xfe\xed\x00\x00\x00\x02\x00\x00\x00\x00\xe2\x68\x6e\x45\xfb\x43\xdf\xa4\xd9\x92\xdd\x41\xce\xb6\xb2\x1c\x63\x30\xd7\x92' > /etc/ssl/certs/java/cacerts && \
    /var/lib/dpkg/info/ca-certificates-java.postinst configure

RUN mkdir -p $OMNETPP_DIR
COPY omnetpp $OMNETPP_DIR
WORKDIR $OMNETPP_DIR

# update the eclipse build's MAVEN cache for faster builds

# getting only the dependnecies does not work correctly
#RUN (cd $OMNETPP_DIR/ui && (mvn dependency:go-offline -DexcludeGroupIds=org.omnetpp.ide || true)) && rm $OMNETPP_DIR/* -rf

# we dummy build the IDE and then delete the whole OMNeT++ source, so as a side effect the maven cache will be updated
RUN cp configure.user.dist configure.user && \
    ./configure WITH_TKENV=no WITH_QTENV=no WITH_OSG=no WITH_OSGEARTH=no && \
    make MODE=release ui -j$(nproc) && \
    ONLINE=true ui/releng/org.omnetpp.ide.product/build-ide-java && \
    rm $OMNETPP_DIR/* -rf

# copy the windows/macOS tools into the build image
COPY tools/$TOOLS_VERSION $TOOLS_DIR/$TOOLS_VERSION

# default variables controlling the build
ENV OMNETPP_REPO /root/omnetpp-repo
ENV GIT_VERSION origin/master
ENV WHAT omnetpp
ENV BUILD_CORE_DISTRO true
ENV BUILD_LINUX_DISTRO true
ENV BUILD_WIN64_DISTRO true
ENV BUILD_MACOSX_DISTRO true
CMD cd .. && rm -rf $OMNETPP_DIR && git clone $OMNETPP_REPO $OMNETPP_DIR && cd $OMNETPP_DIR && git checkout -f $GIT_VERSION && _scripts/build-omnetpp-src-dist
