
TEXSRC = ch-ana-sim.tex ch-introduction.tex ch-opnet.tex ch-parallel-exec.tex ch-sim-lib.tex references.tex ch-build-sim-progs.tex ch-ned-grammar.tex ch-opp-design.tex ch-parsec.tex ch-simple-modules.tex title.tex ch-example-nim.tex ch-ned-lang.tex ch-overview.tex ch-run-sim.tex history.tex usman.tex

PSPICS = figures/*.eps
PDFPICS = figures/*.pdf

default: usman.dvi usman.pdf

usman.dvi: $(TEXSRC) $(PSPICS)
	latex usman.tex
	makeindex usman
	latex usman.tex
	latex usman.tex

usman.pdf: $(TEXSRC) $(PDFPICS)
	pdflatex usman.tex
	makeindex usman
	pdflatex usman.tex
	pdflatex usman.tex

clean:
	rm -f usman.dvi usman.pdf *.aux *.idx *.ilg *.ind *.log *.out *.toc