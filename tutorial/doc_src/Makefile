#
# Makefile to produce pdf & html from the tex sources
#

TARGET = tutorial

all: pdf

pdf: dvi
	dvipdf $(TARGET).dvi
	mv $(TARGET).pdf ..

dvi:
	latex $(TARGET).tex
	latex $(TARGET).tex

html:
	latex2html -split 1 $(TARGET).tex
	mv $(TARGET) $(TARGET).html
	mv $(TARGET).html ..

clean:
	- rm -f $(TARGET).aux
	- rm -f $(TARGET).dvi
	- rm -f $(TARGET).log
	- rm -f $(TARGET).toc
