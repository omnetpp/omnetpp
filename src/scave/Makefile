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

INCL_FLAGS= -I"$(OMNETPP_INCL_DIR)" -I"$(OMNETPP_SRC_DIR)"

COPTS=$(CFLAGS) $(XML_CFLAGS) $(INCL_FLAGS)
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
	$(Q)$(MAKEDEPEND) -I../../include -I.. -P\$$O/ -- *.cc

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
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h \
  ../common/bigdecimal.h \
  ../common/commondefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/filereader.h \
  ../common/progressmonitor.h \
  arraybuilder.h \
  channel.h \
  commonnodes.h \
  dataflowmanager.h \
  node.h \
  nodetype.h \
  scavedefs.h \
  xyarray.h
$O/channel.o: channel.cc \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../common/bigdecimal.h \
  ../common/commondefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  channel.h \
  node.h \
  scavedefs.h
$O/commonnodes.o: commonnodes.cc \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h \
  ../common/bigdecimal.h \
  ../common/commondefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/filereader.h \
  ../common/progressmonitor.h \
  channel.h \
  commonnodes.h \
  dataflowmanager.h \
  node.h \
  nodetype.h \
  scavedefs.h
$O/compoundfilter.o: compoundfilter.cc \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h \
  ../common/bigdecimal.h \
  ../common/commondefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/filereader.h \
  ../common/progressmonitor.h \
  channel.h \
  commonnodes.h \
  compoundfilter.h \
  dataflowmanager.h \
  enums.h \
  filternodes.h \
  node.h \
  nodetype.h \
  nodetyperegistry.h \
  scavedefs.h
$O/customfilter.o: customfilter.cc \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h \
  ../../include/omnetpp/platdep/timeutil.h \
  ../common/bigdecimal.h \
  ../common/commondefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/expression.h \
  ../common/filereader.h \
  ../common/progressmonitor.h \
  ../common/stringpool.h \
  ../common/stringutil.h \
  channel.h \
  commonnodes.h \
  customfilter.h \
  dataflowmanager.h \
  node.h \
  nodetype.h \
  scavedefs.h \
  scaveutils.h
$O/dataflowmanager.o: dataflowmanager.cc \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h \
  ../common/bigdecimal.h \
  ../common/commondefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/filereader.h \
  ../common/progressmonitor.h \
  channel.h \
  commonnodes.h \
  dataflowmanager.h \
  node.h \
  nodetype.h \
  scavedefs.h
$O/datasorter.o: datasorter.cc \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/timeutil.h \
  ../common/bigdecimal.h \
  ../common/commondefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/patternmatcher.h \
  ../common/rwlock.h \
  ../common/stringutil.h \
  datasorter.h \
  enums.h \
  enumtype.h \
  fields.h \
  idlist.h \
  resultfilemanager.h \
  scavedefs.h \
  scaveutils.h \
  statistics.h
$O/diffquot.o: diffquot.cc \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h \
  ../common/bigdecimal.h \
  ../common/commondefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/filereader.h \
  ../common/progressmonitor.h \
  channel.h \
  commonnodes.h \
  dataflowmanager.h \
  diffquot.h \
  node.h \
  nodetype.h \
  scavedefs.h
$O/enumtype.o: enumtype.cc \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/timeutil.h \
  ../common/bigdecimal.h \
  ../common/commondefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/stringtokenizer.h \
  enumtype.h \
  scavedefs.h \
  scaveutils.h
$O/export.o: export.cc \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/timeutil.h \
  ../common/bigdecimal.h \
  ../common/commondefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/opp_ctype.h \
  ../common/patternmatcher.h \
  ../common/rwlock.h \
  ../common/stringutil.h \
  datasorter.h \
  enums.h \
  enumtype.h \
  export.h \
  fields.h \
  idlist.h \
  resultfilemanager.h \
  scavedefs.h \
  scaveutils.h \
  statistics.h \
  xyarray.h
