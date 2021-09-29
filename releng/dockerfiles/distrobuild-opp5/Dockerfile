FROM  omnetpp/distrobuild:eclipse4.21-tools210809-210927
LABEL maintainer="Rudolf Hornig <rudi@omnetpp.org>"

# install additional packages for building a complete OMNeT++ 5 distro (doc related packages that are not needed for the 6.0 images)
RUN apt-get update && \
    apt-get upgrade -yy && \
    apt-get install --no-install-recommends -yy \
        asciidoc docbook-xml docbook-xsl xsltproc fop qpdf && \
    apt-get clean && \
    apt-get autoclean
