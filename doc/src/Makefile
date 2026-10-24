DOXYGEN=doxygen
VENV          = .venv/pyvenv.cfg
SPHINXBUILD   ?= sphinx-build
SOURCEDIR     = .
BUILDDIR      = out
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

# Prefix for running the Sphinx toolchain. Defaults to running it via uv in the
# project virtualenv (uv sync creates ./.venv from uv.lock). Inside the doc-builder
# container (where Sphinx is installed globally) invoke make with PIPRUN= VENVDEP=
# to bypass uv entirely.
PIPRUN        ?= uv run
VENVDEP       ?= $(VENV)

.PHONY: all clean site site-api apis api nedxml-api parsim-api do-manual do-manual-html do-manual-pdf do-manual-eclipse do-guides do-guides-html do-guides-eclipsehelp do-guides-pdf ci-html ci-audit

#
# Targets
#
all: | apis do-manual do-guides

apis: | api nedxml-api parsim-api

site-server: site-api $(VENVDEP)
	@$(PIPRUN) sphinx-autobuild --host localhost --port 8000 -b html "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)

site: site-api $(VENVDEP)
	@$(PIPRUN) $(SPHINXBUILD) -b html "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)

site-api: $(VENVDEP)
	$(DOXYGEN) doxy/site.doxyfile
	@(cd api && rm -f pages.html files.html *-members.html functions*) # we do not need a separate all members list page
	@$(PIPRUN) doxysphinx build --doxygen_exe $(DOXYGEN) . "$(BUILDDIR)" api
	@(cd api && sed -i 's/:orphan://g' *.rst && rm -f index.rst graph_legend.rst annotated.rst files.rst annotated_classes.rst topics.rst dir_*.rst *-members.rst deprecated.rst)
	$(DOXYGEN) doxy/nedxml-site.doxyfile
	@(cd nedxml-api && rm -f pages.html files.html *-members.html functions*)
	@$(PIPRUN) doxysphinx build --doxygen_exe $(DOXYGEN) . "$(BUILDDIR)" nedxml-api
	@(cd nedxml-api && sed -i 's/:orphan://g' *.rst && rm -f index.rst graph_legend.rst annotated.rst files.rst annotated_classes.rst topics.rst dir_*.rst *-members.rst deprecated.rst)
	$(DOXYGEN) doxy/parsim-site.doxyfile
	@(cd parsim-api && rm -f pages.html files.html *-members.html functions*)
	@$(PIPRUN) doxysphinx build --doxygen_exe $(DOXYGEN) . "$(BUILDDIR)" parsim-api
	@(cd parsim-api && sed -i 's/:orphan://g' *.rst && rm -f index.rst graph_legend.rst annotated.rst files.rst annotated_classes.rst topics.rst dir_*.rst *-members.rst deprecated.rst)

site-manual:

# --- CI: one-shot unified HTML build --------------------------------------------
# Generates the manual RST from LaTeX and its figures, gates on conversion
# quality, builds the doxygen APIs, then the unified Sphinx HTML site into out/.
# This is the single entry point used by site/deploy/build-version.sh.
ci-html: $(VENVDEP)
	$(MAKE) -C manual rst figures
	$(MAKE) ci-audit
	$(MAKE) site

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
	@$(PIPRUN) $(SPHINXBUILD) -M html "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)
	cp -R $(BUILDDIR)/html ../guides

do-guides-eclipse:
	@$(PIPRUN) $(SPHINXBUILD) -M eclipsehelp "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)

do-guides-pdf:
	LATEXOPTS="$(LATEXOPTS)" $(PIPRUN) $(SPHINXBUILD) -M latexpdf "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)
	cp $(BUILDDIR)/latex/UserGuide.pdf ..
	cp $(BUILDDIR)/latex/IDE-CustomizationGuide.pdf ..
	cp $(BUILDDIR)/latex/IDE-DevelopersGuide.pdf ..
	cp $(BUILDDIR)/latex/InstallGuide.pdf ..
	cp $(BUILDDIR)/latex/IDE-Overview.pdf ..

clean:
	rm -rf $(BUILDDIR) ../api ../nedxml-api ../parsim-api ../manual ../guides ../*.pdf
	cd manual && $(MAKE) clean

$(VENV): pyproject.toml uv.lock
	uv sync --frozen
