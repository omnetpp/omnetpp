FOP=fop
DOCBOOK=/usr/share/xml/docbook/stylesheet/nwalsh
export XML_CATALOG_FILES=../docbook-dtd/catalog.xml

all: pdf html eclipse

html:
	perl trans.pl <ide-customization-guide.xml >tmp0.xml
	xsltproc --nonet --xinclude --output tmp.xml trans.xsl tmp0.xml
	xsltproc --nonet --output ide-customization-guide.html  $(DOCBOOK)/html/docbook.xsl tmp.xml
	rm -f tmp.xml tmp0.xml
	rm -rf ../../ide-customization-guide
	mkdir ../../ide-customization-guide
	mkdir ../../ide-customization-guide/images
	cp ide-customization-guide.html ../../ide-customization-guide
	cp images/* ../../ide-customization-guide/images

.PHONY : eclipse
eclipse:
	perl trans.pl <ide-customization-guide.xml >tmp0.xml
	xsltproc --nonet --xinclude --output tmp.xml trans.xsl tmp0.xml
	rm -rf eclipse
	mkdir eclipse
	mkdir eclipse/images
	cd eclipse && xsltproc --nonet $(DOCBOOK)/eclipse/eclipse.xsl ../tmp.xml
	cp images/* eclipse/images
	rm tmp.xml tmp0.xml

pdf:
	perl trans.pl <ide-customization-guide.xml >tmp0.xml
	xsltproc --nonet --xinclude --output tmp.xml --stringparam output pdf trans.xsl tmp0.xml
	perl -pe "s!\@DOCBOOK_STYLESHEET\@!file:///$(DOCBOOK)/fo/docbook.xsl!g" custom-fo-docbook.xsl >custom-fo-docbook-tmp.xsl
	xsltproc --nonet --output tmp.fo custom-fo-docbook-tmp.xsl tmp.xml
	$(FOP) -fo tmp.fo -c ../fop.xconf -pdf ide-customization-guide.pdf
	rm -f tmp.fo tmp.xml tmp0.xml
	rm -f custom-fo-docbook-tmp.xsl
	pdftk C=cover.pdf E=empty.pdf D=ide-customization-guide.pdf cat C E D1 E D3-end output ../../IDE-CustomizationGuide.pdf

clean:
	rm -rf ide-customization-guide.pdf ide-customization-guide.html
	rm -rf eclipse
	rm -rf ../../ide-customization-guide

