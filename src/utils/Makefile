#
# variables set by `configure':
#

CONFIGFILE = ../../Makefile.inc
include $(CONFIGFILE)

#
# Local definitions
#
O=$(OMNETPP_OUT_DIR)/$(CONFIGNAME)/src/utils
L=$(OMNETPP_LIB_DIR)

.PHONY : all clean
#
# Rules
#
all: $(CONFIGFILE)
	$(Q)echo "#!/bin/sh" >$(OMNETPP_BIN_DIR)/opp_configfilepath && echo '(cd $$(dirname $$0)/.. && echo $$(pwd)/Makefile.inc)' >>$(OMNETPP_BIN_DIR)/opp_configfilepath
	$(Q)cp opp_makemake $(OMNETPP_BIN_DIR)
	$(Q)cp opp_featuretool $(OMNETPP_BIN_DIR)
	$(Q)cp opp_charttool $(OMNETPP_BIN_DIR)
	$(Q)cp opp_test $(OMNETPP_BIN_DIR)
	$(Q)cp opp_fingerprinttest $(OMNETPP_BIN_DIR)
	$(Q)cp opp_shlib_postprocess $(OMNETPP_BIN_DIR)
	$(Q)cp opp_runall $(OMNETPP_BIN_DIR)
	$(Q)cp opp_neddoc $(OMNETPP_BIN_DIR)
	$(Q)cp opp_ide $(OMNETPP_BIN_DIR)
	$(Q)cp opp_dbgmi $(OMNETPP_BIN_DIR)
	$(Q)cp opp_dbgdap $(OMNETPP_BIN_DIR)
	$(Q)cp opp_python_repl $(OMNETPP_BIN_DIR)
	$(Q)cp omnetpp $(OMNETPP_BIN_DIR)
	$(Q)cp omnest $(OMNETPP_BIN_DIR)
	$(Q)chmod +x $(OMNETPP_BIN_DIR)/opp_makemake $(OMNETPP_BIN_DIR)/opp_test $(OMNETPP_BIN_DIR)/opp_fingerprinttest $(OMNETPP_BIN_DIR)/opp_configfilepath $(OMNETPP_BIN_DIR)/opp_shlib_postprocess $(OMNETPP_BIN_DIR)/opp_runall $(OMNETPP_BIN_DIR)/opp_neddoc $(OMNETPP_BIN_DIR)/opp_ide $(OMNETPP_BIN_DIR)/opp_dbgdap $(OMNETPP_BIN_DIR)/opp_dbgmi $(OMNETPP_BIN_DIR)/omnetpp $(OMNETPP_BIN_DIR)/omnest

clean:
	$(qecho) Cleaning utils
	$(Q)rm -rf $O
	$(Q)cd $(OMNETPP_BIN_DIR) && rm -f omnest omnetpp opp_ide opp_dbgmi opp_dbgdap opp_configfilepath opp_charttool opp_featuretool opp_shlib_postprocess opp_makemake opp_runall opp_neddoc opp_test opp_fingerprinttest

-include $(OBJS:%=%.d)