$O/fields.o: fields.cc \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/timeutil.h \
  ../common/bigdecimal.h \
  ../common/commondefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/patternmatcher.h \
  ../common/rwlock.h \
  ../common/stringutil.h \
  enums.h \
  enumtype.h \
  fields.h \
  idlist.h \
  resultfilemanager.h \
  scavedefs.h \
  scaveutils.h \
  statistics.h
$O/filewriter.o: filewriter.cc \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h \
  ../common/bigdecimal.h \
  ../common/commondefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/filereader.h \
  ../common/progressmonitor.h \
  channel.h \
  commonnodes.h \
  dataflowmanager.h \
  filewriter.h \
  node.h \
  nodetype.h \
  scavedefs.h
$O/filternodes.o: filternodes.cc \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h \
  ../common/bigdecimal.h \
  ../common/commondefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/filereader.h \
  ../common/progressmonitor.h \
  ../common/stringutil.h \
  channel.h \
  commonnodes.h \
  dataflowmanager.h \
  enums.h \
  filternodes.h \
  node.h \
  nodetype.h \
  scavedefs.h
$O/idlist.o: idlist.cc \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/timeutil.h \
  ../common/bigdecimal.h \
  ../common/commondefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/rwlock.h \
  ../common/stringutil.h \
  enums.h \
  enumtype.h \
  idlist.h \
  resultfilemanager.h \
  scavedefs.h \
  scaveutils.h \
  statistics.h
$O/indexedvectorfile.o: indexedvectorfile.cc \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h \
  ../../include/omnetpp/platdep/timeutil.h \
  ../common/bigdecimal.h \
  ../common/commondefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/filereader.h \
  ../common/linetokenizer.h \
  ../common/progressmonitor.h \
  ../common/rwlock.h \
  ../common/stringutil.h \
  channel.h \
  dataflowmanager.h \
  enums.h \
  enumtype.h \
  idlist.h \
  indexedvectorfile.h \
  indexfile.h \
  node.h \
  nodetype.h \
  resultfilemanager.h \
  scavedefs.h \
  scaveutils.h \
  statistics.h
$O/indexedvectorfilereader.o: indexedvectorfilereader.cc \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h \
  ../../include/omnetpp/platdep/timeutil.h \
  ../common/bigdecimal.h \
  ../common/commondefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/filereader.h \
  ../common/linetokenizer.h \
  ../common/opp_ctype.h \
  ../common/progressmonitor.h \
  ../common/rwlock.h \
  channel.h \
  commonnodes.h \
  dataflowmanager.h \
  enums.h \
  enumtype.h \
  idlist.h \
  indexedvectorfilereader.h \
  indexfile.h \
  node.h \
  nodetype.h \
  resultfilemanager.h \
  scavedefs.h \
  scaveutils.h \
  statistics.h \
  vectorfilereader.h
$O/indexedvectorfilereader2.o: indexedvectorfilereader2.cc \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h \
  ../../include/omnetpp/platdep/timeutil.h \
  ../common/bigdecimal.h \
  ../common/commondefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/filereader.h \
  ../common/linetokenizer.h \
  ../common/opp_ctype.h \
  ../common/progressmonitor.h \
  ../common/rwlock.h \
  channel.h \
  commonnodes.h \
  dataflowmanager.h \
  enums.h \
  enumtype.h \
  idlist.h \
  indexedvectorfilereader2.h \
  indexfile.h \
  node.h \
  nodetype.h \
  resultfilemanager.h \
  scavedefs.h \
  scaveutils.h \
  statistics.h \
  vectorfilereader.h
$O/indexfile.o: indexfile.cc \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h \
  ../../include/omnetpp/platdep/timeutil.h \
  ../common/bigdecimal.h \
  ../common/commondefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/filereader.h \
  ../common/linetokenizer.h \
  ../common/stringutil.h \
  indexfile.h \
  scavedefs.h \
  scaveexception.h \
  scaveutils.h \
  statistics.h
$O/mergernodes.o: mergernodes.cc \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h \
  ../../include/omnetpp/platdep/timeutil.h \
  ../common/bigdecimal.h \
  ../common/commondefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/filereader.h \
  ../common/progressmonitor.h \
  channel.h \
  commonnodes.h \
  dataflowmanager.h \
  mergernodes.h \
  node.h \
  nodetype.h \
  scavedefs.h \
  scaveutils.h
