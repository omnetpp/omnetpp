#
# Definitions to be customized by 'configure'
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
LIBNAME=libscave$D
O=$(OMNETPP_OUT_DIR)/$(TOOLCHAIN_NAME)-$(MODE)/src/scave
L=$(OMNETPP_LIB_DIR)/$(TOOLCHAIN_NAME)

COPTS=$(CFLAGS) $(XML_CFLAGS) -I. -I../common -I$(OMNETPP_INCL_DIR)/platdep

IMPLIBS= -L$L -lcommon$D

OBJS= $O/arraybuilder.o $O/channel.o $O/commonnodes.o $O/compoundfilter.o \
      $O/dataflowmanager.o $O/datasorter.o $O/diffquot.o \
      $O/filewriter.o $O/filternodes.o $O/customfilter.o $O/stddev.o \
      $O/idlist.o $O/mergernodes.o $O/nodetype.o $O/nodetyperegistry.o \
      $O/resultfilemanager.o $O/slidingwinavg.o \
      $O/vectorfilereader.o $O/vectorfilewriter.o $O/windowavg.o \
      $O/xyplotnode.o $O/indexedvectorfile.o \
      $O/vectorfileindexer.o $O/indexfile.o $O/scaveutils.o \
      $O/scaveexception.o $O/enumtype.o $O/export.o $O/teenode.o \
      $O/indexedvectorfilereader.o $O/xyarray.o $O/fields.o

#
# Automatic rules
#
.SUFFIXES : .cc

$O/%.o: %.cc
	@mkdir -p $O
	$(CXX) -c $(COPTS) -o $@ $<

#
# Targets
#
all : $L/$(LIBNAME)$(LIB_SUFFIX)
	cd $(OMNETPP_LIB_DIR) && rm -f $(LIBNAME).* && ln -s $(TOOLCHAIN_NAME)/$(LIBNAME).* .

$L/$(LIBNAME)$(A_LIB_SUFFIX): $(OBJS)
	@mkdir -p $L
	$(AR) $O/$(LIBNAME)$(A_LIB_SUFFIX) $(OBJS)
	$(RANLIB) $O/$(LIBNAME)$(A_LIB_SUFFIX)
	cd $O && $(OMNETPP_ROOT)/src/utils/install-lib $(LIBNAME)$(A_LIB_SUFFIX).$(OMNETPP_VERSION) $L $(LIBNAME)$(A_LIB_SUFFIX)

$L/$(LIBNAME)$(SO_LIB_SUFFIX) : $(OBJS)
	@mkdir -p $L
	$(SHLIB_LD) -o $O/$(LIBNAME)$(SO_LIB_SUFFIX) $(OBJS)
	cd $O && $(OMNETPP_ROOT)/src/utils/install-lib $(LIBNAME)$(SO_LIB_SUFFIX).$(OMNETPP_VERSION) $L $(LIBNAME)$(SO_LIB_SUFFIX)

$L/$(LIBNAME)$(DLL_LIB_SUFFIX) : $(OBJS)
	mkdir -p $L
	$(SHLIB_LD) -o $O/$(LIBNAME)$(DLL_LIB_SUFFIX) $(OBJS) $(IMPLIBS) -Wl,--out-implib,$O/$(LIBNAME)$(A_LIB_SUFFIX)
	cd $O && $(OMNETPP_ROOT)/src/utils/install-lib $(LIBNAME)$(A_LIB_SUFFIX).$(OMNETPP_VERSION) $L $(LIBNAME)$(A_LIB_SUFFIX)
	cp $O/$(LIBNAME)$(DLL_LIB_SUFFIX) $(OMNETPP_BIN_DIR)

depend:
	$(MAKEDEPEND) -I. -I../common -P\$$O/ -- *.cc

doc:
	# NOTE: doxy.cfg has the output directory hardcoded!
	$(DOXYGEN) doxy.cfg

