#PERL=$(TOOLS_DIR)\perl\bin\perl.exe
PERL=perl

INKSCAPE=inkscape

# --- generating pdf ---

TEXSRC = ch-ana-sim.tex ch-introduction.tex ch-parallel-exec.tex ch-sim-lib.tex \
         ch-build-sim-progs.tex ch-ned-grammar.tex ch-neddoc.tex \
         ch-opp-design.tex ch-simple-modules.tex title.tex \
         ch-ned-lang.tex ch-overview.tex ch-run-sim.tex history.tex usman.tex

PICS = figures/*

# an imperfect attempt to filter out less important warnings...
# LOGFILTER = | sed '/Underfull/{N;N;d;};/Overfull/{N;N;d;};/Token not allowed/{N;N;d;}'
LOGFILTER =

#default: usman.dvi usman.pdf
default: usman.pdf
	cp usman.pdf ../../manual

usman.dvi: $(TEXSRC) $(PICS)
	latex usman.tex $(LOGFILTER)
	bibtex usman usman-bibl
	makeindex usman
	latex usman.tex $(LOGFILTER)
	latex usman.tex $(LOGFILTER)

usman.pdf: $(TEXSRC) $(PDFPICS)
	pdflatex usman.tex $(LOGFILTER)
	bibtex usman
	makeindex usman
	pdflatex usman.tex $(LOGFILTER)
	pdflatex usman.tex $(LOGFILTER)

# --- generating html ---

HTM_FILES = history.htm \
            ch-introduction.htm \
            ch-overview.htm \
            ch-ned-lang.htm \
            ch-simple-modules.htm \
            ch-messages.htm \
            ch-sim-lib.htm \
            ch-build-sim-progs.htm \
            ch-run-sim.htm \
            ch-graphics.htm \
            ch-ana-sim.htm \
            ch-neddoc.htm \
            ch-parallel-exec.htm \
            ch-opp-design.htm \
            ch-ned-grammar.htm \
            usman-bibl.htm

.SUFFIXES : .tex .bbl .htm

#usman-bibl.bbl : usman.bbl
#	copy usman.bbl usman-bibl.bbl

.bbl.htm:
	cp $< `echo $< | sed 's|\.bbl|.tex|'`
	$(PERL) ltoh.pl `echo $< | sed 's|\.bbl|.tex|'`
	mv $@l $@
	rm  `echo $< | sed 's|\.bbl|.tex|'`

.tex.htm:
	$(PERL) ltoh.pl $<

usman.html : $(HTM_FILES) omnest.thtml omnetpp.thtml prephtml2 ltoh.specs ltoh.pl
	cat $(HTM_FILES) > usman.tmp
	$(PERL) prephtml2 usman.tmp
	rm usman.tmp
	cp usman.html .../../manual
	cp *.gif ../../manual
	cp figures/*.gif ../../manual

clean:
	rm -f usman.dvi usman.pdf *.aux *.idx *.ilg *.ind *.log *.out *.toc *.bbl *.blg *.htm usman.tmp usman.html


