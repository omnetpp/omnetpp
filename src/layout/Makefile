#
# Definitions to be customized by 'configure'
#

CONFIGFILE = ../../Makefile.inc
include $(CONFIGFILE)

#
# Local definitions
#
LIBNAME=$(LIB_PREFIX)opplayout$D
O=$(OMNETPP_OUT_DIR)/$(TOOLCHAIN_NAME)-$(MODE)/src/layout
L=$(OMNETPP_LIB_DIR)

INCL_FLAGS= -I"$(OMNETPP_INCL_DIR)" -I"$(OMNETPP_SRC_DIR)"

COPTS=$(CFLAGS) $(INCL_FLAGS)

IMPLIBS= -loppcommon$D

OBJS= $O/geometry.o $O/graphcomponent.o $O/heapembedding.o $O/startreeembedding.o \
      $O/forcedirectedparametersbase.o $O/forcedirectedparameters.o $O/forcedirectedembedding.o \
      $O/graphlayouter.o $O/basicspringembedderlayout.o $O/forcedirectedgraphlayouter.o

# macro is used in $(EXPORT_DEFINES) with clang/c2 when building a shared lib
EXPORT_MACRO = -DLAYOUT_EXPORT

.PHONY: all clean build install
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
all : install

install: build $(CONFIGFILE)
	@mkdir -p $L
	-$(Q)cp $O/$(LIBNAME)$(A_LIB_SUFFIX) $L 2>/dev/null || :
	-$(Q)cp $O/$(LIBNAME)$(SO_LIB_SUFFIX) $L 2>/dev/null || :
	-$(Q)$(SHLIB_POSTPROCESS) $L/$(LIBNAME)$(SO_LIB_SUFFIX) 2>/dev/null || :
	-$(Q)cp $O/$(LIBNAME)$(DLL_LIB_SUFFIX) $(OMNETPP_BIN_DIR) 2>/dev/null || :
ifeq ($(TOOLCHAIN_NAME),clangc2)
ifeq ($(MODE),debug)
	-$(Q)cp $O/$(LIBNAME).pdb $(OMNETPP_BIN_DIR) 2>/dev/null || :
endif
endif

build : $O/$(LIBNAME)$(LIB_SUFFIX)

$O/$(LIBNAME)$(A_LIB_SUFFIX) : $(OBJS)
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

clean:
	$(qecho) Cleaning...
	$(Q)rm -rf $O
	$(Q)rm -f $L/$(LIBNAME).* $(OMNETPP_BIN_DIR)/$(LIBNAME).*

-include $(OBJS:%.o=%.d)
