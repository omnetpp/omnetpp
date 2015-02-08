#
# variables set by `configure':
#

include ../../Makefile.inc

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

#
# Local definitions
#
O=$(OMNETPP_OUT_DIR)/$(OUTPUT_PREFIX)$(TOOLCHAIN_NAME)-$(MODE)/src/utils
L=$(OMNETPP_LIB_DIR)/$(OUTPUT_PREFIX)$(TOOLCHAIN_NAME)

#
# Rules
#
all: $O/opp_lcg32_seedtool$(EXE_SUFFIX) $O/abspath$(EXE_SUFFIX)
	$(qecho) Copying scripts to bin directory...
	$(Q)echo "#!/bin/sh" >opp_configfilepath && echo 'echo `dirname $$0`/../Makefile.inc' >>opp_configfilepath
	$(Q)if [ "$$OS" = "Windows_NT" ] ; then \
	  echo "@echo $(OMNETPP_ROOT)/Makefile.inc" >opp_configfilepath.cmd && \
	  cp opp_configfilepath.cmd $(OMNETPP_BIN_DIR); \
	  cp opp_makemake.cmd $(OMNETPP_BIN_DIR); \
	  cp opp_runall.cmd $(OMNETPP_BIN_DIR); \
	  cp opp_test.cmd $(OMNETPP_BIN_DIR); \
	  cp opp_makedep.cmd $(OMNETPP_BIN_DIR); \
	  cp omnetpp.cmd $(OMNETPP_BIN_DIR); \
	  cp omnest.cmd $(OMNETPP_BIN_DIR); \
	fi
	$(Q)cp opp_configfilepath $(OMNETPP_BIN_DIR)
	$(Q)cp opp_makemake $(OMNETPP_BIN_DIR)
	$(Q)cp opp_test $(OMNETPP_BIN_DIR)
	$(Q)cp opp_makedep $(OMNETPP_BIN_DIR)
	$(Q)cp opp_shlib_postprocess $(OMNETPP_BIN_DIR)
	$(Q)cp opp_runall $(OMNETPP_BIN_DIR)
	$(Q)cp splitvec $(OMNETPP_BIN_DIR)
	$(Q)cp omnetpp $(OMNETPP_BIN_DIR)
	$(Q)cp omnest $(OMNETPP_BIN_DIR)
	$(Q)chmod +x $(OMNETPP_BIN_DIR)/opp_makemake $(OMNETPP_BIN_DIR)/opp_test $(OMNETPP_BIN_DIR)/opp_makedep $(OMNETPP_BIN_DIR)/splitvec $(OMNETPP_BIN_DIR)/opp_configfilepath $(OMNETPP_BIN_DIR)/opp_shlib_postprocess $(OMNETPP_BIN_DIR)/opp_runall $(OMNETPP_BIN_DIR)/omnetpp $(OMNETPP_BIN_DIR)/omnest
	$(Q)cd $O && $(OMNETPP_ROOT)/src/utils/install-prog opp_lcg32_seedtool$(EXE_SUFFIX) $(OMNETPP_BIN_DIR)
	$(Q)cd $O && $(OMNETPP_ROOT)/src/utils/install-prog abspath$(EXE_SUFFIX) $(OMNETPP_BIN_DIR)
	@# next line is for samples/rundemo
	$(Q)echo $(WISH) >$(HOME)/.wishname

$O/opp_lcg32_seedtool$(EXE_SUFFIX) : opp_lcg32_seedtool.cc
	@mkdir -p $O
	@echo Creating executable: $@
	$(Q)$(CXX) $(CXXFLAGS) opp_lcg32_seedtool.cc -o $O/opp_lcg32_seedtool$(EXE_SUFFIX)

$O/abspath$(EXE_SUFFIX) : abspath.cc
	@mkdir -p $O
	@echo Creating executable: $@
	$(Q)$(CXX) $(CXXFLAGS) abspath.cc -o $O/abspath$(EXE_SUFFIX)

depend:
	$(qecho) Creating dependencies...
	$(Q)$(MAKEDEPEND) -I. -P\$$O/ -- *.cc *.c

clean:
	$(qecho) Cleaning...
	$(Q)rm -rf $O
	$(Q)cd $(OMNETPP_BIN_DIR) && rm -f opp_lcg32_seedtool$(EXE_SUFFIX) abspath$(EXE_SUFFIX) opp_makemake opp_runall opp_test opp_makedep splitvec
#	$(Q)rm -f opp_configfilepath
#	$(Q)rm -f opp_configfilepath.cmd

# DO NOT DELETE THIS LINE -- make depend depends on it.
$O/abspath.o: abspath.cc \
  ../common/ver.h
$O/opp_lcg32_seedtool.o: opp_lcg32_seedtool.cc \
  ../common/ver.h
$O/opp_pefileversion.o: opp_pefileversion.cc
$O/ln.o: ln.c
$O/opp_replace.o: opp_replace.c
