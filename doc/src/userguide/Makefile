FOP=fop
export XML_CATALOG_FILES=dtd/catalog.xml

all: pdf html

html:  
	xsltproc --xinclude --output tmp.xml trans.xsl userguide.xml
	xsltproc --output userguide.html  /usr/share/xml/docbook/stylesheet/nwalsh/html/docbook.xsl tmp.xml
	rm tmp.xml

pdf:   chart-properties.xml
	xsltproc --xinclude --output tmp.xml trans.xsl userguide.xml
	perl -pe "s!\@DOCBOOK_STYLESHEET\@!file:////usr/share/xml/docbook/stylesheet/nwalsh/fo/docbook.xsl!g" custom-fo-docbook.xsl >custom-fo-docbook-tmp.xsl
	xsltproc  --output tmp.fo custom-fo-docbook-tmp.xsl tmp.xml
	$(FOP) -fo tmp.fo -pdf userguide.pdf
	rm tmp.fo tmp.xml
	cp userguide.pdf ../..

CHART_PROP_DIR = ../../../ui/org.omnetpp.scave/src/org/omnetpp/scave/charting/properties

chart-properties.xml: chart-properties.pl $(CHART_PROP_DIR)/*Properties.java
	perl chart-properties.pl $(CHART_PROP_DIR) >chart-properties.xml
