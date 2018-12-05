FROM omnetpp/cross-compilers:ubuntu18.04-macos11-180705
MAINTAINER Rudolf Hornig <rudi@omnetpp.org>

# install additional packages for building a complete OMNeT++ distro (with IDE and docs)
RUN apt-get update && \
    apt-get upgrade -yy && \
    apt-get install --no-install-recommends -yy \
        bison flex default-jdk swig doxygen asciidoc graphviz texlive \
        texlive-latex-extra inkscape docbook-xml docbook-xsl \
        qpdf txt2html xsltproc fop maven ssh zip p7zip-full git && \
    apt-get clean && \
    apt-get autoclean
