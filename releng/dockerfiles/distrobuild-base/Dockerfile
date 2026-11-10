FROM ghcr.io/omnetpp/cross-compilers:ubuntu22.04-macos13.3-260504
LABEL maintainer="Rudolf Hornig <rudi@omnetpp.org>"


# install additional packages for building a complete OMNeT++ distro (with IDE); the documentation is built
# in the doc-builder image (see releng/build-omnetpp-doc)
RUN apt-get update && \
    apt-get upgrade -yy && \
    apt-get install --no-install-recommends -yy \
        bison flex lld openjdk-21-jdk swig \
        pkg-config python3 python3-dev \
        zip p7zip-full git wget curl && \
    apt-get clean && \
    apt-get autoclean

# uv creates the Python venv of the distrobuild image; pinned so that it does not change under us
COPY --from=ghcr.io/astral-sh/uv:0.13.1 /uv /uvx /bin/

# install maven manually as the 3.6.3 version in the repo does not work on Java17
RUN cd /opt && \
    wget https://downloads.apache.org/maven/maven-3/3.9.16/binaries/apache-maven-3.9.16-bin.tar.gz && \
    tar xzf apache-maven-3.9.16-bin.tar.gz && \
    rm apache-maven-3.9.16-bin.tar.gz && \
    mv apache-maven-3.9.16 apachemaven
ENV M2_HOME /opt/apachemaven
ENV MAVEN_HOME /opt/apachemaven
ENV PATH $M2_HOME/bin:$PATH
