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
LIBNAME=liboppscave$D
O=$(OMNETPP_OUT_DIR)/$(OUTPUT_PREFIX)$(TOOLCHAIN_NAME)-$(MODE)/src/scave
L=$(OMNETPP_LIB_DIR)/$(OUTPUT_PREFIX)$(TOOLCHAIN_NAME)

COPTS=$(CFLAGS) $(XML_CFLAGS) -I. -I../common -I$(OMNETPP_INCL_DIR)/platdep
IMPLIBS= -L$L -loppcommon$D

ifeq ("$(BUILDING_UILIBS)","yes")
COPTS+= -DTHREADED $(PTHREAD_CFLAGS)
IMPLIBS+= $(PTHREAD_LIBS)
endif

OBJS= $O/arraybuilder.o $O/channel.o $O/commonnodes.o $O/compoundfilter.o \
      $O/dataflowmanager.o $O/datasorter.o $O/diffquot.o \
      $O/filewriter.o $O/filternodes.o $O/customfilter.o $O/stddev.o \
      $O/idlist.o $O/mergernodes.o $O/nodetype.o $O/nodetyperegistry.o \
      $O/resultfilemanager.o $O/slidingwinavg.o \
      $O/vectorfilereader.o $O/vectorfilewriter.o $O/windowavg.o \
      $O/xyplotnode.o $O/indexedvectorfile.o \
      $O/vectorfileindexer.o $O/indexfile.o $O/scaveutils.o \
      $O/scaveexception.o $O/enumtype.o $O/export.o $O/teenode.o \
      $O/indexedvectorfilereader.o $O/xyarray.o $O/fields.o \
      $O/indexedvectorfilereader2.o

#
# Automatic rules
#
.SUFFIXES : .cc

$O/%.o: %.cc
	@mkdir -p $O
	$(qecho) "$<"
	$(Q)$(CXX) -c $(CXXFLAGS) $(COPTS) -o $@ $<

#
# Targets
#
all : $L/$(LIBNAME)$(LIB_SUFFIX) $O/scavetool$(EXE_SUFFIX)
	$(Q)cd $(OMNETPP_LIB_DIR) && rm -f $(LIBNAME).* && $(LN) $(TOOLCHAIN_NAME)/$(LIBNAME).* .

$L/$(LIBNAME)$(A_LIB_SUFFIX): $(OBJS)
	@mkdir -p $L
	@echo Creating static library: $@
	$(Q)$(AR) $O/$(LIBNAME)$(A_LIB_SUFFIX) $(OBJS)
	$(Q)$(RANLIB) $O/$(LIBNAME)$(A_LIB_SUFFIX)
	$(Q)cd $O && $(OMNETPP_ROOT)/src/utils/install-lib $(LIBNAME)$(A_LIB_SUFFIX) $L

$L/$(LIBNAME)$(SO_LIB_SUFFIX) : $(OBJS)
	@mkdir -p $L
	@echo Creating shared library: $@
	$(Q)$(SHLIB_LD) $(LDFLAGS) -o $O/$(LIBNAME)$(SO_LIB_SUFFIX) $(OBJS) $(IMPLIBS)
	$(Q)$(SHLIB_POSTPROCESS) $O/$(LIBNAME)$(SO_LIB_SUFFIX)
	$(Q)cd $O && $(OMNETPP_ROOT)/src/utils/install-lib $(LIBNAME)$(SO_LIB_SUFFIX) $L
	$(Q)rm -f $(OMNETPP_LIB_DIR)/$(LIBNAME)$(SO_LIB_SUFFIX) && $(LN) $L/$(LIBNAME)$(SO_LIB_SUFFIX) $(OMNETPP_LIB_DIR)
	$(Q)$(SHLIB_POSTPROCESS) $(OMNETPP_LIB_DIR)/$(LIBNAME)$(SO_LIB_SUFFIX)

$L/$(LIBNAME)$(DLL_LIB_SUFFIX) : $(OBJS)
	@mkdir -p $L
	@echo Creating DLL: $@
	$(Q)$(SHLIB_LD) $(LDFLAGS) -o $O/$(LIBNAME)$(DLL_LIB_SUFFIX) $(OBJS) $(IMPLIBS) -Wl,--out-implib,$O/$(LIBNAME)$(A_LIB_SUFFIX)
	$(Q)cd $O && $(OMNETPP_ROOT)/src/utils/install-lib $(LIBNAME)$(A_LIB_SUFFIX) $L 
	$(Q)cp $O/$(LIBNAME)$(DLL_LIB_SUFFIX) $(OMNETPP_BIN_DIR)

