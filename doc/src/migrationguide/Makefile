FOP=fop
export XML_CATALOG_FILES=../docbook-dtd/catalog.xml

all: pdf html

html:
	xsltproc --xinclude --output tmp.xml trans.xsl migration.xml
	xsltproc --output migration.html  /usr/share/xml/docbook/stylesheet/nwalsh/html/docbook.xsl tmp.xml
	-rm tmp.xml 2>/dev/null
	-rm -rf ../../migration 2>/dev/null
	mkdir ../../migration
	cp migration.html ../../migration

pdf:
	xsltproc --xinclude --output tmp.xml trans.xsl migration.xml
	perl -pe "s!\@DOCBOOK_STYLESHEET\@!file:////usr/share/xml/docbook/stylesheet/nwalsh/fo/docbook.xsl!g" custom-fo-docbook.xsl >custom-fo-docbook-tmp.xsl
	xsltproc  --output tmp.fo custom-fo-docbook-tmp.xsl tmp.xml
	$(FOP) -fo tmp.fo -pdf migration.pdf
	-rm tmp.fo tmp.xml 2>/dev/null
	-rm custom-fo-docbook-tmp.xsl 2>/dev/null
	cp migration.pdf ../..

clean:
	rm migration.pdf migration.html