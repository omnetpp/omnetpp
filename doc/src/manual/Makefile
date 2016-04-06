#PERL=$(TOOLS_DIR)\perl\bin\perl.exe
PERL=perl

INKSCAPE=inkscape
DOT=dot
TEMPLATE=omnetpp
VERSION=5.x

TITLE=Simulation Manual

# --- generating pdf ---

# FIXME
TEXCHAPTERS = \
	 ch-introduction.tex \
	 ch-overview.tex \
	 ch-ned-lang.tex \
	 ch-simple-modules.tex \
	 ch-messages.tex \
	 ch-message-definitions.tex \
	 ch-sim-lib.tex \
	 ch-graphics.tex \
	 ch-build-sim-progs.tex \
	 ch-config-sim.tex \
	 ch-run-sim.tex \
	 ch-ana-sim.tex \
	 ch-eventlog.tex \
	 ch-neddoc.tex \
	 ch-testing.tex \
	 ch-parallel-exec.tex \
	 ch-plugin-exts.tex \
	 ch-embedding.tex \
	 appendix-ned-ref.tex \
	 appendix-ned-grammar.tex \
	 appendix-ned-dtd.tex \
	 appendix-ned-functions.tex \
	 appendix-msg-grammar.tex \
	 appendix-display-strings.tex \
	 appendix-config-options.tex \
	 appendix-result-file-formats.tex \
	 appendix-eventlog-file-format.tex \
	 appendix-figure-definitions.tex
ALLTEX = usman.tex title.tex $(TEXCHAPTERS)
HTMCHAPTERS = $(TEXCHAPTERS:.tex=.htm)

DOTPICS = $(wildcard figures/*.dot)
SVGPICS = $(wildcard figures/*.svg)
PDFPICS = $(SVGPICS:.svg=.pdf) $(DOTPICS:.dot=.pdf)
PNGPICS = $(SVGPICS:.svg=.png) $(DOTPICS:.dot=.png)

# an imperfect attempt to filter out less important warnings...
# LOGFILTER = | sed '/Underfull/{N;N;d;};/Overfull/{N;N;d;};/Token not allowed/{N;N;d;}'
LOGFILTER =

.PHONY : pdf html eclipse

default: pdf html eclipse

include ../cover/cover.makefrag

pdf: $(ALLTEX) $(PDFPICS) figures/*.png cover
	pdflatex usman.tex $(LOGFILTER)
	bibtex usman
	makeindex usman
	pdflatex usman.tex $(LOGFILTER)
	pdflatex usman.tex $(LOGFILTER)
	pdftk usman.pdf dump_data > tmp.metadata
	perl -i -pe 's/#160;/#32;/g' tmp.metadata
	pdftk C=cover.pdf B=../cover/copyright.pdf D=usman.pdf cat C B D3-end output tmp.pdf
	pdftk tmp.pdf update_info tmp.metadata output ../../SimulationManual.pdf
	rm -f *.aux *.idx *.ilg *.ind *.log *.out *.toc *.blg tmp.metadata tmp.pdf

%.pdf: %.svg
	$(INKSCAPE) $< -A $@

%.svg: %.dot
	$(DOT) -Tsvg $< >$@

# --- generating html ---

.SUFFIXES : .tex .bbl .htm .svg .dot

#usman-bibl.bbl : usman.bbl
#       copy usman.bbl usman-bibl.bbl

%.png: %.svg
	$(INKSCAPE) $< -e $@

%.png: %.dot
	$(DOT) -Tpng $< >$@

.bbl.htm:
	cp $< `echo $< | sed 's|\.bbl|.tex|'`
	$(PERL) ltoh.pl `echo $< | sed 's|\.bbl|.tex|'`
	mv $@l $@
	rm  `echo $< | sed 's|\.bbl|.tex|'`

.tex.htm: ltoh.specs ltoh.pl
	$(PERL) ltoh.pl $<

eclipse : $(HTMCHAPTERS) $(PNGPICS) eclipse.thtml prephtml2
	cat $(HTMCHAPTERS) > usman.tmp
	$(PERL) prephtml2 --split --template eclipse.thtml --opp_version $(VERSION) usman.tmp
	rm usman.tmp
	rm -rf eclipse
	mkdir eclipse
	mv *.html toc.xml eclipse
	xmllint --noout eclipse/toc.xml
	cp *.png eclipse
	cp figures/*.png eclipse

html : $(HTMCHAPTERS) $(PNGPICS) omnest.thtml omnetpp.thtml prephtml2
	cat $(HTMCHAPTERS) > usman.tmp
	$(PERL) prephtml2 --template $(TEMPLATE).thtml --opp_version $(VERSION) usman.tmp
	rm usman.tmp
	rm -rf ../../manual
	mkdir ../../manual
	cp usman.html ../../manual/index.html
	cp *.png ../../manual
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
