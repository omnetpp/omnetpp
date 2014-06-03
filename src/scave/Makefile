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
  channel.h \
  nodetype.h \
  xyarray.h \
  dataflowmanager.h \
  ../common/filereader.h \
  ../common/progressmonitor.h \
  scavedefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  node.h \
  arraybuilder.h \
  ../common/commondefs.h \
  commonnodes.h \
  ../common/bigdecimal.h
$O/channel.o: channel.cc \
  channel.h \
  scavedefs.h \
  node.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/bigdecimal.h \
  ../common/commondefs.h
$O/commonnodes.o: commonnodes.cc \
  ../common/bigdecimal.h \
  commonnodes.h \
  ../common/commondefs.h \
  node.h \
  ../common/commonutil.h \
  ../common/exception.h \
  scavedefs.h \
  ../common/progressmonitor.h \
  ../common/filereader.h \
  dataflowmanager.h \
  nodetype.h \
  channel.h
$O/compoundfilter.o: compoundfilter.cc \
  channel.h \
  nodetype.h \
  ../common/filereader.h \
  ../common/progressmonitor.h \
  nodetyperegistry.h \
  dataflowmanager.h \
  scavedefs.h \
  compoundfilter.h \
  ../common/exception.h \
  ../common/commonutil.h \
  node.h \
  filternodes.h \
  commonnodes.h \
  ../common/commondefs.h \
  ../common/bigdecimal.h
$O/customfilter.o: customfilter.cc \
  customfilter.h \
  scaveutils.h \
  dataflowmanager.h \
  ../common/filereader.h \
  nodetype.h \
  ../common/bigdecimal.h \
  ../common/stringutil.h \
  scavedefs.h \
  ../common/expression.h \
  ../common/progressmonitor.h \
  channel.h \
  ../common/stringpool.h \
  ../common/commondefs.h \
  commonnodes.h \
  node.h \
  ../common/commonutil.h \
  ../common/exception.h
$O/dataflowmanager.o: dataflowmanager.cc \
  node.h \
  ../common/commonutil.h \
  ../common/exception.h \
  commonnodes.h \
  ../common/commondefs.h \
  channel.h \
  scavedefs.h \
  ../common/progressmonitor.h \
  ../common/bigdecimal.h \
  nodetype.h \
  ../common/filereader.h \
  dataflowmanager.h
$O/datasorter.o: datasorter.cc \
  scaveutils.h \
  ../common/rwlock.h \
  fields.h \
  datasorter.h \
  ../common/bigdecimal.h \
  idlist.h \
  scavedefs.h \
  ../common/stringutil.h \
  ../common/patternmatcher.h \
  ../common/exception.h \
  ../common/commonutil.h \
  resultfilemanager.h \
  statistics.h \
  ../common/commondefs.h \
  enumtype.h
$O/diffquot.o: diffquot.cc \
  ../common/bigdecimal.h \
  dataflowmanager.h \
  ../common/filereader.h \
  nodetype.h \
  ../common/commondefs.h \
  commonnodes.h \
  ../common/exception.h \
  ../common/commonutil.h \
  node.h \
  diffquot.h \
  ../common/progressmonitor.h \
  scavedefs.h \
  channel.h
$O/enumtype.o: enumtype.cc \
  scavedefs.h \
  ../common/exception.h \
  ../common/commonutil.h \
  ../common/commondefs.h \
  enumtype.h \
  scaveutils.h \
  ../common/stringtokenizer.h \
  ../common/bigdecimal.h
$O/export.o: export.cc \
  statistics.h \
  resultfilemanager.h \
  ../common/exception.h \
  ../common/commonutil.h \
  enumtype.h \
  export.h \
  ../common/commondefs.h \
  xyarray.h \
  ../common/stringutil.h \
  ../common/patternmatcher.h \
  ../common/opp_ctype.h \
  idlist.h \
  scavedefs.h \
  fields.h \
  ../common/rwlock.h \
  ../common/bigdecimal.h \
  datasorter.h \
  scaveutils.h
