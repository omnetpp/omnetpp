#
# Toplevel makefile for OMNeT++ libraries and programs
#
# Use MODE=release or MODE=debug on the command line to build only
# release or debug mode binaries.
# Use V=1 to print all commands executed by the makefile.
#
#=====================================================================

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

allmodes:
	$(MAKE) MODE=release
	$(MAKE) MODE=debug
	@echo
	@echo "Now you can type \"`echo $(OMNETPP_RELEASE) | sed 's/-.*//'`\" to start the IDE"

components: base

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
SAMPLES=aloha canvas cqn dyna embedding embedding2 fifo hypercube histograms neddemo queueinglib queueinglibext routing tictoc sockets osg-intro osg-earth osg-indoor osg-satellites
JNILIBS=org.omnetpp.ned.model org.omnetpp.ide.nativelibs

ifeq "$(WITH_TKENV)" "yes"
  BASE+= tkenv
endif

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

.PHONY: check-env cleanall cleanall-samples makefiles clean apis docu tests all allmodes \
        components base ui uilibs samples common layout eventlog scave nedxml sim \
        envir cmdenv tkenv qtenv utils systemc
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

# dependencies (because of ver.h, opp_msgtool, etc)
common layout eventlog scave nedxml sim envir cmdenv tkenv qtenv systemc makefiles: utils
layout eventlog scave nedxml sim envir cmdenv tkenv qtenv: common
envir : sim
cmdenv tkenv qtenv: envir
tkenv qtenv: layout
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
	$(Q)cd $(OMNETPP_DOC_DIR)/src && $(MAKE) apis

docu:
	@echo ===== Building $@ ====
	$(Q)cd $(OMNETPP_DOC_DIR)/src && $(MAKE)

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
	@echo "***** Configuration: MODE=$(MODE), TOOLCHAIN_NAME=$(TOOLCHAIN_NAME), LIB_SUFFIX=$(LIB_SUFFIX) ****"
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
	$(Q)cd $(OMNETPP_TEST_DIR) && $(MAKE) clean

cleanall-samples: makefiles
	$(Q)for i in $(SAMPLES) ""; do \
	    if [ "$$i" != "" ]; then (cd $(OMNETPP_SAMPLES_DIR)/$$i && $(MAKE) cleanall && rm Makefile); fi;\
	done

cleanui:
	$(Q)for i in $(JNILIBS); do \
	    (cd $(OMNETPP_UI_DIR)/$$i && $(MAKE) clean); \
	done

makefiles:
	$(Q)(cd $(OMNETPP_SAMPLES_DIR)/embedding && (test -f Makefile || (MAKE="$(MAKE)" opp_makemake -f --deep --nolink)))
	$(Q)(cd $(OMNETPP_SAMPLES_DIR)/embedding2 && (test -f Makefile || (MAKE="$(MAKE)" opp_makemake -f --deep --nolink)))
	$(Q)(cd $(OMNETPP_SAMPLES_DIR)/queueinglib && (test -f Makefile || (MAKE="$(MAKE)" opp_makemake -f --make-so -pQUEUEING -I.)))
	$(Q)(cd $(OMNETPP_SAMPLES_DIR)/queueinglibext && (test -f Makefile || (MAKE="$(MAKE)" opp_makemake -f --make-so -I../queueinglib -L'../queueinglib' -l'queueinglib$$(D)' -KQUEUEINGLIB_PROJ=../queueinglib)))
	$(Q)(cd $(OMNETPP_SAMPLES_DIR)/queuenet && (test -f Makefile || (MAKE="$(MAKE)" opp_makemake -f -n)))
	$(Q)for i in $(SAMPLES) ""; do \
	    if [ "$$i" != "" ]; then (cd $(OMNETPP_SAMPLES_DIR)/$$i && (test -f Makefile || (MAKE="$(MAKE)" opp_makemake -f --deep))); fi;\
	done

