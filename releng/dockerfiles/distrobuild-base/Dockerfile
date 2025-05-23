FROM ghcr.io/omnetpp/cross-compilers:ubuntu20.04-macos12.3-240124
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
    apt-get autoclean && \
    python3 -m pip install --upgrade sphinx sphinx-material sphinxcontrib-images sphinxcontrib-doxylink sphinx-autobuild \
               cppyy dask distributed IPython matplotlib numpy pandas scipy sqlalchemy

# install maven manually as the 3.6.3 version in the repo does not work on Java17
RUN cd /opt && \
    wget https://downloads.apache.org/maven/maven-3/3.9.8/binaries/apache-maven-3.9.8-bin.tar.gz && \
    tar xzf apache-maven-3.9.8-bin.tar.gz && \
    rm apache-maven-3.9.8-bin.tar.gz && \
    mv apache-maven-3.9.8 apachemaven
ENV M2_HOME /opt/apachemaven
ENV MAVEN_HOME /opt/apachemaven
ENV PATH $M2_HOME/bin:$PATH
