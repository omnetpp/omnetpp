#
# Makefile to produce pdf from the tex sources
#

all: tutorial.pdf

tutorial.pdf: tutorial.tex tutorial.bbl
	latex tutorial.tex
	dvipdf tutorial.dvi
	cp tutorial.pdf ..