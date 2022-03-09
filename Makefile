#
# Toplevel makefile for OMNeT++ libraries and programs
#
#===============================================================================


#=====================================================================
#
# Main targets
#
#=====================================================================

ifeq ("$(MODE)","")
all: allmodes
else
all: components
endif

# run in silent mode if V=1 is not specified
ifneq ($(V),1)
MAKE += -s
endif

help:
	@echo "$$HELP_SYNOPSYS"
	@echo "$$HELP_OPP_TARGETS"
	@echo "$$HELP_VARIABLES"
	@echo "$$HELP_OPP_VARIABLES"
	@echo "$$HELP_EXAMPLES"

allmodes:
	@echo
	@echo "Building release and debug mode executables. Type 'make help' for further options."
	@echo
	$(Q)$(MAKE) MODE=release
	$(Q)$(MAKE) MODE=debug
	@echo
	@echo "Now you can type '`echo $(OMNETPP_RELEASE) | sed 's/-.*//'`' to start the IDE."

components: base

# Test if the ide directory exists and create application shortcuts in the omnetpp root dir and add them to the global app menu
ifneq ($(wildcard ide),)
all: install-shortcuts
cleanall: delete-shortcuts uninstall-shortcuts
endif

# Test if the samples directory exists and add dependencies to build it, too
ifneq ($(wildcard samples),)
components: samples
clean: clean-samples
cleanall: cleanall-samples
endif

# Test if the test directory exists and add dependencies to build it, too
ifneq ($(wildcard test),)
clean: clean-tests
cleanall: cleanall-tests
endif

#=====================================================================
#
# Includes and basic checks
#
#=====================================================================

include Makefile.inc

#=====================================================================
#
# OMNeT++ components
#
#=====================================================================

BASE=common layout eventlog scave nedxml sim envir cmdenv utils
SAMPLES=aloha canvas cqn dyna embedding embedding2 fifo hypercube histograms neddemo queueinglib queueinglibext routing tictoc sockets openstreetmap petrinets osg-intro osg-earth osg-indoor osg-satellites wiredphy
JNILIBS=org.omnetpp.ned.model org.omnetpp.ide.nativelibs

ifeq "$(WITH_QTENV)" "yes"
  BASE+= qtenv
endif

# add systemc optionally
ifeq "$(WITH_SYSTEMC)" "yes"
ifneq "$(wildcard src/systemc)" ""
BASE+= systemc
SAMPLES+= systemc-embedding
systemc: sim
endif
endif

.PHONY: check-env cleanall cleanall-samples makefiles clean apis doc tests all allmodes \
        components base ui uilibs samples common layout eventlog scave nedxml sim \
        envir cmdenv qtenv utils systemc help install uninstall \
		create-shortcuts delete-shortcuts install-shortcuts uninstall-shortcuts
#
# Group targets.
#
base: $(BASE)
ifeq "$(SHARED_LIBS)" "yes"
	@echo ===== Compiling opp_run ====
	$(Q)cd $(OMNETPP_SRC_DIR)/envir && $(MAKE) opp_run_executable
endif

samples: $(SAMPLES)

ui:
	$(Q)$(MAKE) $(MOPTS) $(MAKEFILE) BUILDING_UILIBS=yes uilibs

uilibs: $(JNILIBS)

