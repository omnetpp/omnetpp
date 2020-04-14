PERL=perl
INKSCAPE=inkscape
DOT=dot
TEMPLATE=omnetpp
VERSION=6.x

ALLTEX = $(wildcard *.tex)
DOTPICS = $(wildcard figures/*.dot)
SVGPICS = $(wildcard figures/*.svg) $(DOTPICS:.dot=.svg)
PDFPICS = $(SVGPICS:.svg=.pdf) $(DOTPICS:.dot=.pdf)

.PHONY : pdf html eclipse

default: pdf html eclipse

include ../cover/cover.makefrag

pdf: $(ALLTEX) $(PDFPICS) figures/*.png cover
	pdflatex usman.tex
	bibtex usman
	makeindex usman
	pdflatex usman.tex
	pdflatex usman.tex
	qpdf usman.pdf --pages cover.pdf ../cover/copyright.pdf usman.pdf 3-z -- ../../SimulationManual.pdf
	rm -f *.aux *.idx *.ilg *.ind *.log *.out *.toc *.blg

%.pdf: %.svg
	$(INKSCAPE) $< -A $@

%.svg: %.dot
	$(DOT) -Tsvg $< >$@

# --- generating html ---

.SUFFIXES : .bbl .svg .dot

#usman-bibl.bbl : usman.bbl
#       copy usman.bbl usman-bibl.bbl

%.png: %.svg
	$(INKSCAPE) $< -e $@

%.png: %.dot
	$(DOT) -Tpng $< >$@

.bbl.htm:
	cp $< `echo $< | sed 's|\.bbl|.tex|'`
	$(PERL) ltoh `echo $< | sed 's|\.bbl|.tex|'`
	mv $@l $@
	rm  `echo $< | sed 's|\.bbl|.tex|'`

eclipse : $(ALLTEX) $(SVGPICS) eclipse.thtml prephtml ltoh ltoh.specs
	CHAPTERS=$$(grep '\include{' usman.tex | sed 's/\\include{//;s/}/.tex/' | sed '/^title\.tex/d') && cat $$CHAPTERS > tmp.tex
	$(PERL) ltoh tmp.tex
	$(PERL) prephtml --split --template eclipse.thtml --productversion $(VERSION) --outfilenamebase usman tmp.html
	rm tmp.html
	rm -rf eclipse
	mkdir eclipse
	mv *.html toc.xml eclipse
	xmllint --noout eclipse/toc.xml
	cp *.png eclipse
	cp figures/*.svg eclipse
	cp figures/*.png eclipse

html : $(ALLTEX) $(SVGPICS) $(TEMPLATE).thtml prephtml ltoh ltoh.specs
	CHAPTERS=$$(grep '\include{' usman.tex | sed 's/\\include{//;s/}/.tex/' | sed '/^title\.tex/d') && cat $$CHAPTERS > tmp.tex
	$(PERL) ltoh tmp.tex
	$(PERL) prephtml --template $(TEMPLATE).thtml --productversion $(VERSION) --outfilenamebase usman tmp.html
	rm tmp.html
	rm -rf ../../manual
	mkdir ../../manual
	cp usman.html ../../manual/index.html
	cp *.png ../../manual
	cp figures/*.svg ../../manual
	cp figures/*.png ../../manual
	@if grep 'Unknown LaTeX command' usman.html; then echo 'Error: LaTeX commands missing from ltoh.specs! (see lines above)'; exit 1; fi

clean:
	rm -f usman.dvi usman.pdf *.aux *.idx *.ilg *.ind *.log *.out *.toc *.bbl *.blg *.htm usman.tmp usman.html cover.svg cover.pdf
	rm -rf ../../manual
	rm -rf ../../SimulationManual.pdf
	rm -rf eclipse
	rm -f figures/*.pdf
	rm -f `ls figures/*.svg | sed 's/svg$$/png/'`
	rm -f `ls figures/*.dot | sed 's/dot$$/svg/'`

