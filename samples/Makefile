#
# Toplevel makefile for OMNeT++ samples
#
#===============================================================================

SAMPLES=aloha canvas cqn dyna embedding embedding2 fifo hypercube histograms neddemo queueinglib queueinglibext routing tictoc sockets openstreetmap petrinets osg-intro osg-earth osg-indoor osg-satellites wiredphy

# Pull in OMNeT++ configuration (Makefile.inc)
ifneq ("$(OMNETPP_CONFIGFILE)","")
CONFIGFILE = $(OMNETPP_CONFIGFILE)
else
CONFIGFILE = $(shell opp_configfilepath)
endif

ifeq ("$(wildcard $(CONFIGFILE))","")
$(error Config file '$(CONFIGFILE)' does not exist -- add the OMNeT++ bin directory to the path so that opp_configfilepath can be found, or set the OMNETPP_CONFIGFILE variable to point to Makefile.inc)
endif

include $(CONFIGFILE)

.PHONY: makefiles cleanall clean all install $(SAMPLES)

# main targets

all: $(SAMPLES)

$(SAMPLES) : makefiles

$(SAMPLES):
	@echo ===== Compiling $@ ====
	$(Q)cd $@ && $(MAKE)

clean: makefiles
	$(Q)for i in $(SAMPLES) ""; do \
	    if [ "$$i" != "" ]; then (cd $$i && $(MAKE) clean); fi;\
	done

cleanall: makefiles
	$(Q)for i in $(SAMPLES) ""; do \
	    if [ "$$i" != "" ]; then (cd $$i && $(MAKE) cleanall && rm Makefile); fi;\
	done

queuenet: queueinglib
queueinglibext: queueinglib

makefiles:
	$(Q)(cd embedding && MAKE="$(MAKE)" opp_makemake -f --deep --nolink)
	$(Q)(cd embedding2 && MAKE="$(MAKE)" opp_makemake -f --deep --nolink)
	$(Q)(cd queueinglib && MAKE="$(MAKE)" opp_makemake -f --make-so -pQUEUEING -I.)
	$(Q)(cd queueinglibext && MAKE="$(MAKE)" opp_makemake -f --make-so -I../queueinglib -L'../queueinglib' -l'queueinglib$$(D)' -KQUEUEINGLIB_PROJ=../queueinglib)
	$(Q)(cd queuenet && MAKE="$(MAKE)" opp_makemake -f -n)
	$(Q)for i in $(filter-out embedding embedding2 queueinglib queueinglibext queuenet, $(SAMPLES)) ""; do \
	    if [ "$$i" != "" ]; then (cd $$i && MAKE="$(MAKE)" opp_makemake -f --deep); fi;\
	done