$O/fields.o: fields.cc \
  scavedefs.h \
  idlist.h \
  ../common/patternmatcher.h \
  ../common/stringutil.h \
  ../common/commondefs.h \
  enumtype.h \
  ../common/exception.h \
  ../common/commonutil.h \
  resultfilemanager.h \
  statistics.h \
  scaveutils.h \
  ../common/bigdecimal.h \
  fields.h \
  ../common/rwlock.h
$O/filewriter.o: filewriter.cc \
  ../common/bigdecimal.h \
  nodetype.h \
  ../common/filereader.h \
  dataflowmanager.h \
  node.h \
  ../common/commonutil.h \
  ../common/exception.h \
  commonnodes.h \
  ../common/commondefs.h \
  channel.h \
  scavedefs.h \
  filewriter.h \
  ../common/progressmonitor.h
$O/filternodes.o: filternodes.cc \
  channel.h \
  ../common/progressmonitor.h \
  scavedefs.h \
  ../common/stringutil.h \
  ../common/exception.h \
  ../common/commonutil.h \
  node.h \
  commonnodes.h \
  filternodes.h \
  ../common/commondefs.h \
  nodetype.h \
  ../common/filereader.h \
  dataflowmanager.h \
  ../common/bigdecimal.h
$O/idlist.o: idlist.cc \
  scavedefs.h \
  idlist.h \
  ../common/stringutil.h \
  ../common/exception.h \
  ../common/commonutil.h \
  resultfilemanager.h \
  statistics.h \
  ../common/commondefs.h \
  enumtype.h \
  scaveutils.h \
  ../common/rwlock.h \
  ../common/bigdecimal.h
$O/indexedvectorfile.o: indexedvectorfile.cc \
  indexfile.h \
  ../common/bigdecimal.h \
  ../common/rwlock.h \
  ../common/filereader.h \
  dataflowmanager.h \
  scaveutils.h \
  nodetype.h \
  ../common/commondefs.h \
  indexedvectorfile.h \
  enumtype.h \
  ../common/exception.h \
  ../common/commonutil.h \
  ../common/linetokenizer.h \
  resultfilemanager.h \
  statistics.h \
  node.h \
  ../common/progressmonitor.h \
  scavedefs.h \
  idlist.h \
  ../common/stringutil.h \
  channel.h
$O/indexedvectorfilereader.o: indexedvectorfilereader.cc \
  commonnodes.h \
  ../common/commondefs.h \
  enumtype.h \
  ../common/exception.h \
  ../common/commonutil.h \
  resultfilemanager.h \
  ../common/linetokenizer.h \
  node.h \
  statistics.h \
  ../common/progressmonitor.h \
  idlist.h \
  scavedefs.h \
  ../common/opp_ctype.h \
  channel.h \
  indexfile.h \
  ../common/bigdecimal.h \
  ../common/rwlock.h \
  indexedvectorfilereader.h \
  ../common/filereader.h \
  scaveutils.h \
  dataflowmanager.h \
  vectorfilereader.h \
  nodetype.h
$O/indexedvectorfilereader2.o: indexedvectorfilereader2.cc \
  nodetype.h \
  ../common/filereader.h \
  scaveutils.h \
  dataflowmanager.h \
  vectorfilereader.h \
  ../common/rwlock.h \
  indexfile.h \
  ../common/bigdecimal.h \
  channel.h \
  indexedvectorfilereader2.h \
  ../common/progressmonitor.h \
  idlist.h \
  scavedefs.h \
  ../common/opp_ctype.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/linetokenizer.h \
  resultfilemanager.h \
  statistics.h \
  node.h \
  commonnodes.h \
  ../common/commondefs.h \
  enumtype.h
$O/indexfile.o: indexfile.cc \
  ../common/bigdecimal.h \
  indexfile.h \
  scaveutils.h \
  ../common/filereader.h \
  ../common/commondefs.h \
  statistics.h \
  ../common/linetokenizer.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/stringutil.h \
  scavedefs.h \
  scaveexception.h
