FOP=fop
export XML_CATALOG_FILES=../docbook-dtd/catalog.xml

all: pdf html

html:
	xsltproc --xinclude --output tmp.xml trans.xsl Migration.xml
	xsltproc --output Migration.html  /usr/share/xml/docbook/stylesheet/nwalsh/html/docbook.xsl tmp.xml
	rm tmp.xml

pdf:
	xsltproc --xinclude --output tmp.xml trans.xsl Migration.xml
	perl -pe "s!\@DOCBOOK_STYLESHEET\@!file:////usr/share/xml/docbook/stylesheet/nwalsh/fo/docbook.xsl!g" custom-fo-docbook.xsl >custom-fo-docbook-tmp.xsl
	xsltproc  --output tmp.fo custom-fo-docbook-tmp.xsl tmp.xml
	$(FOP) -fo tmp.fo -pdf Migration.pdf
	rm tmp.fo tmp.xml
	cp Migration.pdf ../..


