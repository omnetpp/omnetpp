FOP=./fop/fop

html:  
	xsltproc --xinclude --output tmp.xml trans.xsl userguide.xml
	xsltproc --output userguide.html  /usr/share/xml/docbook/stylesheet/nwalsh/html/docbook.xsl tmp.xml

pdf:   
	xsltproc --xinclude --output tmp.xml trans.xsl userguide.xml
	xsltproc  --output tmp.fo  /usr/share/xml/docbook/stylesheet/nwalsh/fo/docbook.xsl tmp.xml
	$(FOP) -fo tmp.fo -pdf userguide.pdf

