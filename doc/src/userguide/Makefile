.PHONY: pdf html


all: pdf html

pdf:
	(cd ..; make do-guides-pdf)
	ln -sf  ../../UserGuide.pdf .

html:
	(cd ..; make do-guides-html)
	ln -sf ../../guides/userguide/index.html UserGuide.html

clean:
	(cd ..; make clean)
	rm -f UserGuide.pdf UserGuide.html
