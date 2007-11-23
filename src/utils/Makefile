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

all: $O/seedtool $O/tcl2c
	chmod +x opp_makemake opp_test opp_makedep opp_neddoc splitvec
	cp opp_makemake $(OMNETPP_BIN_DIR)
	cp opp_test $(OMNETPP_BIN_DIR)
	cp opp_makedep $(OMNETPP_BIN_DIR)
	cp opp_neddoc $(OMNETPP_BIN_DIR)
	cp splitvec $(OMNETPP_BIN_DIR)
	cp neddoc.xsl $(OMNETPP_BIN_DIR)
	cp neddocproc.pl $(OMNETPP_BIN_DIR)
	cd $O && $(OMNETPP_ROOT)/src/utils/install-prog seedtool$(EXE_SUFFIX) $(OMNETPP_BIN_DIR)
	# next line is for samples/rundemo
	echo $(WISH) >$(HOME)/.wishname

$O/seedtool : seedtool.cc
	@mkdir -p $O
	$(CXX) seedtool.cc -o $O/seedtool$(EXE_SUFFIX)

$O/tcl2c: tcl2c.c
	@mkdir -p $O
	$(CC) tcl2c.c -o $O/tcl2c$(EXE_SUFFIX)

depend:
	$(MAKEDEPEND) -I. -P\$$O/ -- *.cc *.c

clean:
	rm -f $O/*.o $O/seedtool$(EXE_SUFFIX) $O/tcl2c$(EXE_SUFFIX)

# DO NOT DELETE THIS LINE -- make depend depends on it.
$O/seedtool.o: seedtool.cc
$O/opp_replace.o: opp_replace.c
$O/tcl2c.o: tcl2c.c