$O/scavetool$(EXE_SUFFIX) : $O/scavetool.o  $L/$(LIBNAME)$(LIB_SUFFIX)
	@echo Creating executable: $@
	$(Q)$(CXX) $(LDFLAGS) -o $O/scavetool$(EXE_SUFFIX) $O/scavetool.o -L$L -loppscave$D $(IMPLIBS)
	$(Q)cd $O && $(OMNETPP_ROOT)/src/utils/install-prog scavetool$(EXE_SUFFIX) $(OMNETPP_BIN_DIR) 

depend:
	$(qecho) Creating dependencies...
	$(Q)$(MAKEDEPEND) -I. -I../common -P\$$O/ -- *.cc

doc:
	# NOTE: doxy.cfg has the output directory hardcoded!
	$(DOXYGEN) doxy.cfg

clean:
	$(qecho) Cleaning...
	$(Q)rm -rf $O
	$(Q)rm -f $L/$(LIBNAME).* $(OMNETPP_LIB_DIR)/$(LIBNAME).*
	$(Q)cd $(OMNETPP_BIN_DIR) && rm -f scavetool$(EXE_SUFFIX)

# DO NOT DELETE THIS LINE -- make depend depends on it.
$O/arraybuilder.o: arraybuilder.cc \
  commonnodes.h \
  nodetype.h \
  node.h \
  channel.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/progressmonitor.h \
  scavedefs.h \
  dataflowmanager.h \
  ../common/commondefs.h \
  xyarray.h \
  arraybuilder.h \
  ../common/bigdecimal.h \
  ../common/filereader.h
$O/channel.o: channel.cc \
  node.h \
  channel.h \
  ../common/commonutil.h \
  ../common/exception.h \
  scavedefs.h \
  ../common/commondefs.h \
  ../common/bigdecimal.h
$O/commonnodes.o: commonnodes.cc \
  nodetype.h \
  commonnodes.h \
  node.h \
  channel.h \
  ../common/exception.h \
  ../common/commonutil.h \
  scavedefs.h \
  ../common/progressmonitor.h \
  ../common/commondefs.h \
  dataflowmanager.h \
  ../common/bigdecimal.h \
  ../common/filereader.h
$O/compoundfilter.o: compoundfilter.cc \
  nodetype.h \
  commonnodes.h \
  node.h \
  channel.h \
  ../common/exception.h \
  ../common/commonutil.h \
  ../common/progressmonitor.h \
  scavedefs.h \
  compoundfilter.h \
  dataflowmanager.h \
  ../common/commondefs.h \
  nodetyperegistry.h \
  ../common/bigdecimal.h \
  ../common/filereader.h \
  filternodes.h
$O/customfilter.o: customfilter.cc \
  customfilter.h \
  scavedefs.h \
  ../common/commondefs.h \
  scaveutils.h \
  ../common/stringutil.h \
  nodetype.h \
  commonnodes.h \
  node.h \
  ../common/expression.h \
  channel.h \
  ../common/exception.h \
  ../common/commonutil.h \
  ../common/progressmonitor.h \
  dataflowmanager.h \
  ../common/bigdecimal.h \
  ../common/stringpool.h \
  ../common/filereader.h
$O/dataflowmanager.o: dataflowmanager.cc \
  scavedefs.h \
  ../common/commondefs.h \
  commonnodes.h \
  nodetype.h \
  node.h \
  channel.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/progressmonitor.h \
  dataflowmanager.h \
  ../common/bigdecimal.h \
  ../common/filereader.h
$O/datasorter.o: datasorter.cc \
  fields.h \
  idlist.h \
  datasorter.h \
  scavedefs.h \
  ../common/commondefs.h \
  scaveutils.h \
  ../common/patternmatcher.h \
  ../common/stringutil.h \
  ../common/exception.h \
  ../common/commonutil.h \
  enumtype.h \
  resultfilemanager.h \
  ../common/rwlock.h \
  ../common/bigdecimal.h \
  statistics.h
