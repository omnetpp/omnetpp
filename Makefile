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

allmodes:
	$(MAKE) MODE=release
	$(MAKE) MODE=debug
	@echo
	@echo "Now you can type \"`echo $(OMNETPP_RELEASE) | sed 's/-.*//'`\" to start the IDE"

components: base samples

#=====================================================================
#
# Includes and basic checks
#
#=====================================================================

include Makefile.inc

# Make sure that output locations are set
ifeq ("$(strip $(OMNETPP_BIN_DIR))","")
$(error OMNETPP_BIN_DIR must be correctly set)
endif
ifeq ("$(strip $(OMNETPP_OUT_DIR))","")
$(error OMNETPP_OUT_DIR must be correctly set)
endif
ifeq ("$(strip $(OMNETPP_LIB_DIR))","")
$(error OMNETPP_LIB_DIR must be correctly set)
endif
#=====================================================================
#
# OMNeT++ components
#
#=====================================================================

BASE=common layout eventlog scave nedxml sim envir cmdenv tkenv utils
SAMPLES=aloha cqn dyna embedding embedding2 fifo google-earth hypercube histograms neddemo queueinglib queueinglibext routing tictoc sockets
JNILIBS=org.omnetpp.ned.model org.omnetpp.ide.nativelibs

# add systemc optionally
ifeq "$(SYSTEMC)" "yes"
ifneq "$(wildcard src/systemc)" ""
BASE+= systemc
SAMPLES+= systemc-embedding
systemc: sim
endif
endif

#
# Group targets.
#
base: $(BASE)
	cd $(OMNETPP_SRC_DIR)/envir && $(MAKE) opp_run_executable

samples: $(SAMPLES)

opplibs:
	$(MAKE) $(MOPTS) $(MAKEFILE) BUILDING_UILIBS=yes ui

ui: check-ui-vars common layout eventlog scave nedxml $(JNILIBS)

# dependencies (because of ver.h, opp_msgc [nedtool], etc)
clean depend: makefiles
common layout eventlog scave nedxml sim envir cmdenv tkenv systemc makefiles: utils
layout eventlog scave nedxml sim envir cmdenv tkenv : common
envir : sim
cmdenv tkenv : envir
tkenv : layout
sim : nedxml common
$(SAMPLES) : makefiles base
$(BASE) : check-env
queueinglibext : queueinglib

.PHONY: check-env cleanall depend makefiles clean apis docu tests all allmodes \
        components base ui samples common layout eventlog scave nedxml sim \
        envir cmdenv tkenv utils systemc

#
# Core libraries and programs
#
$(BASE):
	@echo ===== Compiling $@ ====
	$(Q)cd $(OMNETPP_SRC_DIR)/$@ && $(MAKE)

#
# Native libs for the UI
#
$(JNILIBS): nedxml
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

check-ui-vars:
	@if [ "$(BUILDING_UILIBS)" != "yes" ]; then echo 'ERROR: "make ui" must be invoked with BUILDING_UILIBS=yes!' && exit 1; fi

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