# copy the documentation to the UI doc folder too.
# patch some files to have correct URLs and add generic eclipse stylesheet when needed
copy-ui-docu:
	$(Q)rm -rf $(OMNETPP_UI_DIR)/org.omnetpp.doc/content/api
	$(Q)rm -rf $(OMNETPP_UI_DIR)/org.omnetpp.doc/content/manual
	$(Q)rm -rf $(OMNETPP_UI_DIR)/org.omnetpp.doc/content/userguide
	$(Q)rm -rf $(OMNETPP_UI_DIR)/org.omnetpp.doc/content/ide-overview
	$(Q)rm -rf $(OMNETPP_UI_DIR)/org.omnetpp.doc/content/ide-customization-guide
	$(Q)rm -rf $(OMNETPP_UI_DIR)/org.omnetpp.doc/content/ide-developersguide
	$(Q)cp -r $(OMNETPP_DOC_DIR)/visual-changelog $(OMNETPP_UI_DIR)/org.omnetpp.doc/content
	$(Q)cp -r $(OMNETPP_DOC_DIR)/api $(OMNETPP_UI_DIR)/org.omnetpp.doc/content
	$(Q)cp -r $(OMNETPP_DOC_DIR)/src/manual/eclipse $(OMNETPP_UI_DIR)/org.omnetpp.doc/content/manual
	$(Q)cp -r $(OMNETPP_DOC_DIR)/src/userguide/eclipse $(OMNETPP_UI_DIR)/org.omnetpp.doc/content/userguide
	$(Q)cp -r $(OMNETPP_DOC_DIR)/ide-overview $(OMNETPP_UI_DIR)/org.omnetpp.doc/content
	$(Q)cp -r $(OMNETPP_DOC_DIR)/src/ide-customization-guide/eclipse $(OMNETPP_UI_DIR)/org.omnetpp.doc/content/ide-customization-guide
	$(Q)cp -r $(OMNETPP_DOC_DIR)/src/ide-developersguide/eclipse $(OMNETPP_UI_DIR)/org.omnetpp.doc/content/ide-developersguide
	$(Q)echo "<html><body><pre>" >$(OMNETPP_UI_DIR)/org.omnetpp.doc/content/License.html
	$(Q)cat $(OMNETPP_DOC_DIR)/License >>$(OMNETPP_UI_DIR)/org.omnetpp.doc/content/License.html
	$(Q)echo "</pre></body></html>" >>$(OMNETPP_UI_DIR)/org.omnetpp.doc/content/License.html
	$(Q)rm -rf $(OMNETPP_UI_DIR)/org.omnetpp.doc/content/userguide/plugin.xml
	$(Q)rm -rf $(OMNETPP_UI_DIR)/org.omnetpp.doc/content/ide-customization-guide/plugin.xml
	$(Q)rm -rf $(OMNETPP_UI_DIR)/org.omnetpp.doc/content/ide-developersguide/plugin.xml
	$(Q)perl -i -pe 's!<head>!<head><link rel="STYLESHEET" href="book.css"  type="text/css"/>!gi' $(OMNETPP_UI_DIR)/org.omnetpp.doc/content/WhatsNew.html
	$(Q)perl -i -pe 's!<head>!<head><link rel="STYLESHEET" href="../book.css"  type="text/css"/>!gi' $(OMNETPP_UI_DIR)/org.omnetpp.doc/content/ide-overview/*.html
	$(Q)perl -i -pe 's!<head>!<head><link rel="STYLESHEET" href="../book.css"  type="text/css"/>!gi' $(OMNETPP_UI_DIR)/org.omnetpp.doc/content/ide-changelog/*.html
	$(Q)perl -i -pe 's!href="!href="content/manual/!gi' $(OMNETPP_UI_DIR)/org.omnetpp.doc/content/manual/toc.xml
	$(Q)perl -i -pe 's!href="!href="content/userguide/!gi' $(OMNETPP_UI_DIR)/org.omnetpp.doc/content/userguide/toc.xml
	$(Q)perl -i -pe 's!<head>!<head><link rel="STYLESHEET" href="../book.css"  type="text/css"/>!gi' $(OMNETPP_UI_DIR)/org.omnetpp.doc/content/userguide/*.html
	$(Q)perl -i -pe 's!href="!href="content/ide-customization-guide/!gi' $(OMNETPP_UI_DIR)/org.omnetpp.doc/content/ide-customization-guide/toc.xml
	$(Q)perl -i -pe 's!<head>!<head><link rel="STYLESHEET" href="../book.css"  type="text/css"/>!gi' $(OMNETPP_UI_DIR)/org.omnetpp.doc/content/ide-customization-guide/*.html
	$(Q)perl -i -pe 's!href="!href="content/ide-developersguide/!gi' $(OMNETPP_UI_DIR)/org.omnetpp.doc/content/ide-developersguide/toc.xml
	$(Q)perl -i -pe 's!<head>!<head><link rel="STYLESHEET" href="../book.css"  type="text/css"/>!gi' $(OMNETPP_UI_DIR)/org.omnetpp.doc/content/ide-developersguide/*.html

ifeq ($(findstring linux,$(PLATFORM)),linux)

generate-desktop-file:
	@echo "[Desktop Entry]\nEncoding=UTF-8\nType=Application\nExec=$(OMNETPP_BIN_DIR)/omnetpp\nIcon=$(OMNETPP_ROOT)/ide/icon.png\nName=$(OMNETPP_PRODUCT) $(OMNETPP_VERSION) IDE\nCategories=Development;IDE;Debugger\n" >opensim-ide.desktop

install-menu-item: generate-desktop-file
	@xdg-desktop-menu uninstall opensim-ide.desktop
	@xdg-desktop-menu install opensim-ide.desktop
	@rm opensim-ide.desktop

install-desktop-icon: generate-desktop-file
	@xdg-desktop-icon uninstall opensim-ide.desktop
	@xdg-desktop-icon install opensim-ide.desktop
	@rm opensim-ide.desktop

else ifeq ($(findstring macosx,$(PLATFORM)),macosx)

install-menu-item:
ifeq ($(OMNETPP_PRODUCT),OMNEST)
	-ln -s -f $(OMNETPP_ROOT)/ide/omnest.app /Applications/'$(OMNETPP_PRODUCT) $(OMNETPP_VERSION) IDE'
else
	-ln -s -f $(OMNETPP_ROOT)/ide/omnetpp.app /Applications/'$(OMNETPP_PRODUCT) $(OMNETPP_VERSION) IDE'
endif

install-desktop-icon:
ifeq ($(OMNETPP_PRODUCT),OMNEST)
	-ln -s -f $(OMNETPP_ROOT)/ide/omnest.app ~/Desktop/'$(OMNETPP_PRODUCT) $(OMNETPP_VERSION) IDE'
else
	-ln -s -f $(OMNETPP_ROOT)/ide/omnetpp.app ~/Desktop/'$(OMNETPP_PRODUCT) $(OMNETPP_VERSION) IDE'
endif

else ifeq ($(findstring win32,$(PLATFORM)),win32)

endif