$O/nodetype.o: nodetype.cc \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../common/bigdecimal.h \
  ../common/commondefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/progressmonitor.h \
  channel.h \
  dataflowmanager.h \
  node.h \
  nodetype.h \
  scavedefs.h
$O/nodetyperegistry.o: nodetyperegistry.cc \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h \
  ../../include/omnetpp/platdep/timeutil.h \
  ../common/bigdecimal.h \
  ../common/commondefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/expression.h \
  ../common/filereader.h \
  ../common/linetokenizer.h \
  ../common/progressmonitor.h \
  ../common/rwlock.h \
  ../common/stringpool.h \
  ../common/stringtokenizer.h \
  ../common/stringutil.h \
  arraybuilder.h \
  channel.h \
  commonnodes.h \
  customfilter.h \
  dataflowmanager.h \
  diffquot.h \
  enums.h \
  enumtype.h \
  filewriter.h \
  filternodes.h \
  idlist.h \
  indexedvectorfile.h \
  indexedvectorfilereader.h \
  indexedvectorfilereader2.h \
  indexfile.h \
  mergernodes.h \
  node.h \
  nodetype.h \
  nodetyperegistry.h \
  resultfilemanager.h \
  scavedefs.h \
  scaveutils.h \
  slidingwinavg.h \
  statistics.h \
  stddev.h \
  teenode.h \
  vectorfilereader.h \
  vectorfilewriter.h \
  windowavg.h \
  xyarray.h \
  xyplotnode.h
$O/octaveexport.o: octaveexport.cc \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../common/bigdecimal.h \
  ../common/commondefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/opp_ctype.h \
  octaveexport.h \
  scavedefs.h \
  xyarray.h
$O/resultfilemanager.o: resultfilemanager.cc \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h \
  ../../include/omnetpp/platdep/timeutil.h \
  ../common/bigdecimal.h \
  ../common/commondefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/filereader.h \
  ../common/fileutil.h \
  ../common/linetokenizer.h \
  ../common/matchexpression.h \
  ../common/opp_ctype.h \
  ../common/patternmatcher.h \
  ../common/rwlock.h \
  ../common/stringtokenizer.h \
  ../common/stringutil.h \
  enums.h \
  enumtype.h \
  idlist.h \
  indexfile.h \
  resultfilemanager.h \
  scavedefs.h \
  scaveexception.h \
  scaveutils.h \
  statistics.h
$O/scaveexception.o: scaveexception.cc \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h \
  ../common/bigdecimal.h \
  ../common/commondefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  scavedefs.h \
  scaveexception.h
$O/scavetool.o: scavetool.cc \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h \
  ../../include/omnetpp/platdep/timeutil.h \
  ../common/bigdecimal.h \
  ../common/commondefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/filereader.h \
  ../common/linetokenizer.h \
  ../common/patternmatcher.h \
  ../common/progressmonitor.h \
  ../common/rwlock.h \
  ../common/stringtokenizer.h \
  ../common/stringutil.h \
  ../common/ver.h \
  arraybuilder.h \
  channel.h \
  commonnodes.h \
  dataflowmanager.h \
  datasorter.h \
  enums.h \
  enumtype.h \
  export.h \
  fields.h \
  filewriter.h \
  filternodes.h \
  idlist.h \
  node.h \
  nodetype.h \
  nodetyperegistry.h \
  resultfilemanager.h \
  scavedefs.h \
  scaveutils.h \
  statistics.h \
  vectorfileindexer.h \
  vectorfilereader.h \
  vectorfilewriter.h \
  xyarray.h
$O/scaveutils.o: scaveutils.cc \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h \
  ../../include/omnetpp/platdep/timeutil.h \
  ../common/bigdecimal.h \
  ../common/commondefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  scavedefs.h \
  scaveutils.h