clean:
	rm -rf $O
	rm -f $L/$(LIBNAME).* $(OMNETPP_LIB_DIR)/$(LIBNAME).*
	cd $(OMNETPP_BIN_DIR) && rm -f scavetool$(EXE_SUFFIX)

# DO NOT DELETE THIS LINE -- make depend depends on it.
$O/arraybuilder.o: arraybuilder.cc \
  channel.h \
  arraybuilder.h \
  ../common/commonutil.h \
  node.h \
  ../common/commondefs.h \
  ../common/exception.h \
  scavedefs.h \
  ../common/bigdecimal.h \
  commonnodes.h \
  xyarray.h \
  nodetype.h \
  ../common/filereader.h \
  dataflowmanager.h \
  ../common/progressmonitor.h
$O/channel.o: channel.cc \
  channel.h \
  ../common/commonutil.h \
  node.h \
  ../common/commondefs.h \
  ../common/exception.h \
  scavedefs.h \
  ../common/bigdecimal.h
$O/commonnodes.o: commonnodes.cc \
  commonnodes.h \
  channel.h \
  node.h \
  nodetype.h \
  ../common/filereader.h \
  scavedefs.h \
  ../common/commonutil.h \
  ../common/bigdecimal.h \
  ../common/commondefs.h \
  ../common/exception.h \
  dataflowmanager.h \
  ../common/progressmonitor.h
$O/compoundfilter.o: compoundfilter.cc \
  ../common/commonutil.h \
  compoundfilter.h \
  filternodes.h \
  nodetyperegistry.h \
  ../common/commondefs.h \
  ../common/exception.h \
  nodetype.h \
  commonnodes.h \
  dataflowmanager.h \
  node.h \
  ../common/progressmonitor.h \
  channel.h \
  scavedefs.h \
  ../common/bigdecimal.h \
  ../common/filereader.h
$O/customfilter.o: customfilter.cc \
  channel.h \
  customfilter.h \
  scaveutils.h \
  ../common/commonutil.h \
  node.h \
  ../common/commondefs.h \
  ../common/exception.h \
  scavedefs.h \
  ../common/bigdecimal.h \
  commonnodes.h \
  ../common/expression.h \
  nodetype.h \
  ../common/filereader.h \
  dataflowmanager.h \
  ../common/progressmonitor.h \
  ../common/stringutil.h
$O/dataflowmanager.o: dataflowmanager.cc \
  channel.h \
  nodetype.h \
  commonnodes.h \
  dataflowmanager.h \
  ../common/commonutil.h \
  node.h \
  ../common/commondefs.h \
  ../common/exception.h \
  scavedefs.h \
  ../common/bigdecimal.h \
  ../common/filereader.h \
  ../common/progressmonitor.h
$O/datasorter.o: datasorter.cc \
  resultfilemanager.h \
  ../common/commonutil.h \
  datasorter.h \
  ../common/stringutil.h \
  idlist.h \
  enumtype.h \
  ../common/exception.h \
  statistics.h \
  scavedefs.h \
  ../common/bigdecimal.h \
  ../common/commondefs.h \
  scaveutils.h \
  ../common/patternmatcher.h \
  fields.h
$O/diffquot.o: diffquot.cc \
  channel.h \
  diffquot.h \
  ../common/commonutil.h \
  node.h \
  ../common/commondefs.h \
  ../common/exception.h \
  scavedefs.h \
  ../common/bigdecimal.h \
  commonnodes.h \
  nodetype.h \
  ../common/filereader.h \
  dataflowmanager.h \
  ../common/progressmonitor.h
$O/enumtype.o: enumtype.cc \
  ../common/stringtokenizer.h \
  scaveutils.h \
  enumtype.h \
  ../common/commondefs.h \
  scavedefs.h \
  ../common/bigdecimal.h \
  ../common/exception.h
