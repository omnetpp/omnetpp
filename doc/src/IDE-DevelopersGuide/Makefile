FOP=fop
DOCBOOK=/usr/share/xml/docbook/stylesheet/nwalsh
export XML_CATALOG_FILES=../docbook-dtd/catalog.xml

all: pdf html eclipse

asciidoc:
	asciidoc -b docbook -d book IDE-DevelopersGuide.txt >IDE-DevelopersGuide.xml

html:
	perl trans.pl <IDE-DevelopersGuide.xml >tmp0.xml
	xsltproc --nonet --xinclude --output tmp.xml trans.xsl tmp0.xml
	xsltproc --nonet --output IDE-DevelopersGuide.html  $(DOCBOOK)/html/docbook.xsl tmp.xml
	rm -f tmp.xml tmp0.xml
	rm -rf ../../IDE-DevelopersGuide
	mkdir ../../IDE-DevelopersGuide
	mkdir ../../IDE-DevelopersGuide/images
	cp IDE-DevelopersGuide.html ../../IDE-DevelopersGuide
	cp images/* ../../IDE-DevelopersGuide/images

.PHONY : eclipse
eclipse:
	perl trans.pl <IDE-DevelopersGuide.xml >tmp0.xml
	xsltproc --nonet --xinclude --output tmp.xml trans.xsl tmp0.xml
	rm -rf eclipse
	mkdir eclipse
	mkdir eclipse/images
	cd eclipse && xsltproc $(DOCBOOK)/eclipse/eclipse.xsl ../tmp.xml
	cp images/* eclipse/images
	rm tmp.xml tmp0.xml

pdf: asciidoc
	perl trans.pl <IDE-DevelopersGuide.xml >tmp0.xml
	xsltproc --nonet --xinclude --output tmp.xml --stringparam output pdf trans.xsl tmp0.xml
	perl -pe "s!\@DOCBOOK_STYLESHEET\@!file:///$(DOCBOOK)/fo/docbook.xsl!g" custom-fo-docbook.xsl >custom-fo-docbook-tmp.xsl
	xsltproc  --nonet --output tmp.fo custom-fo-docbook-tmp.xsl tmp.xml
	$(FOP) -fo tmp.fo -c ../fop.xconf -pdf IDE-DevelopersGuide.pdf
	rm -f tmp.fo tmp.xml tmp0.xml
	rm -f custom-fo-docbook-tmp.xsl
	cp IDE-DevelopersGuide.pdf ../..

clean:
	rm -rf IDE-DevelopersGuide.pdf IDE-DevelopersGuide.html
	rm -rf eclipse
	rm -rf ../../IDE-DevelopersGuide

