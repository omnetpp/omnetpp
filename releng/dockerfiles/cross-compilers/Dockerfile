FROM ubuntu:18.04 as builder

# Contains the (cross-)compilers and dependencies needed to build
# OMNeT++ core (without IDE or docs) for all 3 supported platforms.

# based on osxcross docker image from https://github.com/andrew-d/docker-osxcross

# This image is used (at the moment) as a foundation for: The OMNeT++/OMNEST
# release tarball building images, and for the image used on Travis to test INET.


# Install build tools for osxcross
RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get upgrade -yy && \
    DEBIAN_FRONTEND=noninteractive apt-get install -yy \
        automake bison curl file flex git libtool pkg-config python texinfo vim wget && \
    DEBIAN_FRONTEND=noninteractive apt-get clean

# Install osxcross
# NOTE: The Docker Hub's build machines run varying types of CPUs, so an image
# built with `-march=native` on one of those may not run on every machine - I
# ran into this problem when the images wouldn't run on my 2013-era Macbook
# Pro.  As such, we remove this flag entirely.
ENV OSXCROSS_SDK_VERSION 10.11
RUN mkdir /opt/osxcross && \
    cd /opt && \
    git clone https://github.com/tpoechtrager/osxcross.git && \
    cd osxcross && \
    git checkout 474f359d2f27ff68916a064f0138c9188c63db7d && \
    sed -i -e 's|-march=native||g' ./build_clang.sh ./wrapper/build.sh && \
    ./tools/get_dependencies.sh && \
    curl -L -o ./tarballs/MacOSX${OSXCROSS_SDK_VERSION}.sdk.tar.xz \
        https://s3.amazonaws.com/andrew-osx-sdks/MacOSX${OSXCROSS_SDK_VERSION}.sdk.tar.xz && \
    yes | PORTABLE=true ./build.sh

# sencod stage build (copy only the final resuts from the previous build process)
FROM ubuntu:18.04
RUN mkdir -p /opt/osxcross/target
WORKDIR /opt/osxcross/target
COPY --from=builder /opt/osxcross/target/ .
ENV PATH $PATH:/opt/osxcross/target/bin

# add OMNeT++ specific dependencies and MinGW64 compiler
RUN apt-get update && \
    apt-get install --no-install-recommends -yy \
        build-essential perl tofrodos libxml2-dev zlib1g-dev libxml2-utils mingw-w64 clang llvm-dev && \
    apt-get clean && \
    apt-get autoclean