$O/export.o: export.cc \
  ../common/opp_ctype.h \
  scaveutils.h \
  export.h \
  ../common/commondefs.h \
  scavedefs.h \
  ../common/bigdecimal.h \
  ../common/exception.h \
  xyarray.h \
  resultfilemanager.h \
  datasorter.h \
  idlist.h \
  enumtype.h \
  ../common/commonutil.h \
  statistics.h \
  ../common/patternmatcher.h \
  fields.h \
  ../common/stringutil.h
$O/fields.o: fields.cc \
  ../common/commonutil.h \
  ../common/stringutil.h \
  fields.h \
  ../common/commondefs.h \
  ../common/exception.h \
  resultfilemanager.h \
  ../common/patternmatcher.h \
  scaveutils.h \
  idlist.h \
  enumtype.h \
  statistics.h \
  scavedefs.h \
  ../common/bigdecimal.h
$O/filewriter.o: filewriter.cc \
  channel.h \
  filewriter.h \
  ../common/commonutil.h \
  node.h \
  ../common/commondefs.h \
  ../common/exception.h \
  scavedefs.h \
  ../common/bigdecimal.h \
  commonnodes.h \
  nodetype.h \
  ../common/filereader.h \
  dataflowmanager.h \
  ../common/progressmonitor.h
$O/filternodes.o: filternodes.cc \
  channel.h \
  filternodes.h \
  ../common/stringutil.h \
  ../common/commonutil.h \
  node.h \
  ../common/commondefs.h \
  ../common/exception.h \
  scavedefs.h \
  ../common/bigdecimal.h \
  commonnodes.h \
  nodetype.h \
  ../common/filereader.h \
  dataflowmanager.h \
  ../common/progressmonitor.h
$O/idlist.o: idlist.cc \
  idlist.h \
  resultfilemanager.h \
  ../common/stringutil.h \
  scavedefs.h \
  ../common/bigdecimal.h \
  ../common/commondefs.h \
  ../common/exception.h \
  enumtype.h \
  ../common/commonutil.h \
  statistics.h \
  scaveutils.h
$O/indexedvectorfile.o: indexedvectorfile.cc \
  ../common/exception.h \
  ../common/linetokenizer.h \
  channel.h \
  ../common/stringutil.h \
  indexedvectorfile.h \
  scaveutils.h \
  ../common/commondefs.h \
  ../common/commonutil.h \
  node.h \
  scavedefs.h \
  ../common/bigdecimal.h \
  ../common/filereader.h \
  indexfile.h \
  nodetype.h \
  resultfilemanager.h \
  statistics.h \
  dataflowmanager.h \
  ../common/progressmonitor.h \
  idlist.h \
  enumtype.h
$O/indexedvectorfilereader.o: indexedvectorfilereader.cc \
  ../common/opp_ctype.h \
  channel.h \
  scaveutils.h \
  vectorfilereader.h \
  indexedvectorfilereader.h \
  ../common/commondefs.h \
  ../common/commonutil.h \
  node.h \
  ../common/exception.h \
  scavedefs.h \
  ../common/bigdecimal.h \
  nodetype.h \
  commonnodes.h \
  ../common/filereader.h \
  ../common/linetokenizer.h \
  resultfilemanager.h \
  dataflowmanager.h \
  ../common/progressmonitor.h \
  idlist.h \
  enumtype.h \
  statistics.h \
  indexfile.h
$O/indexfile.o: indexfile.cc \
  ../common/exception.h \
  ../common/filereader.h \
  ../common/linetokenizer.h \
  ../common/stringutil.h \
  scaveutils.h \
  scaveexception.h \
  indexfile.h \
  ../common/commondefs.h \
  ../common/commonutil.h \
  scavedefs.h \
  ../common/bigdecimal.h \
  statistics.h
