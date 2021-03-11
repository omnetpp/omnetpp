.PHONY: pdf html


all: pdf html

pdf:
	(cd ..; make do-guides-pdf)
	ln -sf  ../../InstallGuide.pdf .

html:
	(cd ..; make do-guides-html)
	ln -sf ../../guides/installguide/index.html InstallGuide.html

clean:
	(cd ..; make clean)
	rm -f InstallGuide.pdf InstallGuide.html
