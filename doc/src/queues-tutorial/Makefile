# Makefile to translate tutorial document to ps, pdf.

SOURCE = tutorial

nothing:

pdf: latex
	pdflatex ./$(SOURCE).tex 

ps: latex
	dvips ./$(SOURCE).dvi -o ./$(SOURCE).ps

html: latex
	latex2html -split 1 $(SOURCE)

latex: 
	latex ./$(SOURCE).tex
	latex ./$(SOURCE).tex  # get labels right

clean: 
	rm -f $(SOURCE).aux
	rm -f $(SOURCE).dvi
	rm -f $(SOURCE).log
	rm -f $(SOURCE).toc

cleanall: clean
	rm -r $(SOURCE).pdf
	rm -r $(SOURCE).ps
	rm -r $(SOURCE)

