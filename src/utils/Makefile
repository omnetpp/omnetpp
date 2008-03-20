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
O=$(OMNETPP_OUT_DIR)/$(TOOLCHAIN_NAME)-$(MODE)/src/utils
L=$(OMNETPP_LIB_DIR)/$(TOOLCHAIN_NAME)

#
# Rules
#
all: $O/seedtool$(EXE_SUFFIX)
	echo "#!/bin/sh" >opp_configfilepath && echo 'echo `dirname $$0`/../Makefile.inc' >>opp_configfilepath
#	echo "#!/bin/sh" >opp_configfilepath && echo "echo $(OMNETPP_ROOT)/Makefile.inc" >>opp_configfilepath
	chmod +x opp_makemake opp_test opp_makedep splitvec opp_configfilepath omnetpp
	if [ "$$OS" = "Windows_NT" ] ; then \
	  echo "@echo $(OMNETPP_ROOT)/Makefile.inc" >opp_configfilepath.cmd && \
	  cp opp_configfilepath.cmd $(OMNETPP_BIN_DIR); \
	fi
	cp opp_configfilepath $(OMNETPP_BIN_DIR)
	cp opp_makemake $(OMNETPP_BIN_DIR)
	cp opp_test $(OMNETPP_BIN_DIR)
	cp opp_makedep $(OMNETPP_BIN_DIR)
	cp splitvec $(OMNETPP_BIN_DIR)
	cp omnetpp $(OMNETPP_BIN_DIR)
	cd $O && $(OMNETPP_ROOT)/src/utils/install-prog seedtool$(EXE_SUFFIX) $(OMNETPP_BIN_DIR)
	@# next line is for samples/rundemo
	echo $(WISH) >$(HOME)/.wishname

$O/seedtool$(EXE_SUFFIX) : seedtool.cc
	@mkdir -p $O
	$(CXX) seedtool.cc -o $O/seedtool$(EXE_SUFFIX)

depend:
	$(MAKEDEPEND) -I. -P\$$O/ -- *.cc *.c

clean:
	rm -rf $O
	cd $(OMNETPP_BIN_DIR) && rm -f seedtool$(EXE_SUFFIX) opp_makemake opp_test opp_makedep splitvec
#	rm -f opp_configfilepath
#	rm -f opp_configfilepath.cmd

# DO NOT DELETE THIS LINE -- make depend depends on it.
$O/seedtool.o: seedtool.cc
$O/opp_replace.o: opp_replace.c
