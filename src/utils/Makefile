#
# variables set by `configure':
#

include ../../Makefile.inc

ifeq ($(strip $(CONFIGNAME)),)
O=.
L=$(OMNETPP_LIB_DIR)
else
O=$(OMNETPP_OBJ_DIR)/$(CONFIGNAME)/src/utils
L=$(OMNETPP_LIB_DIR)/$(CONFIGNAME)
endif

all: $O/seedtool
	echo "echo $(OMNETPP_ROOT)/Makefile.inc" >opp_configfilepath
	chmod +x opp_makemake opp_test opp_makedep opp_neddoc splitvec opp_configfilepath
	cp opp_makemake $(OMNETPP_BIN_DIR)
	cp opp_test $(OMNETPP_BIN_DIR)
	cp opp_makedep $(OMNETPP_BIN_DIR)
	cp opp_neddoc $(OMNETPP_BIN_DIR)
	cp splitvec $(OMNETPP_BIN_DIR)
	cp neddoc.xsl $(OMNETPP_BIN_DIR)
	cp neddocproc.pl $(OMNETPP_BIN_DIR)
	cp opp_configfilepath $(OMNETPP_BIN_DIR)
	cd $O && $(OMNETPP_ROOT)/src/utils/install-prog seedtool$(EXE_SUFFIX) $(OMNETPP_BIN_DIR)
	# next line is for samples/rundemo
	echo $(WISH) >$(HOME)/.wishname

$O/seedtool : seedtool.cc
	@mkdir -p $O
	$(CXX) seedtool.cc -o $O/seedtool$(EXE_SUFFIX)

depend:
	$(MAKEDEPEND) -I. -P\$$O/ -- *.cc *.c

clean:
	rm -f $O/*.o $O/seedtool$(EXE_SUFFIX) 
	rm -f opp_configfilepath

# DO NOT DELETE THIS LINE -- make depend depends on it.
$O/seedtool.o: seedtool.cc
$O/opp_replace.o: opp_replace.c
$O/tcl2c.o: tcl2c.c
