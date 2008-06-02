#PERL=$(TOOLS_DIR)\perl\bin\perl.exe
PERL=perl

INKSCAPE=inkscape

# --- generating pdf ---

TEXCHAPTERS = \
         ch-ana-sim.tex ch-introduction.tex ch-parallel-exec.tex ch-sim-lib.tex \
         ch-build-sim-progs.tex ch-neddoc.tex \
         ch-opp-design.tex ch-simple-modules.tex \
         ch-ned-lang.tex ch-overview.tex ch-run-sim.tex \
         appendix-ned-grammar.tex appendix-config-options.tex \
         appendix-msg-grammar.tex appendix-ned-functions.tex \
         appendix-eventlog-file-format.tex appendix-result-file-formats.tex \
         appendix-display-strings.tex
ALLTEX = usman.tex title.tex $(TEXCHAPTERS)
HTMCHAPTERS = $(TEXCHAPTERS:.tex=.htm)

SVGPICS = $(wildcard figures/*.svg)
PDFPICS = $(SVGPICS:.svg=.pdf)

# an imperfect attempt to filter out less important warnings...
# LOGFILTER = | sed '/Underfull/{N;N;d;};/Overfull/{N;N;d;};/Token not allowed/{N;N;d;}'
LOGFILTER =

#default: usman.dvi usman.pdf
default: usman.pdf
	cp usman.pdf ../../manual

usman.pdf: $(ALLTEX) $(PDFPICS) figures/*.png
	pdflatex usman.tex $(LOGFILTER)
	bibtex usman
	makeindex usman
	pdflatex usman.tex $(LOGFILTER)
	pdflatex usman.tex $(LOGFILTER)

# --- generating html ---

.SUFFIXES : .tex .bbl .htm .svg

#usman-bibl.bbl : usman.bbl
#	copy usman.bbl usman-bibl.bbl

%.pdf: %.svg
	$(INKSCAPE) $< -A $@

.bbl.htm:
	cp $< `echo $< | sed 's|\.bbl|.tex|'`
	$(PERL) ltoh.pl `echo $< | sed 's|\.bbl|.tex|'`
	mv $@l $@
	rm  `echo $< | sed 's|\.bbl|.tex|'`

.tex.htm:
	$(PERL) ltoh.pl $<

usman.html : $(HTMCHAPTERS) omnest.thtml omnetpp.thtml prephtml2 ltoh.specs ltoh.pl
	cat $(HTMCHAPTERS) > usman.tmp
	$(PERL) prephtml2 usman.tmp
	rm usman.tmp
	cp usman.html .../../manual
	cp *.png ../../manual
	cp figures/*.png ../../manual

clean:
	rm -f usman.dvi usman.pdf *.aux *.idx *.ilg *.ind *.log *.out *.toc *.bbl *.blg *.htm usman.tmp usman.html

