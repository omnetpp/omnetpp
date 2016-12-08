#
# Definitions to be customized by 'configure'
#

CONFIGFILE = ../../Makefile.inc
include $(CONFIGFILE)

#
# Local definitions
#
LIBNAME=$(LIB_PREFIX)oppscave$D
O=$(OMNETPP_OUT_DIR)/$(TOOLCHAIN_NAME)-$(MODE)/src/scave
L=$(OMNETPP_LIB_DIR)

INCL_FLAGS= -I"$(OMNETPP_INCL_DIR)" -I"$(OMNETPP_SRC_DIR)"

COPTS=$(CFLAGS) $(XML_CFLAGS) $(INCL_FLAGS)
IMPLIBS= -loppcommon$D

ifeq ("$(BUILDING_UILIBS)","yes")
COPTS+= -DTHREADED $(PTHREAD_CFLAGS)
IMPLIBS+= $(PTHREAD_LIBS)
endif

OBJS= $O/arraybuilder.o $O/channel.o $O/commonnodes.o $O/compoundfilter.o \
      $O/dataflowmanager.o $O/datasorter.o $O/diffquot.o \
      $O/filewriter.o $O/filternodes.o $O/customfilter.o $O/stddev.o \
      $O/idlist.o $O/mergernodes.o $O/nodetype.o $O/nodetyperegistry.o \
      $O/omnetppresultfileloader.o $O/sqliteresultfileloader.o \
      $O/resultfilemanager.o $O/slidingwinavg.o \
      $O/vectorfilereader.o $O/vectorfilewriter.o $O/windowavg.o \
      $O/xyplotnode.o $O/indexedvectorfile.o \
      $O/vectorfileindexer.o $O/indexfile.o $O/scaveutils.o \
      $O/scaveexception.o $O/enumtype.o $O/export.o $O/teenode.o \
      $O/indexedvectorfilereader.o $O/xyarray.o $O/fields.o \
      $O/sqlitevectorreader.o $O/vectorreaderbyfiletype.o \
      $O/indexedvectorfilereader2.o $O/sqliteresultfileutils.o

# macro is used in $(EXPORT_DEFINES) with clang/c2 when building a shared lib
EXPORT_MACRO = -DSCAVE_EXPORT

.PHONY: all clean doc build install install_scavetool
#
# Automatic rules
#
.SUFFIXES : .cc

$O/%.o: %.cc $(CONFIGFILE)
	@mkdir -p $O
	$(qecho) "$<"
	$(Q)$(CXX) -c $(CXXFLAGS) $(COPTS) $(EXPORT_DEFINES) $(IMPORT_DEFINES) -o $@ $<

#
# Targets
#
all : install install_scavetool

install: build $(CONFIGFILE)
	@mkdir -p $L
	-$(Q)cp $O/$(LIBNAME)$(A_LIB_SUFFIX) $L 2>/dev/null || :
	-$(Q)cp $O/$(LIBNAME)$(SO_LIB_SUFFIX) $L 2>/dev/null || :
	-$(Q)$(SHLIB_POSTPROCESS) $L/$(LIBNAME)$(SO_LIB_SUFFIX) 2>/dev/null || :
	-$(Q)cp $O/$(LIBNAME)$(DLL_LIB_SUFFIX) $(OMNETPP_BIN_DIR) 2>/dev/null || :

build : $O/$(LIBNAME)$(LIB_SUFFIX)

install_scavetool : $O/scavetool$(EXE_SUFFIX)
	@mkdir -p $(OMNETPP_BIN_DIR)
	$(Q)cp $O/scavetool$(EXE_SUFFIX) $(OMNETPP_BIN_DIR)

$O/scavetool$(EXE_SUFFIX) : scavetool.cc install
	@mkdir -p $O
	@echo Creating executable: $@
	$(Q)$(CXX) $(CXXFLAGS) $(COPTS) $(IMPORT_DEFINES) scavetool.cc -o $@ $(LDFLAGS) -loppscave$D $(IMPLIBS)

$O/$(LIBNAME)$(A_LIB_SUFFIX): $(OBJS)
	@mkdir -p $O
	@echo Creating static library: $@
	$(Q)$(AR) $(ARFLAG_OUT)$O/$(LIBNAME)$(A_LIB_SUFFIX) $(OBJS)
	$(Q)$(RANLIB) $O/$(LIBNAME)$(A_LIB_SUFFIX)

$O/$(LIBNAME)$(SO_LIB_SUFFIX) : $(OBJS)
	@mkdir -p $O
	@echo Creating shared library: $@
	$(Q)$(SHLIB_LD) $(LDFLAGS) -o $O/$(LIBNAME)$(SO_LIB_SUFFIX) $(OBJS) $(IMPLIBS)

$O/$(LIBNAME)$(DLL_LIB_SUFFIX) : $(OBJS)
	@mkdir -p $O
	@echo Creating DLL: $@
	$(Q)$(SHLIB_LD) $(LDFLAGS) -o $O/$(LIBNAME)$(DLL_LIB_SUFFIX) $(OBJS) $(IMPLIBS) $(LDFLAG_IMPLIB)$O/$(LIBNAME)$(A_LIB_SUFFIX)

doc:
	# NOTE: doxy.cfg has the output directory hardcoded!
	$(DOXYGEN) doxy.cfg

clean:
	$(qecho) Cleaning...
	$(Q)rm -rf $O
	$(Q)rm -f $L/$(LIBNAME).* $(OMNETPP_BIN_DIR)/$(LIBNAME).*
	$(Q)rm -f $(OMNETPP_BIN_DIR)/scavetool$(EXE_SUFFIX)

-include $(OBJS:%.o=%.d)