$O/mergernodes.o: mergernodes.cc \
  nodetype.h \
  ../common/filereader.h \
  dataflowmanager.h \
  scaveutils.h \
  ../common/bigdecimal.h \
  channel.h \
  scavedefs.h \
  ../common/progressmonitor.h \
  node.h \
  ../common/exception.h \
  ../common/commonutil.h \
  mergernodes.h \
  commonnodes.h \
  ../common/commondefs.h
$O/nodetype.o: nodetype.cc \
  nodetype.h \
  dataflowmanager.h \
  ../common/bigdecimal.h \
  channel.h \
  ../common/progressmonitor.h \
  scavedefs.h \
  ../common/exception.h \
  ../common/commonutil.h \
  node.h \
  ../common/commondefs.h
$O/nodetyperegistry.o: nodetyperegistry.cc \
  ../common/stringtokenizer.h \
  ../common/rwlock.h \
  indexfile.h \
  ../common/bigdecimal.h \
  xyplotnode.h \
  stddev.h \
  ../common/filereader.h \
  vectorfilereader.h \
  ../common/exception.h \
  ../common/commonutil.h \
  filternodes.h \
  vectorfilewriter.h \
  mergernodes.h \
  windowavg.h \
  ../common/stringpool.h \
  ../common/progressmonitor.h \
  diffquot.h \
  scavedefs.h \
  ../common/stringutil.h \
  filewriter.h \
  indexedvectorfilereader.h \
  arraybuilder.h \
  teenode.h \
  nodetype.h \
  dataflowmanager.h \
  scaveutils.h \
  customfilter.h \
  ../common/linetokenizer.h \
  resultfilemanager.h \
  node.h \
  statistics.h \
  commonnodes.h \
  ../common/commondefs.h \
  slidingwinavg.h \
  indexedvectorfile.h \
  enumtype.h \
  channel.h \
  indexedvectorfilereader2.h \
  xyarray.h \
  ../common/expression.h \
  nodetyperegistry.h \
  idlist.h
$O/octaveexport.o: octaveexport.cc \
  octaveexport.h \
  xyarray.h \
  ../common/opp_ctype.h \
  ../common/commondefs.h \
  ../common/bigdecimal.h \
  scavedefs.h \
  ../common/exception.h
$O/resultfilemanager.o: resultfilemanager.cc \
  enumtype.h \
  ../common/commondefs.h \
  statistics.h \
  resultfilemanager.h \
  ../common/linetokenizer.h \
  idlist.h \
  ../common/opp_ctype.h \
  scaveexception.h \
  scaveutils.h \
  ../common/fileutil.h \
  ../common/exception.h \
  ../common/commonutil.h \
  scavedefs.h \
  ../common/patternmatcher.h \
  ../common/stringutil.h \
  ../common/bigdecimal.h \
  indexfile.h \
  ../common/stringtokenizer.h \
  ../common/rwlock.h \
  ../common/filereader.h \
  ../common/matchexpression.h
$O/scaveexception.o: scaveexception.cc \
  ../common/bigdecimal.h \
  ../common/exception.h \
  scavedefs.h \
  ../common/commondefs.h \
  scaveexception.h
$O/scavetool.o: scavetool.cc \
  nodetype.h \
  dataflowmanager.h \
  scaveutils.h \
  arraybuilder.h \
  datasorter.h \
  channel.h \
  xyarray.h \
  nodetyperegistry.h \
  idlist.h \
  ../common/ver.h \
  resultfilemanager.h \
  ../common/linetokenizer.h \
  node.h \
  statistics.h \
  commonnodes.h \
  ../common/commondefs.h \
  export.h \
  enumtype.h \
  ../common/filereader.h \
  vectorfilereader.h \
  ../common/stringtokenizer.h \
  ../common/rwlock.h \
  fields.h \
  ../common/bigdecimal.h \
  vectorfileindexer.h \
  ../common/progressmonitor.h \
  scavedefs.h \
  ../common/stringutil.h \
  ../common/patternmatcher.h \
  filewriter.h \
  ../common/commonutil.h \
  ../common/exception.h \
  filternodes.h \
  vectorfilewriter.h
