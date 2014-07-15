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
  ../common/bigdecimal.h \
  dataflowmanager.h \
  channel.h \
  node.h \
  ../common/commondefs.h \
  xyarray.h \
  ../common/filereader.h \
  ../common/progressmonitor.h \
  nodetype.h \
  ../common/exception.h \
  scavedefs.h \
  commonnodes.h \
  arraybuilder.h \
  ../common/commonutil.h
$O/channel.o: channel.cc \
  ../common/bigdecimal.h \
  node.h \
  channel.h \
  ../common/commondefs.h \
  ../common/exception.h \
  scavedefs.h \
  ../common/commonutil.h
$O/commonnodes.o: commonnodes.cc \
  ../common/bigdecimal.h \
  dataflowmanager.h \
  node.h \
  channel.h \
  ../common/commondefs.h \
  ../common/filereader.h \
  ../common/exception.h \
  ../common/progressmonitor.h \
  nodetype.h \
  scavedefs.h \
  commonnodes.h \
  ../common/commonutil.h
$O/compoundfilter.o: compoundfilter.cc \
  ../common/commondefs.h \
  compoundfilter.h \
  ../common/filereader.h \
  ../common/progressmonitor.h \
  ../common/exception.h \
  nodetype.h \
  ../common/bigdecimal.h \
  dataflowmanager.h \
  node.h \
  channel.h \
  scavedefs.h \
  commonnodes.h \
  ../common/commonutil.h \
  nodetyperegistry.h \
  filternodes.h
$O/customfilter.o: customfilter.cc \
  scavedefs.h \
  ../common/commonutil.h \
  ../common/bigdecimal.h \
  ../common/expression.h \
  node.h \
  scaveutils.h \
  ../common/stringutil.h \
  ../common/filereader.h \
  ../common/progressmonitor.h \
  ../common/stringpool.h \
  customfilter.h \
  commonnodes.h \
  dataflowmanager.h \
  channel.h \
  ../common/commondefs.h \
  ../common/exception.h \
  nodetype.h
$O/dataflowmanager.o: dataflowmanager.cc \
  ../common/commonutil.h \
  scavedefs.h \
  ../common/filereader.h \
  ../common/progressmonitor.h \
  node.h \
  ../common/bigdecimal.h \
  commonnodes.h \
  ../common/exception.h \
  nodetype.h \
  ../common/commondefs.h \
  dataflowmanager.h \
  channel.h
$O/datasorter.o: datasorter.cc \
  ../common/rwlock.h \
  idlist.h \
  fields.h \
  ../common/patternmatcher.h \
  ../common/exception.h \
  ../common/commondefs.h \
  resultfilemanager.h \
  scaveutils.h \
  ../common/bigdecimal.h \
  ../common/stringutil.h \
  statistics.h \
  datasorter.h \
  ../common/commonutil.h \
  scavedefs.h \
  enumtype.h
$O/diffquot.o: diffquot.cc \
  commonnodes.h \
  nodetype.h \
  ../common/exception.h \
  ../common/commondefs.h \
  channel.h \
  dataflowmanager.h \
  ../common/commonutil.h \
  scavedefs.h \
  ../common/progressmonitor.h \
  ../common/filereader.h \
  node.h \
  diffquot.h \
  ../common/bigdecimal.h
$O/enumtype.o: enumtype.cc \
  ../common/exception.h \
  ../common/commondefs.h \
  ../common/stringtokenizer.h \
  scaveutils.h \
  ../common/bigdecimal.h \
  ../common/commonutil.h \
  enumtype.h \
  scavedefs.h
$O/export.o: export.cc \
  ../common/exception.h \
  ../common/commondefs.h \
  ../common/rwlock.h \
  idlist.h \
  fields.h \
  ../common/patternmatcher.h \
  export.h \
  ../common/stringutil.h \
  xyarray.h \
  statistics.h \
  resultfilemanager.h \
  scaveutils.h \
  ../common/bigdecimal.h \
  ../common/commonutil.h \
  scavedefs.h \
  enumtype.h \
  ../common/opp_ctype.h \
  datasorter.h
