FOP=fop
export XML_CATALOG_FILES=../docbook-dtd/catalog.xml

all: pdf html

html:
	perl trans.pl <migration.xml >tmp0.xml
	xsltproc --xinclude --output tmp.xml trans.xsl tmp0.xml
	xsltproc --output migration.html  /usr/share/xml/docbook/stylesheet/nwalsh/html/docbook.xsl tmp.xml
	rm -f tmp.xml tmp0.xml
	rm -rf ../../migration
	mkdir ../../migration
	cp migration.html ../../migration

pdf:
	perl trans.pl <migration.xml >tmp0.xml
	xsltproc --xinclude --output tmp.xml trans.xsl tmp0.xml
	perl -pe "s!\@DOCBOOK_STYLESHEET\@!file:////usr/share/xml/docbook/stylesheet/nwalsh/fo/docbook.xsl!g" custom-fo-docbook.xsl >custom-fo-docbook-tmp.xsl
	xsltproc  --output tmp.fo custom-fo-docbook-tmp.xsl tmp.xml
	$(FOP) -fo tmp.fo -pdf migration.pdf
	rm -f tmp.fo tmp.xml tmp0.xml
	rm -f custom-fo-docbook-tmp.xsl
	cp migration.pdf ../..

clean:
	rm migration.pdf migration.html