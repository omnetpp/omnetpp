
TEXSRC = ch-ana-sim.tex ch-introduction.tex ch-parallel-exec.tex ch-sim-lib.tex \
         ch-build-sim-progs.tex ch-ned-grammar.tex \
         ch-opp-design.tex ch-simple-modules.tex title.tex ch-example-nim.tex \
         ch-ned-lang.tex ch-overview.tex ch-run-sim.tex history.tex usman.tex

PSPICS = figures/*.eps
PDFPICS = figures/*.pdf

# an imperfect attempt to filter out less important warnings...
# LOGFILTER = | sed '/Underfull/{N;N;d;};/Overfull/{N;N;d;};/Token not allowed/{N;N;d;}'
LOGFILTER = 

default: usman.dvi usman.pdf

usman.dvi: $(TEXSRC) $(PSPICS)
	latex usman.tex $(LOGFILTER)
	bibtex usman
	makeindex usman
	latex usman.tex $(LOGFILTER)
	latex usman.tex $(LOGFILTER)

usman.pdf: $(TEXSRC) $(PDFPICS)
	pdflatex usman.tex $(LOGFILTER)
	bibtex usman
	makeindex usman
	pdflatex usman.tex $(LOGFILTER)
	pdflatex usman.tex $(LOGFILTER)

clean:
	rm -f usman.dvi usman.pdf *.aux *.idx *.ilg *.ind *.log *.out *.toc *.bbl *.blg