ide:
	@echo ===== Building IDE =====
	$(Q)-rm releng/dist/*
	$(Q)releng/build-omnetpp-ide-linux
	$(Q)tar xfz releng/dist/*-omnetpp-*-linux-x86_64.tgz $$(cat Version)/ide --strip-components=1

# dependencies (because of ver.h, opp_msgtool, etc)
common layout eventlog scave nedxml sim envir cmdenv qtenv systemc makefiles: utils
layout eventlog scave nedxml sim envir cmdenv qtenv: common
envir : sim
cmdenv qtenv: envir
qtenv: layout
sim : nedxml common
$(SAMPLES) : makefiles base
$(BASE) : check-env
queueinglibext : queueinglib

#
# Core libraries and programs
#
$(BASE):
	@echo ===== Compiling $@ ====
	$(Q)cd $(OMNETPP_SRC_DIR)/$@ && $(MAKE)

#
# Native libs for the UI
#
$(JNILIBS): nedxml scave layout eventlog common
	@echo ===== Compiling $@ ====
	$(Q)cd $(OMNETPP_UI_DIR)/$@ && $(MAKE) clean
	$(Q)cd $(OMNETPP_UI_DIR)/$@ && $(MAKE)

#
# Sample programs
#

$(SAMPLES):
	@echo ===== Compiling $@ ====
	$(Q)cd $(OMNETPP_SAMPLES_DIR)/$@ && $(MAKE)


#
# Documentation
#
apis:
	@echo ===== Building $@ ====
	$(Q)cd $(OMNETPP_DOC_DIR)/src && $(MAKE) apis -j1

doc:
	@echo ===== Building $@ ====
	$(Q)cd $(OMNETPP_DOC_DIR)/src && $(MAKE) -j1

#
# Test
#
tests: base
	@echo ===== Running $@ ====
	$(Q)cd $(OMNETPP_TEST_DIR) && $(MAKE)

#=====================================================================
#
# Utilities
#
#=====================================================================

check-env:
	@echo "***** Configuration: MODE=$(MODE), TOOLCHAIN_NAME=$(TOOLCHAIN_NAME), SHARED_LIBS=$(SHARED_LIBS), LIB_SUFFIX=$(LIB_SUFFIX) ****"
	@echo ===== Checking environment =====
	@mkdir -p $(OMNETPP_BIN_DIR)
	@probefile=__probe__; \
	if (echo '#!/bin/sh' >$(OMNETPP_BIN_DIR)/$$probefile && \
	    chmod +x $(OMNETPP_BIN_DIR)/$$probefile) 2>/dev/null; then \
	  if $$probefile >/dev/null 2>/dev/null; then :; else \
	    echo 'ERROR: $(OMNETPP_BIN_DIR) is not in the path! You can add it by entering:'; \
	    echo '   export PATH=$(OMNETPP_BIN_DIR):$$PATH'; \
	    exit 1; \
	  fi; \
	else \
	  echo 'ERROR: Cannot write to $(OMNETPP_BIN_DIR)! Please make sure it exists, and has write permission.'; \
	  exit 1; \
	fi; \
	rm -f $(OMNETPP_BIN_DIR)/$$probefile; \

clean:
	$(Q)-rm -f $(OMNETPP_LIB_DIR)/*.*
	$(Q)-rm -rf $(OMNETPP_OUT_DIR)/$(CONFIGNAME)
	$(Q)-rm -rf $(OMNETPP_LIB_DIR)/$(CONFIGNAME)
	$(Q)for i in $(BASE); do \
	    (cd $(OMNETPP_SRC_DIR)/$$i && $(MAKE) clean); \
	done
	$(Q)-rm -f $(OMNETPP_BIN_DIR)/*

clean-tests: makefiles
	$(Q)cd $(OMNETPP_TEST_DIR) && $(MAKE) clean

clean-samples: makefiles
	$(Q)for i in $(SAMPLES) ""; do \
	    if [ "$$i" != "" ]; then (cd $(OMNETPP_SAMPLES_DIR)/$$i && $(MAKE) clean); fi;\
	done

cleanall:
	$(Q)-rm -rf $(OMNETPP_OUT_DIR)
	$(Q)-rm -rf $(OMNETPP_LIB_DIR)/*
	$(Q)for i in $(BASE); do \
	    (cd $(OMNETPP_SRC_DIR)/$$i && $(MAKE) clean); \
	done
# bin should be removed last because opp_configfilepath (in bin directory) is needed to clean
	$(Q)-rm -rf $(OMNETPP_BIN_DIR)/*

cleanall-tests: makefiles
	$(Q)cd $(OMNETPP_TEST_DIR) && $(MAKE) cleanall

cleanall-samples: makefiles
	$(Q)for i in $(SAMPLES) ""; do \
	    if [ "$$i" != "" ]; then (cd $(OMNETPP_SAMPLES_DIR)/$$i && $(MAKE) cleanall && rm Makefile); fi;\
	done

cleanui:
	$(Q)for i in $(JNILIBS); do \
	    (cd $(OMNETPP_UI_DIR)/$$i && $(MAKE) clean); \
	done

makefiles:
	$(Q)(cd $(OMNETPP_SAMPLES_DIR)/embedding && MAKE="$(MAKE)" opp_makemake -f --deep --nolink)
	$(Q)(cd $(OMNETPP_SAMPLES_DIR)/embedding2 && MAKE="$(MAKE)" opp_makemake -f --deep --nolink)
	$(Q)(cd $(OMNETPP_SAMPLES_DIR)/queueinglib && MAKE="$(MAKE)" opp_makemake -f --make-so -pQUEUEING -I.)
	$(Q)(cd $(OMNETPP_SAMPLES_DIR)/queueinglibext && MAKE="$(MAKE)" opp_makemake -f --make-so -I../queueinglib -L'../queueinglib' -l'queueinglib$$(D)' -KQUEUEINGLIB_PROJ=../queueinglib)
	$(Q)(cd $(OMNETPP_SAMPLES_DIR)/queuenet && MAKE="$(MAKE)" opp_makemake -f -n)
	$(Q)for i in $(filter-out embedding embedding2 queueinglib queueinglibext queuenet, $(SAMPLES)) ""; do \
	    if [ "$$i" != "" ]; then (cd $(OMNETPP_SAMPLES_DIR)/$$i && MAKE="$(MAKE)" opp_makemake -f --deep); fi;\
	done

# copy the documentation to the UI doc folder too.
# patch some files to have correct URLs and add generic eclipse stylesheet when needed
copy-ui-doc:
	$(Q)rm -rf $(OMNETPP_UI_DIR)/org.omnetpp.doc/content/api
	$(Q)rm -rf $(OMNETPP_UI_DIR)/org.omnetpp.doc/content/manual
	$(Q)rm -rf $(OMNETPP_UI_DIR)/org.omnetpp.doc/content/guides
	$(Q)cp -r $(OMNETPP_DOC_DIR)/visual-changelog $(OMNETPP_UI_DIR)/org.omnetpp.doc/content
	$(Q)cp -r $(OMNETPP_DOC_DIR)/api $(OMNETPP_UI_DIR)/org.omnetpp.doc/content
	$(Q)cp -r $(OMNETPP_DOC_DIR)/src/manual/eclipse $(OMNETPP_UI_DIR)/org.omnetpp.doc/content/manual
	$(Q)cp -r $(OMNETPP_ROOT)/out/doc/eclipsehelp $(OMNETPP_UI_DIR)/org.omnetpp.doc/content/guides
	$(Q)echo "<html><body><pre>" >$(OMNETPP_UI_DIR)/org.omnetpp.doc/content/License.html
	$(Q)cat $(OMNETPP_DOC_DIR)/License >>$(OMNETPP_UI_DIR)/org.omnetpp.doc/content/License.html
	$(Q)echo "</pre></body></html>" >>$(OMNETPP_UI_DIR)/org.omnetpp.doc/content/License.html
	$(Q)perl -i -pe 's!<head>!<head><link rel="STYLESHEET" href="book.css"  type="text/css"/>!gi' $(OMNETPP_UI_DIR)/org.omnetpp.doc/content/WhatsNew.html
	$(Q)perl -i -pe 's!href="!href="content/manual/!gi' $(OMNETPP_UI_DIR)/org.omnetpp.doc/content/manual/toc.xml

create-shortcuts:
ifeq ($(PLATFORM),linux)
	@echo "[Desktop Entry]\nEncoding=UTF-8\nType=Application\nExec=$(OMNETPP_BIN_DIR)/opp_ide\nIcon=$(OMNETPP_ROOT)/images/logo/logo128.png\nName=$(OMNETPP_PRODUCT) $(OMNETPP_VERSION) IDE\nCategories=Development\n" >$(OMNETPP_RELEASE)-ide.desktop && chmod +x $(OMNETPP_RELEASE)-ide.desktop
	@echo "[Desktop Entry]\nEncoding=UTF-8\nType=Application\nExec=/bin/bash --rcfile $(OMNETPP_ROOT)/setenv\nTerminal=true\nIcon=$(OMNETPP_ROOT)/images/logo/logo128s.png\nName=$(OMNETPP_PRODUCT) $(OMNETPP_VERSION) Shell\nCategories=Development\n" >$(OMNETPP_RELEASE)-shell.desktop && chmod +x $(OMNETPP_RELEASE)-shell.desktop
else ifeq ($(PLATFORM),win32)
	@-shortcut.cmd -linkfile "$(OMNETPP_ROOT)/$(OMNETPP_PRODUCT) $(OMNETPP_VERSION) IDE.lnk" -target "$(OMNETPP_ROOT)/mingwenv.cmd" -linkarguments ide -iconlocation "$(OMNETPP_ROOT)/images/logo/logo128.ico" -workingdirectory "$(OMNETPP_ROOT)" -windowstyle 7
	@-shortcut.cmd -linkfile "$(OMNETPP_ROOT)/$(OMNETPP_PRODUCT) $(OMNETPP_VERSION) Shell.lnk" -target "$(OMNETPP_ROOT)/mingwenv.cmd" -iconlocation "$(OMNETPP_ROOT)/images/logo/logo128s.ico" -workingdirectory "$(OMNETPP_ROOT)" -windowstyle 7
endif

delete-shortcuts:
ifeq ($(PLATFORM),linux)
	@-rm -f $(OMNETPP_RELEASE)-ide.desktop
	@-rm -f $(OMNETPP_RELEASE)-shell.desktop
else ifeq ($(PLATFORM),win32)
	@-rm -f "$(OMNETPP_ROOT)/$(OMNETPP_PRODUCT) $(OMNETPP_VERSION) IDE.lnk"
	@-rm -f "$(OMNETPP_ROOT)/$(OMNETPP_PRODUCT) $(OMNETPP_VERSION) Shell.lnk"
endif

install-shortcuts: create-shortcuts
ifeq ($(PLATFORM),linux)
	@xdg-desktop-menu uninstall $(OMNETPP_RELEASE)-ide.desktop
	@xdg-desktop-menu install $(OMNETPP_RELEASE)-ide.desktop
	@xdg-desktop-menu uninstall $(OMNETPP_RELEASE)-shell.desktop
	@xdg-desktop-menu install $(OMNETPP_RELEASE)-shell.desktop
else ifeq ($(PLATFORM),win32)
	@cp "$(OMNETPP_ROOT)/$(OMNETPP_PRODUCT) $(OMNETPP_VERSION) IDE.lnk" "$(APPDATA)/Microsoft/Windows/Start Menu/Programs"
	@cp "$(OMNETPP_ROOT)/$(OMNETPP_PRODUCT) $(OMNETPP_VERSION) Shell.lnk" "$(APPDATA)/Microsoft/Windows/Start Menu/Programs"
endif

uninstall-shortcuts:
ifeq ($(PLATFORM),linux)
	@xdg-desktop-menu uninstall $(OMNETPP_RELEASE)-ide.desktop
	@xdg-desktop-menu uninstall $(OMNETPP_RELEASE)-shell.desktop
else ifeq ($(PLATFORM),win32)
	@-rm -f "$(APPDATA)/Microsoft/Windows/Start Menu/Programs/$(OMNETPP_PRODUCT) $(OMNETPP_VERSION) IDE.lnk"
	@-rm -f "$(APPDATA)/Microsoft/Windows/Start Menu/Programs/$(OMNETPP_PRODUCT) $(OMNETPP_VERSION) Shell.lnk"
endif