clean: makefiles
	$(Q)-rm -f $(OMNETPP_LIB_DIR)/*.*
	$(Q)-rm -rf $(OMNETPP_OUT_DIR)/$(CONFIGNAME)
	$(Q)-rm -rf $(OMNETPP_LIB_DIR)/$(CONFIGNAME)
	$(Q)for i in $(BASE); do \
	    (cd $(OMNETPP_SRC_DIR)/$$i && $(MAKE) clean); \
	done
	$(Q)for i in $(SAMPLES) ""; do \
	    if [ "$$i" != "" ]; then (cd $(OMNETPP_SAMPLES_DIR)/$$i && $(MAKE) clean); fi;\
	done
	$(Q)cd $(OMNETPP_TEST_DIR) && $(MAKE) clean
	$(Q)-rm -f $(OMNETPP_BIN_DIR)/*

cleanall: makefiles
	$(Q)-rm -rf $(OMNETPP_OUT_DIR)
	$(Q)-rm -rf $(OMNETPP_LIB_DIR)/*
	$(Q)for i in $(BASE); do \
	    (cd $(OMNETPP_SRC_DIR)/$$i && $(MAKE) clean); \
	done
	$(Q)for i in $(SAMPLES) ""; do \
	    if [ "$$i" != "" ]; then (cd $(OMNETPP_SAMPLES_DIR)/$$i && $(MAKE) cleanall); fi;\
	done
	$(Q)cd $(OMNETPP_TEST_DIR) && $(MAKE) clean
# bin should be removed last because opp_configfilepath (in bin directory) is needed to clean
	-rm -rf $(OMNETPP_BIN_DIR)/*

cleanui:
	for i in $(JNILIBS); do \
	    (cd $(OMNETPP_UI_DIR)/$$i && $(MAKE) clean); \
	done

depend:
	$(Q)for i in $(BASE); do \
	    (cd $(OMNETPP_SRC_DIR)/$$i && $(MAKE) depend); \
	done
	$(Q)for i in $(SAMPLES) ""; do \
	    if [ "$$i" != "" ]; then (cd $(OMNETPP_SAMPLES_DIR)/$$i && $(MAKE) depend); fi;\
	done

makefiles:
	$(Q)for i in $(SAMPLES) ""; do \
	    if [ "$$i" != "" ]; then (cd $(OMNETPP_SAMPLES_DIR)/$$i && (opp_makemake -f --deep -DAUTOIMPORT_NAMESPACE)); fi;\
	done
	$(Q)(cd $(OMNETPP_SAMPLES_DIR)/embedding && (opp_makemake -f --deep --nolink -DAUTOIMPORT_NAMESPACE))
	$(Q)(cd $(OMNETPP_SAMPLES_DIR)/embedding2 && (opp_makemake -f --deep --nolink -DAUTOIMPORT_NAMESPACE))
	$(Q)(cd $(OMNETPP_SAMPLES_DIR)/google-earth && (opp_makemake -f -o google-earth-demo -DAUTOIMPORT_NAMESPACE))
	$(Q)(cd $(OMNETPP_SAMPLES_DIR)/queueinglib && (opp_makemake -f --make-so -DAUTOIMPORT_NAMESPACE))
	$(Q)(cd $(OMNETPP_SAMPLES_DIR)/queueinglibext && (opp_makemake -f --make-so -I../queueinglib -L../queueinglib/out/$(CONFIGNAME) -lqueueinglib -KQUEUEINGLIB_PROJ=../queueinglib -DAUTOIMPORT_NAMESPACE))
	$(Q)(cd $(OMNETPP_SAMPLES_DIR)/queuenet && (opp_makemake -f -n -DAUTOIMPORT_NAMESPACE))

# copy the documentation to the UI doc folder too.
# patch some files to have correct URLs and add generic eclipse stylesheet when needed
copy-ui-docu:
	rm -rf $(OMNETPP_UI_DIR)/org.omnetpp.doc/content/api
	rm -rf $(OMNETPP_UI_DIR)/org.omnetpp.doc/content/manual
	rm -rf $(OMNETPP_UI_DIR)/org.omnetpp.doc/content/userguide
	rm -rf $(OMNETPP_UI_DIR)/org.omnetpp.doc/content/migration
	rm -rf $(OMNETPP_UI_DIR)/org.omnetpp.doc/content/tictoc-tutorial
	rm -rf $(OMNETPP_UI_DIR)/org.omnetpp.doc/content/ide-overview
	rm -rf $(OMNETPP_UI_DIR)/org.omnetpp.doc/content/ide-customization-guide
	rm -rf $(OMNETPP_UI_DIR)/org.omnetpp.doc/content/ide-developersguide
	cp -r $(OMNETPP_DOC_DIR)/ide-changelog $(OMNETPP_UI_DIR)/org.omnetpp.doc/content
	cp -r $(OMNETPP_DOC_DIR)/api $(OMNETPP_UI_DIR)/org.omnetpp.doc/content
	cp -r $(OMNETPP_DOC_DIR)/src/manual/eclipse $(OMNETPP_UI_DIR)/org.omnetpp.doc/content/manual
	cp -r $(OMNETPP_DOC_DIR)/src/userguide/eclipse $(OMNETPP_UI_DIR)/org.omnetpp.doc/content/userguide
	cp -r $(OMNETPP_DOC_DIR)/src/migrationguide/eclipse $(OMNETPP_UI_DIR)/org.omnetpp.doc/content/migration
	cp -r $(OMNETPP_DOC_DIR)/tictoc-tutorial $(OMNETPP_UI_DIR)/org.omnetpp.doc/content
	cp -r $(OMNETPP_DOC_DIR)/ide-overview $(OMNETPP_UI_DIR)/org.omnetpp.doc/content
	cp -r $(OMNETPP_DOC_DIR)/src/ide-customization-guide/eclipse $(OMNETPP_UI_DIR)/org.omnetpp.doc/content/ide-customization-guide
	cp -r $(OMNETPP_DOC_DIR)/src/ide-developersguide/eclipse $(OMNETPP_UI_DIR)/org.omnetpp.doc/content/ide-developersguide
	echo "<html><body><pre>" >$(OMNETPP_UI_DIR)/org.omnetpp.doc/content/License.html
	cat $(OMNETPP_DOC_DIR)/License >>$(OMNETPP_UI_DIR)/org.omnetpp.doc/content/License.html
	echo "</pre></body></html>" >>$(OMNETPP_UI_DIR)/org.omnetpp.doc/content/License.html
	rm -rf $(OMNETPP_UI_DIR)/org.omnetpp.doc/content/userguide/plugin.xml
	rm -rf $(OMNETPP_UI_DIR)/org.omnetpp.doc/content/migration/plugin.xml
	rm -rf $(OMNETPP_UI_DIR)/org.omnetpp.doc/content/ide-customization-guide/plugin.xml
	rm -rf $(OMNETPP_UI_DIR)/org.omnetpp.doc/content/ide-developersguide/plugin.xml
	perl -i -pe 's!<head>!<head><link rel="STYLESHEET" href="book.css"  type="text/css"/>!gi' $(OMNETPP_UI_DIR)/org.omnetpp.doc/content/WhatsNew.html
	perl -i -pe 's!<head>!<head><link rel="STYLESHEET" href="../book.css"  type="text/css"/>!gi' $(OMNETPP_UI_DIR)/org.omnetpp.doc/content/ide-overview/*.html
	perl -i -pe 's!<head>!<head><link rel="STYLESHEET" href="../book.css"  type="text/css"/>!gi' $(OMNETPP_UI_DIR)/org.omnetpp.doc/content/ide-changelog/*.html
	perl -i -pe 's!href="!href="content/manual/!gi' $(OMNETPP_UI_DIR)/org.omnetpp.doc/content/manual/toc.xml
	perl -i -pe 's!href="!href="content/userguide/!gi' $(OMNETPP_UI_DIR)/org.omnetpp.doc/content/userguide/toc.xml
	perl -i -pe 's!<head>!<head><link rel="STYLESHEET" href="../book.css"  type="text/css"/>!gi' $(OMNETPP_UI_DIR)/org.omnetpp.doc/content/userguide/*.html
	perl -i -pe 's!href="!href="content/migration/!gi' $(OMNETPP_UI_DIR)/org.omnetpp.doc/content/migration/toc.xml
	perl -i -pe 's!<head>!<head><link rel="STYLESHEET" href="../book.css"  type="text/css"/>!gi' $(OMNETPP_UI_DIR)/org.omnetpp.doc/content/migration/*.html
	perl -i -pe 's!href="!href="content/ide-customization-guide/!gi' $(OMNETPP_UI_DIR)/org.omnetpp.doc/content/ide-customization-guide/toc.xml
	perl -i -pe 's!<head>!<head><link rel="STYLESHEET" href="../book.css"  type="text/css"/>!gi' $(OMNETPP_UI_DIR)/org.omnetpp.doc/content/ide-customization-guide/*.html
	perl -i -pe 's!href="!href="content/ide-developersguide/!gi' $(OMNETPP_UI_DIR)/org.omnetpp.doc/content/ide-developersguide/toc.xml
	perl -i -pe 's!<head>!<head><link rel="STYLESHEET" href="../book.css"  type="text/css"/>!gi' $(OMNETPP_UI_DIR)/org.omnetpp.doc/content/ide-developersguide/*.html

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