$O/slidingwinavg.o: slidingwinavg.cc \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h \
  ../common/bigdecimal.h \
  ../common/commondefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/filereader.h \
  ../common/progressmonitor.h \
  channel.h \
  commonnodes.h \
  dataflowmanager.h \
  node.h \
  nodetype.h \
  scavedefs.h \
  slidingwinavg.h
$O/stddev.o: stddev.cc \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h \
  ../../include/omnetpp/platdep/timeutil.h \
  ../common/bigdecimal.h \
  ../common/commondefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/filereader.h \
  ../common/progressmonitor.h \
  channel.h \
  commonnodes.h \
  dataflowmanager.h \
  node.h \
  nodetype.h \
  scavedefs.h \
  scaveutils.h \
  stddev.h \
  xyarray.h
$O/teenode.o: teenode.cc \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h \
  ../common/bigdecimal.h \
  ../common/commondefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/filereader.h \
  ../common/progressmonitor.h \
  channel.h \
  commonnodes.h \
  dataflowmanager.h \
  node.h \
  nodetype.h \
  scavedefs.h \
  teenode.h
$O/vectorfileindexer.o: vectorfileindexer.cc \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h \
  ../../include/omnetpp/platdep/timeutil.h \
  ../common/bigdecimal.h \
  ../common/commondefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/filereader.h \
  ../common/linetokenizer.h \
  ../common/opp_ctype.h \
  ../common/progressmonitor.h \
  ../common/rwlock.h \
  ../common/stringutil.h \
  channel.h \
  dataflowmanager.h \
  enums.h \
  enumtype.h \
  idlist.h \
  indexedvectorfile.h \
  indexfile.h \
  node.h \
  nodetype.h \
  nodetyperegistry.h \
  resultfilemanager.h \
  scavedefs.h \
  scaveexception.h \
  scaveutils.h \
  statistics.h \
  vectorfileindexer.h
$O/vectorfilereader.o: vectorfilereader.cc \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h \
  ../../include/omnetpp/platdep/timeutil.h \
  ../common/bigdecimal.h \
  ../common/commondefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/filereader.h \
  ../common/linetokenizer.h \
  ../common/opp_ctype.h \
  ../common/progressmonitor.h \
  ../common/rwlock.h \
  channel.h \
  commonnodes.h \
  dataflowmanager.h \
  enums.h \
  enumtype.h \
  idlist.h \
  node.h \
  nodetype.h \
  resultfilemanager.h \
  scavedefs.h \
  scaveexception.h \
  scaveutils.h \
  statistics.h \
  vectorfilereader.h
$O/vectorfilewriter.o: vectorfilewriter.cc \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h \
  ../../include/omnetpp/platdep/timeutil.h \
  ../common/bigdecimal.h \
  ../common/commondefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/progressmonitor.h \
  ../common/rwlock.h \
  ../common/stringutil.h \
  channel.h \
  dataflowmanager.h \
  enums.h \
  enumtype.h \
  idlist.h \
  node.h \
  nodetype.h \
  resultfilemanager.h \
  scavedefs.h \
  scaveutils.h \
  statistics.h \
  vectorfilewriter.h
$O/windowavg.o: windowavg.cc \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h \
  ../../include/omnetpp/platdep/timeutil.h \
  ../common/bigdecimal.h \
  ../common/commondefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/filereader.h \
  ../common/progressmonitor.h \
  channel.h \
  commonnodes.h \
  dataflowmanager.h \
  node.h \
  nodetype.h \
  scavedefs.h \
  scaveutils.h \
  windowavg.h
$O/xyarray.o: xyarray.cc \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../common/bigdecimal.h \
  ../common/commondefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  scavedefs.h \
  xyarray.h
$O/xyplotnode.o: xyplotnode.cc \
  ../../include/omnetpp/platdep/intlimits.h \
  ../../include/omnetpp/platdep/platdefs.h \
  ../../include/omnetpp/platdep/platmisc.h \
  ../common/bigdecimal.h \
  ../common/commondefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  ../common/filereader.h \
  ../common/progressmonitor.h \
  channel.h \
  commonnodes.h \
  dataflowmanager.h \
  node.h \
  nodetype.h \
  scavedefs.h \
  xyplotnode.h