$O/fields.o: fields.cc \
  ../common/commonutil.h \
  enumtype.h \
  scavedefs.h \
  ../common/stringutil.h \
  statistics.h \
  scaveutils.h \
  resultfilemanager.h \
  ../common/bigdecimal.h \
  ../common/exception.h \
  ../common/commondefs.h \
  fields.h \
  ../common/rwlock.h \
  idlist.h \
  ../common/patternmatcher.h
$O/filewriter.o: filewriter.cc \
  commonnodes.h \
  channel.h \
  dataflowmanager.h \
  ../common/commondefs.h \
  ../common/exception.h \
  nodetype.h \
  scavedefs.h \
  ../common/commonutil.h \
  filewriter.h \
  ../common/bigdecimal.h \
  node.h \
  ../common/progressmonitor.h \
  ../common/filereader.h
$O/filternodes.o: filternodes.cc \
  commonnodes.h \
  channel.h \
  dataflowmanager.h \
  nodetype.h \
  ../common/exception.h \
  ../common/commondefs.h \
  filternodes.h \
  ../common/commonutil.h \
  scavedefs.h \
  node.h \
  ../common/bigdecimal.h \
  ../common/progressmonitor.h \
  ../common/stringutil.h \
  ../common/filereader.h
$O/idlist.o: idlist.cc \
  idlist.h \
  ../common/rwlock.h \
  ../common/commondefs.h \
  ../common/exception.h \
  scavedefs.h \
  enumtype.h \
  ../common/commonutil.h \
  ../common/bigdecimal.h \
  resultfilemanager.h \
  scaveutils.h \
  statistics.h \
  ../common/stringutil.h
$O/indexedvectorfile.o: indexedvectorfile.cc \
  channel.h \
  idlist.h \
  ../common/rwlock.h \
  dataflowmanager.h \
  ../common/exception.h \
  nodetype.h \
  ../common/commondefs.h \
  node.h \
  scaveutils.h \
  resultfilemanager.h \
  ../common/bigdecimal.h \
  ../common/progressmonitor.h \
  ../common/stringutil.h \
  ../common/filereader.h \
  indexfile.h \
  statistics.h \
  ../common/commonutil.h \
  indexedvectorfile.h \
  ../common/linetokenizer.h \
  enumtype.h \
  scavedefs.h
$O/indexedvectorfilereader.o: indexedvectorfilereader.cc \
  ../common/bigdecimal.h \
  node.h \
  scaveutils.h \
  resultfilemanager.h \
  indexfile.h \
  vectorfilereader.h \
  statistics.h \
  ../common/progressmonitor.h \
  ../common/filereader.h \
  ../common/linetokenizer.h \
  enumtype.h \
  ../common/opp_ctype.h \
  scavedefs.h \
  ../common/commonutil.h \
  indexedvectorfilereader.h \
  channel.h \
  ../common/rwlock.h \
  dataflowmanager.h \
  idlist.h \
  ../common/commondefs.h \
  ../common/exception.h \
  nodetype.h \
  commonnodes.h
$O/indexedvectorfilereader2.o: indexedvectorfilereader2.cc \
  resultfilemanager.h \
  scaveutils.h \
  node.h \
  indexedvectorfilereader2.h \
  ../common/bigdecimal.h \
  ../common/filereader.h \
  ../common/progressmonitor.h \
  statistics.h \
  vectorfilereader.h \
  indexfile.h \
  ../common/commonutil.h \
  scavedefs.h \
  enumtype.h \
  ../common/opp_ctype.h \
  ../common/linetokenizer.h \
  idlist.h \
  dataflowmanager.h \
  ../common/rwlock.h \
  channel.h \
  ../common/exception.h \
  nodetype.h \
  ../common/commondefs.h \
  commonnodes.h
$O/indexfile.o: indexfile.cc \
  scaveutils.h \
  ../common/bigdecimal.h \
  ../common/filereader.h \
  ../common/stringutil.h \
  indexfile.h \
  statistics.h \
  ../common/commonutil.h \
  ../common/linetokenizer.h \
  scavedefs.h \
  ../common/exception.h \
  scaveexception.h \
  ../common/commondefs.h
$O/mergernodes.o: mergernodes.cc \
  ../common/commondefs.h \
  nodetype.h \
  ../common/exception.h \
  channel.h \
  dataflowmanager.h \
  commonnodes.h \
  ../common/progressmonitor.h \
  ../common/filereader.h \
  ../common/bigdecimal.h \
  node.h \
  scaveutils.h \
  mergernodes.h \
  scavedefs.h \
  ../common/commonutil.h
