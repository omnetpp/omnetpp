DOXYGEN=doxygen
SPHINXBUILD   ?= sphinx-build
SOURCEDIR     = .
BUILDDIR      = out
LATEXDIR      = out-latex
GUIDES_PDF    = UserGuide.pdf InstallGuide.pdf IDE-Overview.pdf IDE-DevelopersGuide.pdf IDE-CustomizationGuide.pdf
SPHINXOPTS    ?= -q

# by default disable latex output to reduce noise.
# remove these options to get the latex error messages if latex stops with an error
LATEXOPTS     ?= --halt-on-error -interaction=batchmode

# Documentation identity, consumed by conf.py via the environment. Overridable on
# the command line (e.g. make ci-html VERSION=6.x BUILDID=abc123 what=omnetpp).
# BASEURL overrides the canonical base URL; empty means /<VERSION>/ (see conf.py).
what          ?= omnetpp
VERSION       ?= internal
BUILDID       ?= deadbeef
BASEURL       ?=
export what VERSION BUILDID BASEURL

# The toolchain is the doc-builder image's: run the targets with build.sh, which runs them in it.
ifneq ($(MAKECMDGOALS),clean)
ifeq ($(shell command -v $(SPHINXBUILD)),)
$(error $(SPHINXBUILD) not found: the documentation is built in the doc-builder image, run the target with ./build.sh <target>)
endif
endif

.PHONY: all clean site site-api apis api nedxml-api parsim-api do-manual do-manual-html do-manual-pdf do-manual-eclipse do-guides do-guides-html do-guides-eclipsehelp do-guides-pdf ci-html ci-pdf ci-audit

#
# Targets
#
all: | apis do-manual do-guides

apis: | api nedxml-api parsim-api

site-server: site-api
	@sphinx-autobuild --host 0.0.0.0 --port 8000 -b html "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)

site: site-api
	@$(SPHINXBUILD) -b html "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)

site-api:
	$(DOXYGEN) doxy/site.doxyfile
	@(cd api && rm -f pages.html files.html *-members.html functions*) # we do not need a separate all members list page
	@doxysphinx build --doxygen_exe $(DOXYGEN) . "$(BUILDDIR)" api
	@(cd api && sed -i 's/:orphan://g' *.rst && rm -f index.rst graph_legend.rst annotated.rst files.rst annotated_classes.rst topics.rst dir_*.rst *-members.rst deprecated.rst)
	$(DOXYGEN) doxy/nedxml-site.doxyfile
	@(cd nedxml-api && rm -f pages.html files.html *-members.html functions*)
	@doxysphinx build --doxygen_exe $(DOXYGEN) . "$(BUILDDIR)" nedxml-api
	@(cd nedxml-api && sed -i 's/:orphan://g' *.rst && rm -f index.rst graph_legend.rst annotated.rst files.rst annotated_classes.rst topics.rst dir_*.rst *-members.rst deprecated.rst)
	$(DOXYGEN) doxy/parsim-site.doxyfile
	@(cd parsim-api && rm -f pages.html files.html *-members.html functions*)
	@doxysphinx build --doxygen_exe $(DOXYGEN) . "$(BUILDDIR)" parsim-api
	@(cd parsim-api && sed -i 's/:orphan://g' *.rst && rm -f index.rst graph_legend.rst annotated.rst files.rst annotated_classes.rst topics.rst dir_*.rst *-members.rst deprecated.rst)

site-manual:

# --- CI: one-shot unified HTML build --------------------------------------------
# Generates the manual RST from LaTeX and its figures, gates on conversion
# quality, builds the doxygen APIs, then the unified Sphinx HTML site into out/.
# This is the single entry point used by site/deploy/build-version.sh.
ci-html:
	$(MAKE) -C manual rst figures
	$(MAKE) ci-audit
	$(MAKE) site
	$(MAKE) ci-pdf

# The PDFs that the pages link to: the manual's comes from its LaTeX sources, the guides' from Sphinx.
ci-pdf:
	$(MAKE) -C manual pdf VERSION=$(VERSION)
	$(SPHINXBUILD) -b latex -d "$(BUILDDIR)/.doctrees" "$(SOURCEDIR)" "$(LATEXDIR)" $(SPHINXOPTS) $(O)
	$(MAKE) -C "$(LATEXDIR)" $(GUIDES_PDF) LATEXOPTS="$(LATEXOPTS)"
	mkdir -p "$(BUILDDIR)/pdf"
	cp ../SimulationManual.pdf $(addprefix $(LATEXDIR)/,$(GUIDES_PDF)) "$(BUILDDIR)/pdf"

# Fail the build on HARD TeX->RST conversion defects (leftover raw TeX, undefined
# roles, unmapped code environments). Soft cross-reference warnings are left to
# the nitpicky Sphinx build, not this gate.
ci-audit:
	@mkdir -p $(BUILDDIR)
	python3 site/check-conversion.py --report $(BUILDDIR)/conversion-report.md manual


api:
	$(DOXYGEN) doxy/api.doxyfile

nedxml-api:
	$(DOXYGEN) doxy/nedxml.doxyfile

parsim-api:
	$(DOXYGEN) doxy/parsim.doxyfile

do-manual: | do-manual-pdf do-manual-html do-manual-eclipse

do-manual-pdf:
	cd manual && $(MAKE) pdf

do-manual-html:
	cd manual && $(MAKE) html

do-manual-eclipse:
	cd manual && $(MAKE) eclipse

do-guides: | do-guides-html do-guides-pdf do-guides-eclipse

do-guides-html:
	@$(SPHINXBUILD) -M html "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)
	cp -R $(BUILDDIR)/html ../guides

do-guides-eclipse:
	@$(SPHINXBUILD) -M eclipsehelp "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)

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

