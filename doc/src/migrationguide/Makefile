FOP=fop
DOCBOOK=/usr/share/xml/docbook/stylesheet/nwalsh
export XML_CATALOG_FILES=../docbook-dtd/catalog.xml

TITLE=Migration Guide

all: pdf html eclipse

include ../cover/cover.makefrag

html:
	perl trans.pl <migration.xml >tmp0.xml
	xsltproc --nonet --xinclude --output tmp.xml trans.xsl tmp0.xml
	xsltproc --nonet --output migration.html  $(DOCBOOK)/html/docbook.xsl tmp.xml
	rm -f tmp.xml tmp0.xml
	rm -rf ../../migration
	mkdir ../../migration
	mkdir ../../migration/images
	cp migration.html ../../migration
	cp images/* ../../migration/images

.PHONY : eclipse
eclipse:
	perl trans.pl <migration.xml >tmp0.xml
	xsltproc --nonet --xinclude --output tmp.xml trans.xsl tmp0.xml
	rm -rf eclipse
	mkdir eclipse
	mkdir eclipse/images
	cd eclipse && xsltproc --nonet $(DOCBOOK)/eclipse/eclipse.xsl ../tmp.xml
	cp images/* eclipse/images
	rm tmp.xml tmp0.xml

pdf: cover
	perl trans.pl <migration.xml >tmp0.xml
	xsltproc --nonet --xinclude --output tmp.xml --stringparam output pdf trans.xsl tmp0.xml
	perl -pe "s!\@DOCBOOK_STYLESHEET\@!file:///$(DOCBOOK)/fo/docbook.xsl!g" custom-fo-docbook.xsl >custom-fo-docbook-tmp.xsl
	xsltproc --nonet --output tmp.fo custom-fo-docbook-tmp.xsl tmp.xml
	$(FOP) -fo tmp.fo -c ../fop.xconf -pdf migration.pdf
	rm -f tmp.fo tmp.xml tmp0.xml
	rm -f custom-fo-docbook-tmp.xsl
	pdftk C=cover.pdf B=../cover/copyright.pdf D=migration.pdf cat C B D3-end output ../../Migration.pdf

clean:
	rm -rf migration.pdf migration.html cover.svg cover.pdf
	rm -rf eclipse
	rm -rf ../../migration ../../Migration.pdf

