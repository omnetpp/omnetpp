#=====================================================================
#
# Toplevel makefile for OMNeT++ libraries and programs
#
#=====================================================================

ifeq ("$(MODE)","")
all: allmodes
endif

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
# Main targets
#
#=====================================================================

all: check-env components

allmodes:
	$(MAKE) MODE=debug
	$(MAKE) MODE=release

components: base samples


#=====================================================================
#
# OMNeT++ components
#
#=====================================================================

BASE=common layout eventlog scave nedxml sim envir cmdenv tkenv utils
SAMPLES=aloha cqn dyna fifo hypercube histograms neddemo queueinglib queuenet routing tictoc tokenring sockets
JNILIBS=org.omnetpp.ned.model org.omnetpp.ide.nativelibs

#
# Group targets.
#
base: $(BASE)
ui : common layout eventlog scave nedxml $(JNILIBS)
samples: $(SAMPLES)

# dependencies (because of ver.h, tcl2c, opp_msgc, etc)
clean depend: makefiles
common layout eventlog scave nedxml sim envir cmdenv tkenv makefiles: utils
layout eventlog scave nedxml sim envir cmdenv : common
sim : nedxml
$(SAMPLES) : makefiles base

# create some additional dependecies required to build windows DLLs
ifeq ($(LIB_SUFFIX),$(DLL_LIB_SUFFIX))
sim : nedxml envir-implib
envir : sim
tkenv cmdenv : envir
tkenv : layout
endif

#
# Core libraries and programs
#
$(BASE):
	@echo ===== Compiling $@ ====
	cd $(OMNETPP_SRC_DIR)/$@ && $(MAKE)

envir-implib:
	@echo ===== Compiling $@ ====
	cd $(OMNETPP_SRC_DIR)/envir && $(MAKE) $(MOPTS) $(MAKEFILE) envir-implib

#
# Native libs for the UI
#
$(JNILIBS):
	@echo ===== Compiling $@ ====
	cd $(OMNETPP_UI_DIR)/$@ && $(MAKE) clean
	cd $(OMNETPP_UI_DIR)/$@ && $(MAKE)

#
# Sample programs
#

$(SAMPLES):
	@echo ===== Compiling $@ ====
	cd $(OMNETPP_SAMPLES_DIR)/$@ && $(MAKE)

#
# Documentation
#
apis:
	cd $(OMNETPP_DOC_DIR)/src && $(MAKE) apis

docu:
	cd $(OMNETPP_DOC_DIR)/src && $(MAKE)

#
# Test
#
tests: check-env base
	cd $(OMNETPP_TEST_DIR) && $(MAKE)

#=====================================================================
#
# Utilities
#
#=====================================================================

check-env:
	@echo "***** Configuration: MODE=$(MODE), TOOLCHAIN_NAME=$(TOOLCHAIN_NAME), LIB_SUFFIX=$(LIB_SUFFIX) ****"
	@echo ===== Checking environment =====
	@probefile=__probe__; \
	if (echo '#!/bin/sh' >$(OMNETPP_BIN_DIR)/$$probefile && \
	    chmod +x $(OMNETPP_BIN_DIR)/$$probefile) 2>/dev/null; then \
	  if $$probefile >/dev/null 2>/dev/null; then :; else \
	    echo '  *** Warning: $(OMNETPP_BIN_DIR) is not in the path, some components may not build!'; \
	  fi; \
	else \
	  echo '  *** Warning: Cannot write to $(OMNETPP_BIN_DIR), does it exist?'; \
	fi; \
	rm -f $(OMNETPP_BIN_DIR)/$$probefile; \
	if uname | grep "CYGWIN" >/dev/null; then :; else \
	  if echo $$LD_LIBRARY_PATH | grep "$(OMNETPP_LIB_DIR)" >/dev/null; then :; else \
	    echo '  *** Warning: Looks like $(OMNETPP_LIB_DIR) is not in LD_LIBRARY_PATH, shared libs may not work!'; \
	  fi; \
	fi

clean:
	-rm -f $(OMNETPP_LIB_DIR)/*.*
	-rm -rf $(OMNETPP_OUT_DIR)/$(CONFIGNAME)
	-rm -rf $(OMNETPP_LIB_DIR)/$(CONFIGNAME)
	for i in $(BASE); do \
	    (cd $(OMNETPP_SRC_DIR)/$$i && $(MAKE) clean); \
	done
	for i in $(SAMPLES) ""; do \
	    if [ "$$i" != "" ]; then (cd $(OMNETPP_SAMPLES_DIR)/$$i && $(MAKE) clean); fi;\
	done
	cd $(OMNETPP_TEST_DIR) && $(MAKE) clean
# bin should be removed last because opp_configfilepath (in bin directory) is needed to clean
	-rm -f $(OMNETPP_BIN_DIR)/*

cleanall: clean
	-rm -rf $(OMNETPP_BIN_DIR)/*
	-rm -rf $(OMNETPP_OUT_DIR)
	-rm -rf $(OMNETPP_LIB_DIR)/*

depend:
	for i in $(BASE); do \
	    (cd $(OMNETPP_SRC_DIR)/$$i && $(MAKE) depend); \
	done
	for i in $(SAMPLES) ""; do \
	    if [ "$$i" != "" ]; then (cd $(OMNETPP_SAMPLES_DIR)/$$i && $(MAKE) depend); fi;\
	done

makefiles:
	for i in $(SAMPLES) ""; do \
	    if [ "$$i" != "" ]; then (cd $(OMNETPP_SAMPLES_DIR)/$$i && (opp_makemake -f --deep)); fi;\
	done
	(cd $(OMNETPP_SAMPLES_DIR)/queuenet && (opp_makemake -f -n))

