DOXYGEN=doxygen
SPHINXBUILD   ?= sphinx-build
SOURCEDIR     = .
BUILDDIR      = ../../out/doc
SPHINXOPTS    ?= -q

# by default disable latex output to reduce noise.
# remove these options to get the latex error messages if latex stops with an error
LATEXOPTS     ?= --halt-on-error -interaction=batchmode

.PHONY: all clean apis api nedxml-api parsim-api do-manual do-manual-html do-manual-pdf do-guides do-guides-html do-guides-pdf

#
# Targets
#
all: apis do-manual do-guides

apis: api nedxml-api parsim-api

api:
	$(DOXYGEN) api.doxyfile

nedxml-api:
	$(DOXYGEN) nedxml.doxyfile

parsim-api:
	$(DOXYGEN) parsim.doxyfile

do-manual: do-manual-pdf do-manual-html

do-manual-pdf:
	cd manual && $(MAKE) pdf

do-manual-html:
	cd manual && $(MAKE) html

do-guides: do-guides-html do-guides-pdf

do-guides-html:
	$(SPHINXBUILD) -M html "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)
	cp -R $(BUILDDIR)/html ../guides

do-guides-pdf:
	LATEXOPTS="$(LATEXOPTS)" $(SPHINXBUILD) -M latexpdf "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)
	cp $(BUILDDIR)/latex/UserGuide.pdf ..
	cp $(BUILDDIR)/latex/IDE-CustomizationGuide.pdf ..
	cp $(BUILDDIR)/latex/IDE-DevelopersGuide.pdf ..
	cp $(BUILDDIR)/latex/InstallGuide.pdf ..
	cp $(BUILDDIR)/latex/IDE-Overview.pdf ..

clean:
	rm -rf $(BUILDDIR) ../api ../nedxml-api ../parsim-api ../manual ../guides ../*.pdf
	cd manual && $(MAKE) clean
