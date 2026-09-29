FROM ghcr.io/omnetpp/cross-compilers:ubuntu22.04-macos13.3-260504
LABEL maintainer="Rudolf Hornig <rudi@omnetpp.org>"


# install additional packages for building a complete OMNeT++ distro (with IDE and docs)
RUN apt-get update && \
    apt-get upgrade -yy && \
    apt-get install --no-install-recommends -yy \
        bison flex lld openjdk-21-jdk swig doxygen graphviz inkscape librsvg2-bin \
        pkg-config python3 python3-dev python3-pip python3-setuptools \
        texlive texlive-latex-extra latexmk \
        txt2html ssh zip p7zip-full git wget curl \
        docbook-xml docbook-xsl qpdf && \
    apt-get clean && \
    apt-get autoclean

# pandoc converts the Markdown docstrings in the Python API reference; pinned to match the doc-builder image
ARG PANDOC_VERSION=3.10
RUN curl -fsSL -o /tmp/pandoc.deb \
        "https://github.com/jgm/pandoc/releases/download/${PANDOC_VERSION}/pandoc-${PANDOC_VERSION}-1-amd64.deb" && \
    dpkg -i /tmp/pandoc.deb && \
    rm -f /tmp/pandoc.deb

# uv builds the doc venv in the distrobuild image; pinned so lock handling does not change under us
COPY --from=ghcr.io/astral-sh/uv:0.11.8 /uv /uvx /bin/

# install maven manually as the 3.6.3 version in the repo does not work on Java17
RUN cd /opt && \
    wget https://downloads.apache.org/maven/maven-3/3.9.16/binaries/apache-maven-3.9.16-bin.tar.gz && \
    tar xzf apache-maven-3.9.16-bin.tar.gz && \
    rm apache-maven-3.9.16-bin.tar.gz && \
    mv apache-maven-3.9.16 apachemaven
ENV M2_HOME /opt/apachemaven
ENV MAVEN_HOME /opt/apachemaven
ENV PATH $M2_HOME/bin:$PATH
