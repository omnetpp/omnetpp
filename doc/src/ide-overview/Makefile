FOP=fop
DOCBOOK=/usr/share/xml/docbook/stylesheet/nwalsh
export XML_CATALOG_FILES=../docbook-dtd/catalog.xml

all: pdf html eclipse

html:
	xsltproc --nonet --xinclude --output tmp1.xml trans.xsl ide-overview.xml
	perl trans.pl <tmp1.xml >tmp.xml
	perl -pe "s!\@DOCBOOK_STYLESHEET\@!file:///$(DOCBOOK)/html/docbook.xsl!g" custom-html-docbook.xsl >custom-html-docbook-tmp.xsl
	xsltproc --nonet --output ide-overview.html custom-html-docbook-tmp.xsl tmp.xml
	rm tmp.xml custom-html-docbook-tmp.xsl
	rm -rf ../../ide-overview
	mkdir ../../ide-overview
	mkdir ../../ide-overview/pictures
	cp ide-overview.html ../../ide-overview
	cp pictures/*.png ../../ide-overview/pictures

.PHONY : eclipse
eclipse:
	xsltproc --nonet --xinclude --output tmp1.xml trans.xsl ide-overview.xml
	perl trans.pl <tmp1.xml >tmp.xml
	perl -pe "s!\@DOCBOOK_STYLESHEET\@!file:///$(DOCBOOK)/eclipse/eclipse.xsl!g" custom-html-docbook.xsl >custom-html-docbook-tmp.xsl
	rm -rf eclipse
	mkdir eclipse
	mkdir eclipse/pictures
	cp pictures/*.png eclipse/pictures
	cd eclipse && xsltproc --nonet ../custom-html-docbook-tmp.xsl ../tmp.xml
	xmllint --noout eclipse/toc.xml
	rm tmp.xml custom-html-docbook-tmp.xsl

pdf:
	xsltproc --nonet --xinclude --stringparam output pdf --output tmp1.xml trans.xsl ide-overview.xml
	perl trans.pl <tmp1.xml >tmp.xml
	perl -pe "s!\@DOCBOOK_STYLESHEET\@!file:///$(DOCBOOK)/fo/docbook.xsl!g" custom-fo-docbook.xsl >custom-fo-docbook-tmp.xsl
	xsltproc --nonet --output tmp.fo custom-fo-docbook-tmp.xsl tmp.xml
	$(FOP) -fo tmp.fo -c fop.xconf -pdf ide-overview.pdf
	rm tmp.fo tmp.xml custom-fo-docbook-tmp.xsl
	cp ide-overview.pdf ../../IDE-Overview.pdf

clean:
	rm -f ide-overview.pdf ide-overview.html tmp1.xml tmp.xml
	rm -rf eclipse
	rm -rf ../../ide-overview
