FROM ubuntu:20.04 as builder

# Contains the (cross-)compilers and dependencies needed to build
# OMNeT++ core (without IDE or docs) for all 3 supported platforms.

# based on osxcross docker image from https://github.com/andrew-d/docker-osxcross

# This image is used (at the moment) as a foundation for: The OMNeT++/OMNEST
# release tarball building images, and for the image used on Travis to test INET.


# Install build tools for osxcross
RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get upgrade -yy && \
    DEBIAN_FRONTEND=noninteractive apt-get install -yy \
        automake bison cmake curl file flex git libtool pkg-config python texinfo vim wget && \
    DEBIAN_FRONTEND=noninteractive apt-get clean

# Install osxcross
ENV OSXCROSS_SDK_VERSION 11.3
RUN mkdir /opt/osxcross && \
    cd /opt && \
    git clone https://github.com/tpoechtrager/osxcross.git && \
    cd osxcross && \
    git checkout 447cf3b3 && \
    ./tools/get_dependencies.sh && \
    curl -L -o ./tarballs/MacOSX${OSXCROSS_SDK_VERSION}.sdk.tar.xz \
        https://github.com/phracker/MacOSX-SDKs/releases/download/${OSXCROSS_SDK_VERSION}/MacOSX${OSXCROSS_SDK_VERSION}.sdk.tar.xz && \
    yes | UNATTENDED=1 ./build.sh

# second stage build (copy only the final resuts from the previous build process)
FROM ubuntu:20.04
RUN mkdir -p /opt/osxcross/target
WORKDIR /opt/osxcross/target
COPY --from=builder /opt/osxcross/target/ .
ENV PATH $PATH:/opt/osxcross/target/bin

# add OMNeT++ specific dependencies and MinGW64+aarch64 corss compilers
RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get upgrade -yy && \
    DEBIAN_FRONTEND=noninteractive apt-get install --no-install-recommends -yy \
        build-essential cmake crossbuild-essential-arm64 perl tofrodos libxml2-dev zlib1g-dev libxml2-utils \
        mingw-w64 clang llvm-dev && \
    apt-get clean && \
    apt-get autoclean
