#
# Definitions to be customized by 'configure'
#

CONFIGFILE = ../../Makefile.inc
include $(CONFIGFILE)

#
# Local definitions
#
LIBNAME=$(LIB_PREFIX)opptkenv$D
TARGET_LIB_FILES=$(OMNETPP_LIB_DIR)/$(LIBNAME)$(LIB_SUFFIX)
# On Windows, the targets are different (DLLs go into the bin dir and separate export libs are created)
ifeq ($(PLATFORM),win32.x86_64)
  ifeq ($(SHARED_LIBS),yes)
    TARGET_LIB_FILES=$(OMNETPP_BIN_DIR)/$(LIBNAME)$(DLL_LIB_SUFFIX) $(OMNETPP_LIB_DIR)/$(LIBNAME)$(A_LIB_SUFFIX)
    ifeq ($(TOOLCHAIN_NAME),clangc2)
      ifeq ($(MODE),debug)
        TARGET_LIB_FILES+=$(OMNETPP_BIN_DIR)/$(LIBNAME).pdb
      endif
    endif
  endif
endif

TKPATH_FILES=$(OMNETPP_LIB_DIR)/tkpath
TKDOCK_FILES=$(OMNETPP_LIB_DIR)/tkdock

O=$(OMNETPP_OUT_DIR)/$(TOOLCHAIN_NAME)-$(MODE)/src/tkenv

INCL_FLAGS= -I"$(OMNETPP_INCL_DIR)" -I"$(OMNETPP_SRC_DIR)"

COPTS=$(CFLAGS) $(INCL_FLAGS) $(TK_CFLAGS) $(ZLIB_CFLAGS)

IMPLIBS= -loppsim$D -loppenvir$D -lopplayout$D -loppcommon$D $(TK_LIBS) $(ZLIB_LIBS)

OBJS = $O/tkenv$(TKENV_EMBED_SUFFIX).o $O/tkcmd.o $O/tklib.o $O/inspector.o $O/logbuffer.o \
       $O/inspectorfactory.o $O/genericobjectinspector.o $O/watchinspector.o $O/canvasinspector.o \
       $O/moduleinspector.o $O/loginspector.o $O/gateinspector.o $O/figurerenderers.o \
       $O/outputvectorinspector.o $O/histograminspector.o $O/componenthistory.o \
       $O/canvasrenderer.o $O/layouterenv.o $O/arrow.o \
       $O/tkutil.o $O/tkImgPNG.o $O/tkImgPNGInit.o

GENERATED_SOURCES=tclcode.cc

ifeq ("$(EMBED_TCL_CODE)","yes")
  TKENV_EMBED_SUFFIX=-e
else
  TKENV_EMBED_SUFFIX=
endif

# Required only on Mac OS X
ifeq ("$(PLATFORM)", "macosx")
  CFLAGS += -Wno-deprecated-declarations
  LDFLAGS += -framework ApplicationServices
endif

# macro is used in $(EXPORT_DEFINES) with clang/c2 when building a shared lib
EXPORT_MACRO = -DTKENV_EXPORT

.PHONY : all clean
.SECONDARY: $(OBJS) $O/$(LIBNAME)$(LIB_SUFFIX) $(GENERATED_SOURCES)

#
# Automatic rules
#
.SUFFIXES : .cc .c

$O/%.o: %.c $(CONFIGFILE)
	@mkdir -p $O
	$(qecho) "$<"
	$(Q)$(CC) -c $(COPTS) $(EXPORT_DEFINES) $(IMPORT_DEFINES) -o $@ $<

$O/%.o: %.cc $(CONFIGFILE)
	@mkdir -p $O
	$(qecho) "$<"
	$(Q)$(CXX) -c $(CXXFLAGS) $(COPTS) $(EXPORT_DEFINES) $(IMPORT_DEFINES) -o $@ $<

#
# Targets
#
all : $(TARGET_LIB_FILES) $(TKPATH_FILES) $(TKDOCK_FILES)

ifeq ($(SHARED_LIBS),yes)
# dynamically linked library (on all platforms except Windows)
$O/%$(SO_LIB_SUFFIX) : $(OBJS)
	@mkdir -p $O
	@echo Creating shared library: $@
	$(Q)$(SHLIB_LD) $(LDFLAGS) -o $O/$(LIBNAME)$(SO_LIB_SUFFIX) $(OBJS) $(IMPLIBS)

# dynamically linked library + import lib (on Windows)
$O/%$(DLL_LIB_SUFFIX) $O/%$(A_LIB_SUFFIX): $(OBJS)
	@mkdir -p $O
	@echo Creating DLL and import library: $@
	$(Q)$(SHLIB_LD) $(LDFLAGS) -o $O/$(LIBNAME)$(DLL_LIB_SUFFIX) $(OBJS) $(IMPLIBS) $(LDFLAG_IMPLIB)$O/$(LIBNAME)$(A_LIB_SUFFIX)
else
# statically linked library (on all platforms)
$O/%$(A_LIB_SUFFIX) : $(OBJS)
	@mkdir -p $O
	@echo Creating static library: $@
	$(Q)$(AR) $(ARFLAG_OUT)$O/$(LIBNAME)$(A_LIB_SUFFIX) $(OBJS)
	$(Q)$(RANLIB) $O/$(LIBNAME)$(A_LIB_SUFFIX)
endif

# copy files to the bin and lib directories from the out directory
$(OMNETPP_BIN_DIR)/% $(OMNETPP_LIB_DIR)/%: $O/% $(CONFIGFILE)
	@mkdir -p $(OMNETPP_BIN_DIR) $(OMNETPP_LIB_DIR)
	@echo Copying to: $@
	$(Q)cp $< $@
	$(Q)$(SHLIB_POSTPROCESS) $@

$O/$(LIBNAME).pdb: $O/$(LIBNAME)$(DLL_LIB_SUFFIX)

clean:
	$(qecho) Cleaning tkenv
	$(Q)rm -rf $O $(GENERATED_SOURCES) $(TARGET_LIB_FILES) $(TKPATH_FILES) $(TKDOCK_FILES)

$(TKPATH_FILES):
	@echo Copying Tkpath extensions
	$(Q)cp -R tkpath $(OMNETPP_LIB_DIR)

$(TKDOCK_FILES):
ifeq ("$(PLATFORM)", "macosx")
	@echo Copying Tkdock extensions
	$(Q)cp -R tkdock $(OMNETPP_LIB_DIR)
endif

$O/tkenv.o : tkenv.cc
	@mkdir -p $O
	$(qecho) "$<"
	$(Q)$(CXX) -c $(CXXFLAGS) $(COPTS) $(EXPORT_DEFINES) $(IMPORT_DEFINES) -DOMNETPP_LIB_DIR="\"$(OMNETPP_LIB_DIR)\"" -DOMNETPP_TKENV_DIR="\"$(OMNETPP_TKENV_DIR)\"" tkenv.cc -o $O/tkenv.o

$O/tkenv-e.o : tkenv.cc tclcode.cc
	@mkdir -p $O
	$(qecho) "$<"
	$(Q)$(CXX) -c $(CXXFLAGS) $(COPTS) $(EXPORT_DEFINES) $(IMPORT_DEFINES) -DOMNETPP_LIB_DIR="\"$(OMNETPP_LIB_DIR)\"" tkenv.cc -o $O/tkenv-e.o

tclcode.cc: *.tcl tcl2c.pl
	@# clearlooksimg.tcl must precede clearlooks.tcl (tcl2c filters out duplicates)
	$(PERL) tcl2c.pl tclcode.cc clearlooksimg.tcl *.tcl

-include $(OBJS:%.o=%.d)