$O/diffquot.o: diffquot.cc \
  scavedefs.h \
  ../common/commondefs.h \
  commonnodes.h \
  nodetype.h \
  node.h \
  diffquot.h \
  channel.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/progressmonitor.h \
  dataflowmanager.h \
  ../common/bigdecimal.h \
  ../common/filereader.h
$O/enumtype.o: enumtype.cc \
  scavedefs.h \
  ../common/commondefs.h \
  scaveutils.h \
  ../common/stringtokenizer.h \
  ../common/commonutil.h \
  ../common/exception.h \
  enumtype.h \
  ../common/bigdecimal.h
$O/export.o: export.cc \
  fields.h \
  datasorter.h \
  idlist.h \
  export.h \
  ../common/opp_ctype.h \
  scavedefs.h \
  ../common/commondefs.h \
  xyarray.h \
  scaveutils.h \
  ../common/stringutil.h \
  ../common/patternmatcher.h \
  ../common/exception.h \
  ../common/commonutil.h \
  enumtype.h \
  resultfilemanager.h \
  ../common/rwlock.h \
  ../common/bigdecimal.h \
  statistics.h
$O/fields.o: fields.cc \
  fields.h \
  idlist.h \
  scavedefs.h \
  ../common/commondefs.h \
  scaveutils.h \
  ../common/patternmatcher.h \
  ../common/stringutil.h \
  ../common/exception.h \
  ../common/commonutil.h \
  resultfilemanager.h \
  enumtype.h \
  ../common/bigdecimal.h \
  ../common/rwlock.h \
  statistics.h
$O/filewriter.o: filewriter.cc \
  scavedefs.h \
  ../common/commondefs.h \
  filewriter.h \
  commonnodes.h \
  nodetype.h \
  node.h \
  channel.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/progressmonitor.h \
  dataflowmanager.h \
  ../common/bigdecimal.h \
  ../common/filereader.h
$O/filternodes.o: filternodes.cc \
  scavedefs.h \
  ../common/commondefs.h \
  ../common/stringutil.h \
  commonnodes.h \
  nodetype.h \
  node.h \
  channel.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/progressmonitor.h \
  dataflowmanager.h \
  ../common/bigdecimal.h \
  ../common/filereader.h \
  filternodes.h
$O/idlist.o: idlist.cc \
  idlist.h \
  scavedefs.h \
  ../common/commondefs.h \
  scaveutils.h \
  ../common/stringutil.h \
  ../common/commonutil.h \
  ../common/exception.h \
  enumtype.h \
  resultfilemanager.h \
  ../common/bigdecimal.h \
  ../common/rwlock.h \
  statistics.h
$O/indexedvectorfile.o: indexedvectorfile.cc \
  idlist.h \
  scavedefs.h \
  indexfile.h \
  ../common/commondefs.h \
  scaveutils.h \
  ../common/stringutil.h \
  nodetype.h \
  node.h \
  channel.h \
  ../common/commonutil.h \
  ../common/exception.h \
  resultfilemanager.h \
  ../common/progressmonitor.h \
  enumtype.h \
  dataflowmanager.h \
  ../common/linetokenizer.h \
  ../common/rwlock.h \
  ../common/bigdecimal.h \
  indexedvectorfile.h \
  ../common/filereader.h \
  statistics.h
$O/indexedvectorfilereader.o: indexedvectorfilereader.cc \
  idlist.h \
  ../common/opp_ctype.h \
  scavedefs.h \
  indexfile.h \
  ../common/commondefs.h \
  scaveutils.h \
  vectorfilereader.h \
  nodetype.h \
  commonnodes.h \
  node.h \
  channel.h \
  ../common/commonutil.h \
  ../common/exception.h \
  resultfilemanager.h \
  ../common/progressmonitor.h \
  enumtype.h \
  ../common/linetokenizer.h \
  dataflowmanager.h \
  indexedvectorfilereader.h \
  ../common/rwlock.h \
  ../common/bigdecimal.h \
  ../common/filereader.h \
  statistics.h
