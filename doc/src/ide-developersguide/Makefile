FOP=fop
DOCBOOK=/usr/share/xml/docbook/stylesheet/nwalsh
export XML_CATALOG_FILES=../docbook-dtd/catalog.xml

TITLE=IDE Developers Guide

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
  VERSION_REPL = 4.X
endif

all: pdf html eclipse

include ../cover/cover.makefrag

docbook:
	asciidoc -b docbook -d book -f asciidoc.conf $(DEFINE_OMNEST) ide-developersguide.txt
	perl -i~ -pe "s/\@OMNETPP\@/$(WHAT_REPL)/g" ide-developersguide.xml
	perl -i~ -pe "s/\@omnetpp\@/$(what_repl)/g" ide-developersguide.xml 
	perl -i~ -pe "s/\@version\@/$(VERSION_REPL)/g" ide-developersguide.xml
	perl -i~ -pe "s,\@DOWNLOADSITE\@,$(DOWNLOADSITE),g" ide-developersguide.xml

html: docbook
	xsltproc --nonet --xinclude --output tmp1.xml trans.xsl ide-developersguide.xml
	perl trans.pl <tmp1.xml >tmp.xml
	xsltproc --nonet --output ide-developersguide.html  $(DOCBOOK)/html/docbook.xsl tmp.xml
	rm tmp.xml
	rm -rf ../../ide-developersguide
	mkdir ../../ide-developersguide
	mkdir ../../ide-developersguide/icons
	mkdir ../../ide-developersguide/images
	mkdir ../../ide-developersguide/pictures
	cp ide-developersguide.html ../../ide-developersguide
	-cp icons/*.png icons/*.gif ../../ide-developersguide/icons
	-cp images/*.png ../../ide-developersguide/images
	-cp pictures/*.png ../../ide-developersguide/pictures

.PHONY : eclipse
eclipse: docbook
	xsltproc --nonet --xinclude --output tmp1.xml trans.xsl ide-developersguide.xml
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
	xsltproc --nonet --xinclude --stringparam output pdf --output tmp1.xml trans.xsl ide-developersguide.xml
	perl trans.pl <tmp1.xml >tmp.xml
	perl -pe "s!\@DOCBOOK_STYLESHEET\@!file:///$(DOCBOOK)/fo/docbook.xsl!g" custom-fo-docbook.xsl >custom-fo-docbook-tmp.xsl
	xsltproc --nonet --output tmp.fo custom-fo-docbook-tmp.xsl tmp.xml
	$(FOP) -fo tmp.fo -c fop.xconf -pdf ide-developersguide.pdf
	pdftk ide-developersguide.pdf dump_data > tmp.metadata
	perl -i -pe 's/#160;/#32;/g' tmp.metadata
	pdftk C=cover.pdf B=../cover/copyright.pdf D=ide-developersguide.pdf cat C B D3-end output tmp.pdf
	pdftk tmp.pdf update_info tmp.metadata output ../../IDE-DevelopersGuide.pdf
	rm tmp.fo tmp.xml custom-fo-docbook-tmp.xsl tmp.pdf tmp.metadata

clean:
	rm -f ide-developersguide.pdf ide-developersguide.html ide-developersguide.xml tmp1.xml tmp.xml cover.svg cover.pdf tmp.pdf tmp.metadata
	rm -rf eclipse
	rm -rf ../../ide-developersguide ../../IDE-DevelopersGuide.pdf