$O/nodetype.o: nodetype.cc \
  channel.h \
  dataflowmanager.h \
  ../common/commondefs.h \
  ../common/exception.h \
  nodetype.h \
  scavedefs.h \
  ../common/commonutil.h \
  ../common/bigdecimal.h \
  node.h \
  ../common/progressmonitor.h
$O/nodetyperegistry.o: nodetyperegistry.cc \
  filternodes.h \
  slidingwinavg.h \
  arraybuilder.h \
  enumtype.h \
  indexedvectorfilereader2.h \
  scaveutils.h \
  node.h \
  diffquot.h \
  ../common/expression.h \
  filewriter.h \
  ../common/bigdecimal.h \
  ../common/progressmonitor.h \
  ../common/filereader.h \
  ../common/stringutil.h \
  xyarray.h \
  indexfile.h \
  statistics.h \
  customfilter.h \
  idlist.h \
  ../common/rwlock.h \
  ../common/stringtokenizer.h \
  windowavg.h \
  ../common/exception.h \
  ../common/commondefs.h \
  indexedvectorfile.h \
  ../common/commonutil.h \
  vectorfilewriter.h \
  ../common/linetokenizer.h \
  mergernodes.h \
  scavedefs.h \
  resultfilemanager.h \
  vectorfilereader.h \
  ../common/stringpool.h \
  nodetyperegistry.h \
  stddev.h \
  teenode.h \
  xyplotnode.h \
  commonnodes.h \
  channel.h \
  indexedvectorfilereader.h \
  dataflowmanager.h \
  nodetype.h
$O/octaveexport.o: octaveexport.cc \
  scavedefs.h \
  xyarray.h \
  ../common/bigdecimal.h \
  ../common/opp_ctype.h \
  ../common/commondefs.h \
  ../common/exception.h \
  octaveexport.h
$O/resultfilemanager.o: resultfilemanager.cc \
  scaveexception.h \
  ../common/commondefs.h \
  ../common/exception.h \
  ../common/stringtokenizer.h \
  ../common/rwlock.h \
  idlist.h \
  enumtype.h \
  ../common/opp_ctype.h \
  indexfile.h \
  statistics.h \
  ../common/filereader.h \
  ../common/stringutil.h \
  ../common/bigdecimal.h \
  scaveutils.h \
  ../common/fileutil.h \
  ../common/matchexpression.h \
  ../common/patternmatcher.h \
  ../common/linetokenizer.h \
  scavedefs.h \
  ../common/commonutil.h \
  resultfilemanager.h
$O/scaveexception.o: scaveexception.cc \
  ../common/exception.h \
  ../common/commondefs.h \
  scaveexception.h \
  ../common/bigdecimal.h \
  scavedefs.h
$O/scavetool.o: scavetool.cc \
  ../common/patternmatcher.h \
  dataflowmanager.h \
  channel.h \
  nodetype.h \
  nodetyperegistry.h \
  commonnodes.h \
  vectorfileindexer.h \
  resultfilemanager.h \
  vectorfilereader.h \
  scavedefs.h \
  ../common/linetokenizer.h \
  vectorfilewriter.h \
  ../common/commonutil.h \
  ../common/stringtokenizer.h \
  ../common/rwlock.h \
  idlist.h \
  fields.h \
  ../common/commondefs.h \
  ../common/exception.h \
  export.h \
  ../common/bigdecimal.h \
  filewriter.h \
  node.h \
  scaveutils.h \
  statistics.h \
  ../common/filereader.h \
  xyarray.h \
  ../common/ver.h \
  ../common/stringutil.h \
  ../common/progressmonitor.h \
  datasorter.h \
  filternodes.h \
  enumtype.h \
  arraybuilder.h
$O/scaveutils.o: scaveutils.cc \
  ../common/exception.h \
  ../common/commondefs.h \
  scaveutils.h \
  ../common/bigdecimal.h \
  ../common/commonutil.h \
  scavedefs.h
