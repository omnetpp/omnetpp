FOP=fop
DOCBOOK=/usr/share/xml/docbook/stylesheet/nwalsh
export XML_CATALOG_FILES=../docbook-dtd/catalog.xml

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

all: pdf

docbook:
	asciidoc -b docbook -d book -f asciidoc.conf $(DEFINE_OMNEST) IDE-DevelopersGuide.txt
	perl -i~ -pe "s/\@OMNETPP\@/$(WHAT_REPL)/g" IDE-DevelopersGuide.xml
	perl -i~ -pe "s/\@omnetpp\@/$(what_repl)/g" IDE-DevelopersGuide.xml 
	perl -i~ -pe "s/\@version\@/$(VERSION_REPL)/g" IDE-DevelopersGuide.xml
	perl -i~ -pe "s,\@DOWNLOADSITE\@,$(DOWNLOADSITE),g" IDE-DevelopersGuide.xml

html: docbook
	xsltproc --nonet --xinclude --output tmp1.xml trans.xsl IDE-DevelopersGuide.xml
	perl trans.pl <tmp1.xml >tmp.xml
	xsltproc --nonet --output IDE-DevelopersGuide.html  $(DOCBOOK)/html/docbook.xsl tmp.xml
	rm tmp.xml
	rm -rf ../../IDE-DevelopersGuide
	mkdir ../../IDE-DevelopersGuide
	mkdir ../../IDE-DevelopersGuide/icons
	mkdir ../../IDE-DevelopersGuide/images
	mkdir ../../IDE-DevelopersGuide/pictures
	cp IDE-DevelopersGuide.html ../../IDE-DevelopersGuide
	-cp icons/*.png icons/*.gif ../../IDE-DevelopersGuide/icons
	-cp images/*.png ../../IDE-DevelopersGuide/images
	-cp pictures/*.png ../../IDE-DevelopersGuide/pictures

.PHONY : eclipse
eclipse: docbook
	xsltproc --nonet --xinclude --output tmp1.xml trans.xsl IDE-DevelopersGuide.xml
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
	rm tmp.xml

pdf: docbook
	xsltproc --nonet --xinclude --stringparam output pdf --output tmp1.xml trans.xsl IDE-DevelopersGuide.xml
	perl trans.pl <tmp1.xml >tmp.xml
	perl -pe "s!\@DOCBOOK_STYLESHEET\@!file:///$(DOCBOOK)/fo/docbook.xsl!g" custom-fo-docbook.xsl >custom-fo-docbook-tmp.xsl
	xsltproc --nonet --output tmp.fo custom-fo-docbook-tmp.xsl tmp.xml
	$(FOP) -fo tmp.fo -c ../fop.xconf -pdf IDE-DevelopersGuide.pdf
	rm tmp.fo tmp.xml custom-fo-docbook-tmp.xsl
	cp IDE-DevelopersGuide.pdf ../..

clean:
	rm -f IDE-DevelopersGuide.pdf IDE-DevelopersGuide.html tmp1.xml tmp.xml
	rm -rf eclipse
	rm -rf ../../IDE-DevelopersGuide


