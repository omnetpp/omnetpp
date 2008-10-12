FOP=fop
export XML_CATALOG_FILES=../docbook-dtd/catalog.xml

all: pdf html

html:  
	xsltproc --xinclude --output tmp.xml trans.xsl userguide.xml
	xsltproc --output userguide.html  /usr/share/xml/docbook/stylesheet/nwalsh/html/docbook.xsl tmp.xml
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

eclipse:  
	xsltproc --xinclude --output tmp.xml trans.xsl userguide.xml
	xsltproc --output userguide.html  /usr/share/xml/docbook/stylesheet/nwalsh/eclipse/eclipse.xsl tmp.xml
	rm tmp.xml

pdf:   chart-properties.xml processing-operations.xml
	xsltproc --xinclude --output tmp.xml trans.xsl userguide.xml
	perl -pe "s!\@DOCBOOK_STYLESHEET\@!file:////usr/share/xml/docbook/stylesheet/nwalsh/fo/docbook.xsl!g" custom-fo-docbook.xsl >custom-fo-docbook-tmp.xsl
	xsltproc  --output tmp.fo custom-fo-docbook-tmp.xsl tmp.xml
	$(FOP) -fo tmp.fo -pdf userguide.pdf
	rm tmp.fo tmp.xml custom-fo-docbook-tmp.xsl
	cp userguide.pdf ../..

clean:
	rm userguide.pdf userguide.html

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
