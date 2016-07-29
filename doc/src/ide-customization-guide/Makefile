FOP=fop
DOCBOOK=/usr/share/xml/docbook/stylesheet/nwalsh
export XML_CATALOG_FILES=../docbook-dtd/catalog.xml

TITLE=IDE Customization Guide

all: pdf html eclipse

include ../cover/cover.makefrag

docbook:
	asciidoc -b docbook -d book -f asciidoc.conf $(DEFINE_OMNEST) ide-customization-guide.txt
	perl -i~ -pe "s/\@OMNETPP\@/$(WHAT_REPL)/g" ide-customization-guide.xml
	perl -i~ -pe "s/\@omnetpp\@/$(what_repl)/g" ide-customization-guide.xml
	perl -i~ -pe "s/\@version\@/$(VERSION_REPL)/g" ide-customization-guide.xml
	perl -i~ -pe "s,\@DOWNLOADSITE\@,$(DOWNLOADSITE),g" ide-customization-guide.xml

html: docbook
	perl trans.pl <ide-customization-guide.xml >tmp0.xml
	xsltproc --nonet --xinclude --output tmp.xml trans.xsl tmp0.xml
	xsltproc --nonet --output ide-customization-guide.html  $(DOCBOOK)/html/docbook.xsl tmp.xml
	rm -f tmp.xml tmp0.xml
	rm -rf ../../ide-customization-guide
	mkdir ../../ide-customization-guide
	mkdir ../../ide-customization-guide/images
	mkdir ../../ide-customization-guide/pictures
	cp ide-customization-guide.html ../../ide-customization-guide
	cp images/* ../../ide-customization-guide/images
	cp pictures/* ../../ide-customization-guide/pictures

.PHONY : eclipse
eclipse: docbook
	perl trans.pl <ide-customization-guide.xml >tmp0.xml
	xsltproc --nonet --xinclude --output tmp.xml trans.xsl tmp0.xml
	rm -rf eclipse
	mkdir eclipse
	mkdir eclipse/images
	mkdir eclipse/pictures
	cd eclipse && xsltproc --nonet $(DOCBOOK)/eclipse/eclipse.xsl ../tmp.xml
	xmllint --noout eclipse/toc.xml
	cp images/* eclipse/images
	cp pictures/* eclipse/pictures
	rm tmp.xml tmp0.xml

pdf: cover docbook
	perl trans.pl <ide-customization-guide.xml >tmp0.xml
	xsltproc --nonet --xinclude --output tmp.xml --stringparam output pdf trans.xsl tmp0.xml
	perl -pe "s!\@DOCBOOK_STYLESHEET\@!file:///$(DOCBOOK)/fo/docbook.xsl!g" custom-fo-docbook.xsl >custom-fo-docbook-tmp.xsl
	xsltproc --nonet --output tmp.fo custom-fo-docbook-tmp.xsl tmp.xml
	$(FOP) -fo tmp.fo -c fop.xconf -pdf ide-customization-guide.pdf
	pdftk ide-customization-guide.pdf dump_data > tmp.metadata
	perl -i -pe 's/#160;/#32;/g' tmp.metadata
	pdftk C=cover.pdf B=../cover/copyright.pdf D=ide-customization-guide.pdf cat C B D3-end output tmp.pdf
	pdftk tmp.pdf update_info tmp.metadata output ../../IDE-CustomizationGuide.pdf
	rm -f tmp.fo tmp.xml tmp0.xml custom-fo-docbook-tmp.xsl tmp.pdf tmp.metadata

clean:
	rm -rf ide-customization-guide.pdf ide-customization-guide.html ide-customization-guide.xml cover.svg cover.pdf tmp.pdf tmp.metadata
	rm -rf eclipse
	rm -rf ../../ide-customization-guide