$O/mergernodes.o: mergernodes.cc \
  channel.h \
  mergernodes.h \
  ../common/commonutil.h \
  node.h \
  ../common/commondefs.h \
  ../common/exception.h \
  scavedefs.h \
  ../common/bigdecimal.h \
  commonnodes.h \
  nodetype.h \
  ../common/filereader.h \
  dataflowmanager.h \
  ../common/progressmonitor.h
$O/nodetype.o: nodetype.cc \
  ../common/commonutil.h \
  nodetype.h \
  ../common/commondefs.h \
  ../common/exception.h \
  dataflowmanager.h \
  node.h \
  ../common/progressmonitor.h \
  channel.h \
  scavedefs.h \
  ../common/bigdecimal.h
$O/nodetyperegistry.o: nodetyperegistry.cc \
  ../common/commonutil.h \
  nodetyperegistry.h \
  ../common/stringtokenizer.h \
  arraybuilder.h \
  vectorfilereader.h \
  vectorfilewriter.h \
  indexedvectorfile.h \
  indexedvectorfilereader.h \
  filewriter.h \
  windowavg.h \
  slidingwinavg.h \
  filternodes.h \
  mergernodes.h \
  xyplotnode.h \
  diffquot.h \
  customfilter.h \
  stddev.h \
  teenode.h \
  ../common/commondefs.h \
  ../common/exception.h \
  node.h \
  nodetype.h \
  scavedefs.h \
  ../common/bigdecimal.h \
  dataflowmanager.h \
  ../common/progressmonitor.h \
  channel.h \
  commonnodes.h \
  xyarray.h \
  ../common/filereader.h \
  ../common/linetokenizer.h \
  resultfilemanager.h \
  idlist.h \
  enumtype.h \
  statistics.h \
  scaveutils.h \
  indexfile.h \
  ../common/expression.h \
  ../common/stringutil.h
$O/octaveexport.o: octaveexport.cc \
  ../common/opp_ctype.h \
  octaveexport.h \
  ../common/commondefs.h \
  scavedefs.h \
  xyarray.h \
  ../common/exception.h \
  ../common/bigdecimal.h
$O/resultfilemanager.o: resultfilemanager.cc \
  ../common/opp_ctype.h \
  ../common/matchexpression.h \
  ../common/patternmatcher.h \
  ../common/filereader.h \
  ../common/linetokenizer.h \
  ../common/stringtokenizer.h \
  indexfile.h \
  scaveutils.h \
  resultfilemanager.h \
  ../common/commonutil.h \
  ../common/commondefs.h \
  ../common/exception.h \
  scavedefs.h \
  statistics.h \
  ../common/bigdecimal.h \
  idlist.h \
  enumtype.h
$O/scaveexception.o: scaveexception.cc \
  scaveexception.h \
  ../common/commondefs.h \
  scavedefs.h \
  ../common/exception.h \
  ../common/bigdecimal.h
$O/scavetool.o: scavetool.cc \
  ../utils/ver.h \
  resultfilemanager.h \
  nodetype.h \
  nodetyperegistry.h \
  dataflowmanager.h \
  vectorfilereader.h \
  vectorfilewriter.h \
  vectorfileindexer.h \
  filternodes.h \
  filewriter.h \
  arraybuilder.h \
  export.h \
  ../common/stringutil.h \
  scaveutils.h \
  idlist.h \
  enumtype.h \
  ../common/exception.h \
  ../common/commonutil.h \
  statistics.h \
  scavedefs.h \
  ../common/bigdecimal.h \
  ../common/commondefs.h \
  node.h \
  ../common/progressmonitor.h \
  channel.h \
  commonnodes.h \
  ../common/filereader.h \
  ../common/linetokenizer.h \
  xyarray.h \
  datasorter.h \
  ../common/patternmatcher.h \
  fields.h
$O/scaveutils.o: scaveutils.cc \
  scaveutils.h \
  scavedefs.h \
  ../common/bigdecimal.h \
  ../common/commondefs.h \
  ../common/exception.h