$O/indexedvectorfilereader2.o: indexedvectorfilereader2.cc \
  idlist.h \
  ../common/opp_ctype.h \
  scavedefs.h \
  indexfile.h \
  ../common/commondefs.h \
  indexedvectorfilereader2.h \
  scaveutils.h \
  vectorfilereader.h \
  nodetype.h \
  commonnodes.h \
  node.h \
  channel.h \
  ../common/commonutil.h \
  ../common/exception.h \
  resultfilemanager.h \
  ../common/progressmonitor.h \
  enumtype.h \
  ../common/linetokenizer.h \
  dataflowmanager.h \
  ../common/rwlock.h \
  ../common/bigdecimal.h \
  ../common/filereader.h \
  statistics.h
$O/indexfile.o: indexfile.cc \
  scaveexception.h \
  scavedefs.h \
  ../common/commondefs.h \
  indexfile.h \
  scaveutils.h \
  ../common/stringutil.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/linetokenizer.h \
  ../common/bigdecimal.h \
  statistics.h \
  ../common/filereader.h
$O/mergernodes.o: mergernodes.cc \
  scavedefs.h \
  ../common/commondefs.h \
  mergernodes.h \
  scaveutils.h \
  commonnodes.h \
  nodetype.h \
  node.h \
  channel.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/progressmonitor.h \
  dataflowmanager.h \
  ../common/bigdecimal.h \
  ../common/filereader.h
$O/nodetype.o: nodetype.cc \
  scavedefs.h \
  ../common/commondefs.h \
  nodetype.h \
  node.h \
  channel.h \
  ../common/exception.h \
  ../common/commonutil.h \
  ../common/progressmonitor.h \
  dataflowmanager.h \
  ../common/bigdecimal.h
$O/nodetyperegistry.o: nodetyperegistry.cc \
  teenode.h \
  customfilter.h \
  slidingwinavg.h \
  indexfile.h \
  ../common/commondefs.h \
  indexedvectorfilereader2.h \
  xyarray.h \
  scaveutils.h \
  ../common/stringutil.h \
  vectorfilereader.h \
  ../common/expression.h \
  node.h \
  diffquot.h \
  channel.h \
  windowavg.h \
  resultfilemanager.h \
  enumtype.h \
  ../common/progressmonitor.h \
  ../common/linetokenizer.h \
  dataflowmanager.h \
  indexedvectorfilereader.h \
  arraybuilder.h \
  ../common/rwlock.h \
  ../common/stringpool.h \
  statistics.h \
  filternodes.h \
  xyplotnode.h \
  vectorfilewriter.h \
  idlist.h \
  scavedefs.h \
  mergernodes.h \
  nodetyperegistry.h \
  filewriter.h \
  ../common/stringtokenizer.h \
  stddev.h \
  nodetype.h \
  commonnodes.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/bigdecimal.h \
  indexedvectorfile.h \
  ../common/filereader.h
$O/octaveexport.o: octaveexport.cc \
  octaveexport.h \
  ../common/opp_ctype.h \
  ../common/commondefs.h \
  xyarray.h \
  scavedefs.h \
  ../common/exception.h \
  ../common/bigdecimal.h
$O/resultfilemanager.o: resultfilemanager.cc \
  scaveexception.h \
  ../common/opp_ctype.h \
  ../common/fileutil.h \
  ../common/commondefs.h \
  indexfile.h \
  scaveutils.h \
  ../common/stringutil.h \
  ../common/patternmatcher.h \
  ../common/matchexpression.h \
  enumtype.h \
  resultfilemanager.h \
  ../common/linetokenizer.h \
  ../common/rwlock.h \
  statistics.h \
  idlist.h \
  scavedefs.h \
  ../common/stringtokenizer.h \
  ../common/exception.h \
  ../common/commonutil.h \
  ../common/bigdecimal.h \
  ../common/filereader.h
$O/scaveexception.o: scaveexception.cc \
  scaveexception.h \
  ../common/commondefs.h \
  scavedefs.h \
  ../common/exception.h \
  ../common/bigdecimal.h
$O/scavetool.o: scavetool.cc \
  datasorter.h \
  export.h \
  ../common/commondefs.h \
  xyarray.h \
  scaveutils.h \
  ../common/patternmatcher.h \
  ../common/stringutil.h \
  vectorfilereader.h \
  node.h \
  channel.h \
  ../common/progressmonitor.h \
  enumtype.h \
  resultfilemanager.h \
  dataflowmanager.h \
  ../common/linetokenizer.h \
  arraybuilder.h \
  ../common/rwlock.h \
  statistics.h \
  filternodes.h \
  fields.h \
  vectorfilewriter.h \
  idlist.h \
  scavedefs.h \
  nodetyperegistry.h \
  filewriter.h \
  ../common/stringtokenizer.h \
  commonnodes.h \
  nodetype.h \
  ../common/exception.h \
  ../common/commonutil.h \
  vectorfileindexer.h \
  ../common/ver.h \
  ../common/bigdecimal.h \
  ../common/filereader.h
