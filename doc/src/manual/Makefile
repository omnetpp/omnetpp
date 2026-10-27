LATEX=pdflatex -halt-on-error -file-line-error -interaction=nonstopmode >pdflatex.log
PERL=perl
RSVGCONVERT=rsvg-convert
DOT=dot
TEMPLATE=omnetpp
VERSION=6.x

ALLTEX = $(wildcard *.tex)
CHAPTERS_TEX = $(wildcard ch-*.tex) $(wildcard appendix-*.tex)
CHAPTERS_RST := $(CHAPTERS_TEX:.tex=.rst)
DOTPICS = $(wildcard figures/*.dot)
SVGPICS = $(wildcard figures/*.svg) $(DOTPICS:.dot=.svg)
PDFPICS = $(SVGPICS:.svg=.pdf) $(DOTPICS:.dot=.pdf)

.PHONY : pdf html eclipse clean rst figures

# disabling all implicit rules
.SUFFIXES :

default: pdf html eclipse

pdf: $(ALLTEX) $(PDFPICS) figures/*.png
	$(LATEX) -jobname=usman "\def\oppversion{$(VERSION)}\input{usman.tex}"
	bibtex usman
	makeindex usman
	$(LATEX) -jobname=usman "\def\oppversion{$(VERSION)}\input{usman.tex}"
	$(LATEX) -jobname=usman "\def\oppversion{$(VERSION)}\input{usman.tex}"
	cp usman.pdf ../../SimulationManual.pdf
	rm -f *.aux *.idx *.ilg *.ind *.log *.out *.toc *.blg

%.rst: %.tex
	tools/tex2rst $<

%.rst: %.trst
	@cp $< $@

%.pdf: %.svg
	$(RSVGCONVERT) $< -f pdf -o $@

%.svg: %.dot
	$(DOT) -Tsvg $< >$@
	python3 tools/update-svg-viewBox-after-dot.py $@ 2.0

%.png: %.svg
	$(RSVGCONVERT) $< -f png -o $@

%.png: %.dot
	$(DOT) -Tpng $< >$@

# --- generating html ---

#usman-bibl.bbl : usman.bbl
#       copy usman.bbl usman-bibl.bbl

.bbl.htm:
	cp $< $(echo $< | sed 's|\.bbl|.tex|')
	$(PERL) ltoh $(echo $< | sed 's|\.bbl|.tex|')
	mv $@l $@
	rm  $(echo $< | sed 's|\.bbl|.tex|')

rst: $(CHAPTERS_RST)

# Generate the SVGs derived from figures/*.dot (these are gitignored and must be
# built; the hand-drawn figures/*.svg are committed). Used by the Sphinx site.
figures: $(DOTPICS:.dot=.svg)

eclipse : $(ALLTEX) $(SVGPICS) eclipse.thtml prephtml ltoh ltoh.specs
	CHAPTERS=$$(grep '\include{' usman.tex | sed 's/\\include{//;s/}/.tex/' | sed '/^title\.tex/d') && cat $$CHAPTERS > tmp.tex
	$(PERL) ltoh tmp.tex
	$(PERL) prephtml --split --template eclipse.thtml --productversion $(VERSION) --outfilenamebase usman tmp.html
	rm tmp.html tmp.tex
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
	rm tmp.html tmp.tex
	rm -rf ../../manual
	mkdir ../../manual
	cp usman.html ../../manual/index.html
	cp *.png ../../manual
	cp figures/*.svg ../../manual
	cp figures/*.png ../../manual
	@if grep 'Unknown LaTeX command' usman.html; then echo 'Error: LaTeX commands missing from ltoh.specs! (see lines above)'; exit 1; fi

clean:
	@rm -f usman.dvi usman.pdf *.aux *.idx *.ilg *.ind *.log *.out *.toc *.bbl *.blg *.htm toc.xml tmp.tex usman.tmp usman.html usman.fdb_latexmk usman.fls
	@rm -rf ../../manual
	@rm -rf ../../SimulationManual.pdf
	@rm -rf eclipse
	@rm -f figures/*.pdf
	@rm -f $$(ls figures/*.svg | sed 's/svg$$/png/')
	@rm -f $$(ls figures/*.dot | sed 's/dot$$/svg/')
	@rm -f $(CHAPTERS_RST)
