#
# variables set by `configure':
#

CONFIGFILE = ../../Makefile.inc
include $(CONFIGFILE)

#
# Local definitions
#
O=$(OMNETPP_OUT_DIR)/$(TOOLCHAIN_NAME)-$(MODE)/src/utils
L=$(OMNETPP_LIB_DIR)

INCL_FLAGS= -I"$(OMNETPP_INCL_DIR)" -I"$(OMNETPP_SRC_DIR)"

COPTS=$(CFLAGS) $(INCL_FLAGS)

.PHONY : all clean
#
# Rules
#
all: $(CONFIGFILE)
	$(Q)echo "#!/bin/sh" >opp_configfilepath && echo 'echo `dirname $$0`/../Makefile.inc' >>opp_configfilepath
	$(Q)if [ "$$OS" = "Windows_NT" ] ; then \
	  echo "@echo $(OMNETPP_ROOT)/Makefile.inc" >opp_configfilepath.cmd && \
	  cp opp_configfilepath.cmd $(OMNETPP_BIN_DIR); \
	  cp opp_makemake.cmd $(OMNETPP_BIN_DIR); \
	  cp opp_runall.cmd $(OMNETPP_BIN_DIR); \
	  cp opp_test.cmd $(OMNETPP_BIN_DIR); \
	  cp omnetpp.cmd $(OMNETPP_BIN_DIR); \
	  cp omnest.cmd $(OMNETPP_BIN_DIR); \
	fi
	$(Q)cp opp_configfilepath $(OMNETPP_BIN_DIR)
	$(Q)cp opp_makemake $(OMNETPP_BIN_DIR)
	$(Q)cp opp_featuretool $(OMNETPP_BIN_DIR)
	$(Q)cp opp_test $(OMNETPP_BIN_DIR)
	$(Q)cp opp_fingerprinttest $(OMNETPP_BIN_DIR)
	$(Q)cp opp_shlib_postprocess $(OMNETPP_BIN_DIR)
	$(Q)cp opp_runall $(OMNETPP_BIN_DIR)
	$(Q)cp splitvec $(OMNETPP_BIN_DIR)
	$(Q)cp omnetpp $(OMNETPP_BIN_DIR)
	$(Q)cp omnest $(OMNETPP_BIN_DIR)
	$(Q)chmod +x $(OMNETPP_BIN_DIR)/opp_makemake $(OMNETPP_BIN_DIR)/opp_test $(OMNETPP_BIN_DIR)/opp_fingerprinttest $(OMNETPP_BIN_DIR)/splitvec $(OMNETPP_BIN_DIR)/opp_configfilepath $(OMNETPP_BIN_DIR)/opp_shlib_postprocess $(OMNETPP_BIN_DIR)/opp_runall $(OMNETPP_BIN_DIR)/omnetpp $(OMNETPP_BIN_DIR)/omnest
	@# next line is for samples/rundemo
	$(Q)echo $(WISH) >$(HOME)/.wishname

clean:
	$(qecho) Cleaning utils
	$(Q)rm -rf $O
	$(Q)cd $(OMNETPP_BIN_DIR) && rm -f opp_makemake opp_runall opp_test opp_fingerprinttest splitvec

-include $(OBJS:%.o=%.d)