$O/scaveutils.o: scaveutils.cc \
  scavedefs.h \
  ../common/exception.h \
  ../common/commonutil.h \
  ../common/bigdecimal.h \
  ../common/commondefs.h \
  scaveutils.h
$O/slidingwinavg.o: slidingwinavg.cc \
  ../common/bigdecimal.h \
  ../common/filereader.h \
  ../common/commonutil.h \
  ../common/exception.h \
  scavedefs.h \
  ../common/progressmonitor.h \
  dataflowmanager.h \
  nodetype.h \
  commonnodes.h \
  ../common/commondefs.h \
  slidingwinavg.h \
  node.h \
  channel.h
$O/stddev.o: stddev.cc \
  ../common/bigdecimal.h \
  ../common/filereader.h \
  stddev.h \
  ../common/commonutil.h \
  ../common/exception.h \
  scavedefs.h \
  ../common/progressmonitor.h \
  dataflowmanager.h \
  scaveutils.h \
  nodetype.h \
  commonnodes.h \
  ../common/commondefs.h \
  node.h \
  xyarray.h \
  channel.h
$O/teenode.o: teenode.cc \
  scavedefs.h \
  ../common/progressmonitor.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/filereader.h \
  ../common/bigdecimal.h \
  channel.h \
  ../common/commondefs.h \
  commonnodes.h \
  node.h \
  dataflowmanager.h \
  teenode.h \
  nodetype.h
$O/vectorfileindexer.o: vectorfileindexer.cc \
  dataflowmanager.h \
  scaveutils.h \
  nodetype.h \
  ../common/commondefs.h \
  indexedvectorfile.h \
  enumtype.h \
  resultfilemanager.h \
  ../common/linetokenizer.h \
  statistics.h \
  node.h \
  scaveexception.h \
  nodetyperegistry.h \
  idlist.h \
  ../common/opp_ctype.h \
  channel.h \
  indexfile.h \
  vectorfileindexer.h \
  ../common/bigdecimal.h \
  ../common/rwlock.h \
  ../common/filereader.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/progressmonitor.h \
  scavedefs.h \
  ../common/stringutil.h
$O/vectorfilereader.o: vectorfilereader.cc \
  scaveexception.h \
  idlist.h \
  ../common/opp_ctype.h \
  channel.h \
  commonnodes.h \
  ../common/commondefs.h \
  enumtype.h \
  resultfilemanager.h \
  ../common/linetokenizer.h \
  node.h \
  statistics.h \
  scaveutils.h \
  dataflowmanager.h \
  nodetype.h \
  ../common/progressmonitor.h \
  scavedefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/filereader.h \
  vectorfilereader.h \
  ../common/bigdecimal.h \
  ../common/rwlock.h
$O/vectorfilewriter.o: vectorfilewriter.cc \
  ../common/stringutil.h \
  scavedefs.h \
  ../common/progressmonitor.h \
  vectorfilewriter.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/bigdecimal.h \
  ../common/rwlock.h \
  idlist.h \
  channel.h \
  enumtype.h \
  ../common/commondefs.h \
  node.h \
  statistics.h \
  resultfilemanager.h \
  scaveutils.h \
  dataflowmanager.h \
  nodetype.h
$O/windowavg.o: windowavg.cc \
  windowavg.h \
  ../common/progressmonitor.h \
  scavedefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/filereader.h \
  ../common/bigdecimal.h \
  channel.h \
  node.h \
  commonnodes.h \
  ../common/commondefs.h \
  nodetype.h \
  scaveutils.h \
  dataflowmanager.h
$O/xyarray.o: xyarray.cc \
  ../common/exception.h \
  scavedefs.h \
  ../common/bigdecimal.h \
  ../common/commondefs.h \
  xyarray.h
$O/xyplotnode.o: xyplotnode.cc \
  xyplotnode.h \
  ../common/filereader.h \
  ../common/bigdecimal.h \
  scavedefs.h \
  ../common/progressmonitor.h \
  ../common/commonutil.h \
  ../common/exception.h \
  nodetype.h \
  dataflowmanager.h \
  channel.h \
  node.h \
  commonnodes.h \
  ../common/commondefs.h