$O/slidingwinavg.o: slidingwinavg.cc \
  channel.h \
  slidingwinavg.h \
  ../common/commonutil.h \
  node.h \
  ../common/commondefs.h \
  ../common/exception.h \
  scavedefs.h \
  ../common/bigdecimal.h \
  commonnodes.h \
  nodetype.h \
  ../common/filereader.h \
  dataflowmanager.h \
  ../common/progressmonitor.h
$O/stddev.o: stddev.cc \
  channel.h \
  stddev.h \
  ../common/commonutil.h \
  node.h \
  ../common/commondefs.h \
  ../common/exception.h \
  scavedefs.h \
  ../common/bigdecimal.h \
  commonnodes.h \
  xyarray.h \
  nodetype.h \
  ../common/filereader.h \
  dataflowmanager.h \
  ../common/progressmonitor.h
$O/teenode.o: teenode.cc \
  channel.h \
  teenode.h \
  ../common/commonutil.h \
  node.h \
  ../common/commondefs.h \
  ../common/exception.h \
  scavedefs.h \
  ../common/bigdecimal.h \
  commonnodes.h \
  nodetype.h \
  ../common/filereader.h \
  dataflowmanager.h \
  ../common/progressmonitor.h
$O/vectorfileindexer.o: vectorfileindexer.cc \
  ../common/opp_ctype.h \
  ../common/stringutil.h \
  scaveutils.h \
  scaveexception.h \
  ../common/filereader.h \
  ../common/linetokenizer.h \
  indexfile.h \
  vectorfileindexer.h \
  ../common/commondefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  scavedefs.h \
  ../common/bigdecimal.h \
  statistics.h \
  resultfilemanager.h \
  ../common/progressmonitor.h \
  idlist.h \
  enumtype.h
$O/vectorfilereader.o: vectorfilereader.cc \
  ../common/opp_ctype.h \
  channel.h \
  scaveutils.h \
  scaveexception.h \
  vectorfilereader.h \
  ../common/commondefs.h \
  ../common/commonutil.h \
  node.h \
  ../common/exception.h \
  scavedefs.h \
  ../common/bigdecimal.h \
  nodetype.h \
  commonnodes.h \
  ../common/filereader.h \
  ../common/linetokenizer.h \
  resultfilemanager.h \
  dataflowmanager.h \
  ../common/progressmonitor.h \
  idlist.h \
  enumtype.h \
  statistics.h
$O/vectorfilewriter.o: vectorfilewriter.cc \
  channel.h \
  vectorfilewriter.h \
  ../common/stringutil.h \
  ../common/commonutil.h \
  node.h \
  ../common/commondefs.h \
  ../common/exception.h \
  scavedefs.h \
  ../common/bigdecimal.h \
  nodetype.h \
  resultfilemanager.h \
  dataflowmanager.h \
  ../common/progressmonitor.h \
  idlist.h \
  enumtype.h \
  statistics.h \
  scaveutils.h
$O/windowavg.o: windowavg.cc \
  channel.h \
  windowavg.h \
  ../common/commonutil.h \
  node.h \
  ../common/commondefs.h \
  ../common/exception.h \
  scavedefs.h \
  ../common/bigdecimal.h \
  commonnodes.h \
  nodetype.h \
  ../common/filereader.h \
  dataflowmanager.h \
  ../common/progressmonitor.h
$O/xyarray.o: xyarray.cc \
  xyarray.h \
  scavedefs.h \
  ../common/bigdecimal.h \
  ../common/commondefs.h \
  ../common/exception.h
$O/xyplotnode.o: xyplotnode.cc \
  channel.h \
  xyplotnode.h \
  ../common/commonutil.h \
  node.h \
  ../common/commondefs.h \
  ../common/exception.h \
  scavedefs.h \
  ../common/bigdecimal.h \
  commonnodes.h \
  nodetype.h \
  ../common/filereader.h \
  dataflowmanager.h \
  ../common/progressmonitor.h
