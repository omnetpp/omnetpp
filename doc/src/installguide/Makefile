.PHONY: pdf html


all: pdf html

pdf:
	(cd ..; make do-guides-pdf)
	ln -s  ../../InstallGuide.pdf .

html:
	(cd ..; make do-guides-html)
	ln -s ../../html/installguide/index.html InstallGuilde.html

clean:
	(cd ..; make clean)
	rm -f InstallGuide.pdf InstallGuide.html
