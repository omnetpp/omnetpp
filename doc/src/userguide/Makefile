FOP=./fop/fop

all: html pdf

html:  
	xsltproc --xinclude --output tmp.xml trans.xsl userguide.xml
	xsltproc --output userguide.html  /usr/share/xml/docbook/stylesheet/nwalsh/html/docbook.xsl tmp.xml
	rm tmp.xml

pdf:   
	xsltproc --xinclude --output tmp.xml trans.xsl userguide.xml
	perl -pe "s!\@DOCBOOK_STYLESHEET\@!file:////usr/share/xml/docbook/stylesheet/nwalsh/fo/docbook.xsl!g" custom-fo-docbook.xsl >custom-fo-docbook-tmp.xsl
	xsltproc  --output tmp.fo custom-fo-docbook-tmp.xsl tmp.xml
	$(FOP) -fo tmp.fo -pdf userguide.pdf
	rm tmp.fo tmp.xml
	cp userguide.pdf ../..

