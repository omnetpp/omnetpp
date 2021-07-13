FROM omnetpp/cross-compilers:ubuntu20.04-macos11.3-210713
LABEL maintainer="Rudolf Hornig <rudi@omnetpp.org>"

# install additional packages for building a complete OMNeT++ distro (with IDE and docs)
RUN apt-get update && \
    apt-get upgrade -yy && \
    apt-get install --no-install-recommends -yy \
        bison flex default-jdk swig doxygen graphviz inkscape python3 python3-pip python3-setuptools \
        texlive texlive-latex-extra latexmk \
        txt2html maven ssh zip p7zip-full git \
        docbook-xml docbook-xsl qpdf && \
    apt-get clean && \
    apt-get autoclean && \
    python3 -m pip install --upgrade sphinx sphinx-material sphinxcontrib-images sphinxcontrib-doxylink sphinx-autobuild