$O/slidingwinavg.o: slidingwinavg.cc \
  scavedefs.h \
  ../common/commonutil.h \
  channel.h \
  dataflowmanager.h \
  nodetype.h \
  commonnodes.h \
  ../common/bigdecimal.h \
  node.h \
  ../common/progressmonitor.h \
  ../common/filereader.h \
  slidingwinavg.h \
  ../common/commondefs.h \
  ../common/exception.h
$O/stddev.o: stddev.cc \
  ../common/commonutil.h \
  scavedefs.h \
  nodetype.h \
  dataflowmanager.h \
  channel.h \
  commonnodes.h \
  stddev.h \
  ../common/filereader.h \
  xyarray.h \
  ../common/progressmonitor.h \
  scaveutils.h \
  node.h \
  ../common/bigdecimal.h \
  ../common/exception.h \
  ../common/commondefs.h
$O/teenode.o: teenode.cc \
  ../common/bigdecimal.h \
  node.h \
  ../common/filereader.h \
  ../common/progressmonitor.h \
  ../common/commondefs.h \
  ../common/exception.h \
  scavedefs.h \
  ../common/commonutil.h \
  dataflowmanager.h \
  channel.h \
  nodetype.h \
  teenode.h \
  commonnodes.h
$O/vectorfileindexer.o: vectorfileindexer.cc \
  nodetype.h \
  dataflowmanager.h \
  channel.h \
  nodetyperegistry.h \
  resultfilemanager.h \
  vectorfileindexer.h \
  indexedvectorfile.h \
  ../common/commonutil.h \
  scavedefs.h \
  ../common/linetokenizer.h \
  ../common/exception.h \
  ../common/commondefs.h \
  scaveexception.h \
  idlist.h \
  ../common/rwlock.h \
  ../common/stringutil.h \
  ../common/filereader.h \
  ../common/progressmonitor.h \
  statistics.h \
  indexfile.h \
  scaveutils.h \
  node.h \
  ../common/bigdecimal.h \
  enumtype.h \
  ../common/opp_ctype.h
$O/vectorfilereader.o: vectorfilereader.cc \
  resultfilemanager.h \
  vectorfilereader.h \
  scavedefs.h \
  ../common/linetokenizer.h \
  ../common/commonutil.h \
  dataflowmanager.h \
  channel.h \
  nodetype.h \
  commonnodes.h \
  ../common/bigdecimal.h \
  node.h \
  scaveutils.h \
  statistics.h \
  ../common/filereader.h \
  ../common/progressmonitor.h \
  ../common/opp_ctype.h \
  enumtype.h \
  ../common/rwlock.h \
  idlist.h \
  ../common/commondefs.h \
  scaveexception.h \
  ../common/exception.h
$O/vectorfilewriter.o: vectorfilewriter.cc \
  ../common/commonutil.h \
  vectorfilewriter.h \
  scavedefs.h \
  resultfilemanager.h \
  channel.h \
  dataflowmanager.h \
  nodetype.h \
  enumtype.h \
  node.h \
  scaveutils.h \
  ../common/bigdecimal.h \
  ../common/progressmonitor.h \
  ../common/stringutil.h \
  statistics.h \
  idlist.h \
  ../common/rwlock.h \
  ../common/exception.h \
  ../common/commondefs.h
$O/windowavg.o: windowavg.cc \
  commonnodes.h \
  nodetype.h \
  channel.h \
  dataflowmanager.h \
  ../common/commonutil.h \
  scavedefs.h \
  ../common/exception.h \
  ../common/commondefs.h \
  windowavg.h \
  ../common/progressmonitor.h \
  ../common/filereader.h \
  scaveutils.h \
  node.h \
  ../common/bigdecimal.h
$O/xyarray.o: xyarray.cc \
  ../common/commondefs.h \
  ../common/exception.h \
  xyarray.h \
  ../common/bigdecimal.h \
  scavedefs.h
$O/xyplotnode.o: xyplotnode.cc \
  scavedefs.h \
  ../common/commonutil.h \
  commonnodes.h \
  xyplotnode.h \
  channel.h \
  dataflowmanager.h \
  nodetype.h \
  ../common/bigdecimal.h \
  node.h \
  ../common/progressmonitor.h \
  ../common/filereader.h \
  ../common/commondefs.h \
  ../common/exception.h
