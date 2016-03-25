FOP=fop
DOCBOOK=/usr/share/xml/docbook/stylesheet/nwalsh
export XML_CATALOG_FILES=../docbook-dtd/catalog.xml

TITLE=Installation Guide

ifneq ($(what),omnest)
  WHAT_REPL = OMNeT&\#x2060;+&\#x2060;+
  what_repl = omnetpp
  DOWNLOADSITE = http://omnetpp.org
else
  WHAT_REPL = OMNEST
  what_repl = omnest
  DOWNLOADSITE = http://omnest.com
  DEFINE_OMNEST = -a OMNEST=1
endif

ifneq ($(VERSION),)
  VERSION_REPL = $(VERSION)
else
  VERSION_REPL = 5.X
endif

all: pdf html

include ../cover/cover.makefrag

docbook:
	asciidoc -b docbook -d book -f asciidoc.conf $(DEFINE_OMNEST) -a DOWNLOADSITE=$(DOWNLOADSITE) installguide.txt
	perl -i~ -pe "s/\@OMNETPP\@/$(WHAT_REPL)/g" installguide.xml
	perl -i~ -pe "s/\@omnetpp\@/$(what_repl)/g" installguide.xml 
	perl -i~ -pe "s/\@version\@/$(VERSION_REPL)/g" installguide.xml

html: docbook
	xsltproc --nonet --xinclude --output tmp1.xml trans.xsl installguide.xml
	perl trans.pl <tmp1.xml >tmp.xml
	xsltproc --nonet --output installguide.html  $(DOCBOOK)/html/docbook.xsl tmp.xml
	rm tmp.xml
	rm -rf ../../installguide
	mkdir ../../installguide
	mkdir ../../installguide/icons
	mkdir ../../installguide/images
	mkdir ../../installguide/pictures
	cp installguide.html ../../installguide
	-cp icons/*.png icons/*.gif ../../installguide/icons
	-cp images/*.png ../../installguide/images
	-cp pictures/*.png ../../installguide/pictures

.PHONY : eclipse
eclipse: docbook
	xsltproc --nonet --xinclude --output tmp1.xml trans.xsl installguide.xml
	perl trans.pl <tmp1.xml >tmp.xml
	rm -rf eclipse
	mkdir eclipse
	mkdir eclipse/icons
	mkdir eclipse/images
	mkdir eclipse/pictures
	-cp icons/*.png icons/*.gif eclipse/icons
	-cp images/*.png eclipse/images
	-cp pictures/*.png eclipse/pictures
	cd eclipse && xsltproc --nonet $(DOCBOOK)/eclipse/eclipse.xsl ../tmp.xml
	xmllint --noout eclipse/toc.xml
	rm tmp.xml

pdf: docbook cover
	xsltproc --nonet --xinclude --stringparam output pdf --output tmp1.xml trans.xsl installguide.xml
	perl trans.pl <tmp1.xml >tmp.xml
	perl -pe "s!\@DOCBOOK_STYLESHEET\@!file:///$(DOCBOOK)/fo/docbook.xsl!g" custom-fo-docbook.xsl >custom-fo-docbook-tmp.xsl
	xsltproc --nonet --output tmp.fo custom-fo-docbook-tmp.xsl tmp.xml
	$(FOP) -fo tmp.fo -c ../fop.xconf -pdf installguide.pdf
	rm tmp.fo tmp.xml custom-fo-docbook-tmp.xsl
	pdftk installguide.pdf dump_data > tmp.metadata
	perl -i -pe 's/#160;/#32;/g' tmp.metadata
	pdftk C=cover.pdf B=../cover/copyright.pdf D=installguide.pdf cat C B D3-end output tmp.pdf
	pdftk tmp.pdf update_info tmp.metadata output ../../InstallGuide.pdf
	rm -f tmp.pdf tmp.metadata

clean:
	rm -f installguide.pdf installguide.html tmp1.xml tmp.xml cover.svg cover.pdf
	rm -rf eclipse
	rm -rf ../../installguide