$O/scaveutils.o: scaveutils.cc \
  ../common/commondefs.h \
  scaveutils.h \
  scavedefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/bigdecimal.h
$O/slidingwinavg.o: slidingwinavg.cc \
  slidingwinavg.h \
  ../common/commondefs.h \
  node.h \
  channel.h \
  ../common/progressmonitor.h \
  dataflowmanager.h \
  scavedefs.h \
  commonnodes.h \
  nodetype.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/bigdecimal.h \
  ../common/filereader.h
$O/stddev.o: stddev.cc \
  ../common/commondefs.h \
  xyarray.h \
  scaveutils.h \
  node.h \
  channel.h \
  ../common/progressmonitor.h \
  dataflowmanager.h \
  scavedefs.h \
  stddev.h \
  commonnodes.h \
  nodetype.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/bigdecimal.h \
  ../common/filereader.h
$O/teenode.o: teenode.cc \
  teenode.h \
  ../common/commondefs.h \
  node.h \
  channel.h \
  ../common/progressmonitor.h \
  dataflowmanager.h \
  scavedefs.h \
  commonnodes.h \
  nodetype.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/bigdecimal.h \
  ../common/filereader.h
$O/vectorfileindexer.o: vectorfileindexer.cc \
  scaveexception.h \
  ../common/opp_ctype.h \
  ../common/commondefs.h \
  indexfile.h \
  scaveutils.h \
  ../common/stringutil.h \
  node.h \
  channel.h \
  resultfilemanager.h \
  enumtype.h \
  ../common/progressmonitor.h \
  dataflowmanager.h \
  ../common/linetokenizer.h \
  ../common/rwlock.h \
  statistics.h \
  idlist.h \
  scavedefs.h \
  nodetyperegistry.h \
  nodetype.h \
  ../common/commonutil.h \
  ../common/exception.h \
  vectorfileindexer.h \
  ../common/bigdecimal.h \
  indexedvectorfile.h \
  ../common/filereader.h
$O/vectorfilereader.o: vectorfilereader.cc \
  scaveexception.h \
  ../common/opp_ctype.h \
  ../common/commondefs.h \
  scaveutils.h \
  vectorfilereader.h \
  node.h \
  channel.h \
  resultfilemanager.h \
  ../common/progressmonitor.h \
  enumtype.h \
  ../common/linetokenizer.h \
  dataflowmanager.h \
  ../common/rwlock.h \
  statistics.h \
  idlist.h \
  scavedefs.h \
  nodetype.h \
  commonnodes.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/bigdecimal.h \
  ../common/filereader.h
$O/vectorfilewriter.o: vectorfilewriter.cc \
  ../common/commondefs.h \
  scaveutils.h \
  ../common/stringutil.h \
  node.h \
  channel.h \
  resultfilemanager.h \
  ../common/progressmonitor.h \
  enumtype.h \
  dataflowmanager.h \
  ../common/rwlock.h \
  statistics.h \
  vectorfilewriter.h \
  idlist.h \
  scavedefs.h \
  nodetype.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/bigdecimal.h
$O/windowavg.o: windowavg.cc \
  ../common/commondefs.h \
  scaveutils.h \
  node.h \
  channel.h \
  windowavg.h \
  ../common/progressmonitor.h \
  dataflowmanager.h \
  scavedefs.h \
  commonnodes.h \
  nodetype.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/bigdecimal.h \
  ../common/filereader.h
$O/xyarray.o: xyarray.cc \
  ../common/commondefs.h \
  xyarray.h \
  scavedefs.h \
  ../common/exception.h \
  ../common/bigdecimal.h
$O/xyplotnode.o: xyplotnode.cc \
  ../common/commondefs.h \
  node.h \
  channel.h \
  ../common/progressmonitor.h \
  dataflowmanager.h \
  xyplotnode.h \
  scavedefs.h \
  commonnodes.h \
  nodetype.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/bigdecimal.h \
  ../common/filereader.h
