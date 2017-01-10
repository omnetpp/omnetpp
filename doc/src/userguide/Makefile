FOP=fop

# on Fedora, change this to /usr/share/sgml/docbook/xsl-ns-stylesheets
DOCBOOK=/usr/share/xml/docbook/stylesheet/nwalsh

export XML_CATALOG_FILES=../docbook-dtd/catalog.xml

TITLE=User Guide

all: pdf html eclipse

include ../cover/cover.makefrag

html:
	xsltproc --nonet --xinclude --output tmp1.xml trans.xsl userguide.xml
	perl trans.pl <tmp1.xml >tmp.xml
	xsltproc --nonet --output userguide.html  $(DOCBOOK)/html/docbook.xsl tmp.xml
	rm tmp.xml
	rm -rf ../../userguide
	mkdir ../../userguide
	mkdir ../../userguide/icons
	mkdir ../../userguide/images
	mkdir ../../userguide/pictures
	cp userguide.html ../../userguide
	cp icons/*.png icons/*.gif ../../userguide/icons
	cp images/*.png ../../userguide/images
	cp pictures/*.png ../../userguide/pictures

.PHONY : eclipse
eclipse:
	xsltproc --nonet --xinclude --output tmp1.xml trans.xsl userguide.xml
	perl trans.pl <tmp1.xml >tmp.xml
	rm -rf eclipse
	mkdir eclipse
	mkdir eclipse/icons
	mkdir eclipse/images
	mkdir eclipse/pictures
	cp icons/*.png icons/*.gif eclipse/icons
	cp images/*.png eclipse/images
	cp pictures/*.png eclipse/pictures
	cd eclipse && xsltproc --nonet $(DOCBOOK)/eclipse/eclipse.xsl ../tmp.xml
	xmllint --noout eclipse/toc.xml
	rm tmp.xml

pdf:  chart-properties.xml processing-operations.xml cover
	xsltproc --nonet --xinclude --stringparam output pdf --output tmp1.xml trans.xsl userguide.xml
	perl trans.pl <tmp1.xml >tmp.xml
	perl -pe "s!\@DOCBOOK_STYLESHEET\@!file:///$(DOCBOOK)/fo/docbook.xsl!g" custom-fo-docbook.xsl >custom-fo-docbook-tmp.xsl
	xsltproc --nonet --output tmp.fo custom-fo-docbook-tmp.xsl tmp.xml
	$(FOP) -fo tmp.fo -c fop.xconf -pdf userguide.pdf
	pdftk userguide.pdf dump_data > tmp.metadata
	perl -i -pe 's/#160;/#32;/g' tmp.metadata
	pdftk C=cover.pdf B=../cover/copyright.pdf D=userguide.pdf cat C B D3-end output tmp.pdf
	pdftk tmp.pdf update_info tmp.metadata output ../../UserGuide.pdf
	rm tmp.fo tmp.xml custom-fo-docbook-tmp.xsl tmp.pdf tmp.metadata

clean:
	rm -f userguide.pdf userguide.html tmp1.xml tmp.xml cover.svg cover.pdf tmp.pdf tmp.metadata
	rm -rf eclipse
	rm -rf ../../userguide ../../UserGuide.pdf

# the following target should be called manually because it depends on binary file (scavetool) which is
# created only later durnig the run of the automatic build script.
# Re-run this target if any of the chart properties change in the CHART_POP_DIR, and check in the results
# into the repository

generate-tables: chart-properties processing-operations

CHART_PROP_DIR = ../../../ui/org.omnetpp.scave/src/org/omnetpp/scave/charting/properties

chart-properties: generate.pl $(CHART_PROP_DIR)/*Properties.java
	perl generate.pl chart-properties $(CHART_PROP_DIR) >chart-properties.xml

processing-operations: generate.pl
	perl generate.pl processing-operations >processing-operations.xml
