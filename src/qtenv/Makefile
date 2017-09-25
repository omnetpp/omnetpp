#
# Definitions to be customized by 'configure'
#

CONFIGFILE = ../../Makefile.inc
include $(CONFIGFILE)

#
# Local definitions
#
LIBNAME=$(LIB_PREFIX)oppqtenv$D
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

O=$(OMNETPP_OUT_DIR)/$(TOOLCHAIN_NAME)-$(MODE)/src/qtenv

# disable warnings for cleaner output on MinGW/clang
CFLAGS += -I"$(OMNETPP_INCL_DIR)" -I"$(OMNETPP_SRC_DIR)" $(QT_CFLAGS) -Wno-ignored-attributes -Wno-inconsistent-missing-override
DEFINES += -DUNICODE -DQT_NO_EMIT -DQT_OPENGL_LIB -DQT_PRINTSUPPORT_LIB -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB
IMPLIBS = -loppsim$D -loppenvir$D -lopplayout$D -loppcommon$D $(QT_LIBS)

# Required only on Mac OS X
ifeq ("$(PLATFORM)", "macosx")
  CFLAGS += -Wno-deprecated-declarations
  LDFLAGS += -framework ApplicationServices
endif

ifeq ($(MODE),release)
  DEFINES += -DQT_NO_DEBUG_OUTPUT
endif

ifeq ($(TOOLCHAIN_NAME),clangc2)
# QT_COMPILING_QSTRING_COMPAT_CPP should be added only for clang-c2 compiler
  DEFINES += -DQT_COMPILING_QSTRING_COMPAT_CPP -DQT_COMPILING_QIMAGE_COMPAT_CPP
endif

#
# local tools
#
SOURCES=$(wildcard *.cc)
UI_SRC=$(wildcard *.ui)
UI_OUT=$(UI_SRC:%.ui=ui_%.h)
MOC_SRC=$(filter-out ui_%.h, $(wildcard *.h))
MOC_OUT=$(MOC_SRC:%.h=moc_%.cpp)
RESOURCES_SRC=$(wildcard *.qrc)
RESOURCES_OUT=$(RESOURCES_SRC:%.qrc=qrc_%.cpp)
GENERATED_SOURCES=$(UI_OUT) $(MOC_OUT) $(RESOURCES_OUT)
OBJS=$(SOURCES:%.cc=$O/%.o) $(MOC_OUT:%.cpp=$O/%.o) $(RESOURCES_OUT:%.cpp=$O/%.o)

# macro is used in $(EXPORT_DEFINES) with clang/c2 when building a shared lib
EXPORT_MACRO = -DQTENV_EXPORT

.PHONY: all clean qtenv-osg
.SECONDARY: $(OBJS) $O/$(LIBNAME)$(LIB_SUFFIX) $(GENERATED_SOURCES)

#
# Automatic rules
#
.SUFFIXES : .cc .cpp .ui .qrc .h

$O/%.o: %.cc $(CONFIGFILE) $(GENERATED_SOURCES)
	@mkdir -p $O
	$(qecho) "$<"
	$(Q)$(CXX) -c $(CXXFLAGS) $(CFLAGS) $(EXPORT_DEFINES) $(IMPORT_DEFINES) -o $@ $<

$O/%.o: %.cpp $(CONFIGFILE) $(GENERATED_SOURCES)
	@mkdir -p $O
	$(qecho) "$<"
	$(Q)$(CXX) -c $(CXXFLAGS) $(CFLAGS) $(EXPORT_DEFINES) $(IMPORT_DEFINES) -o $@ $<

qrc_%.cpp: %.qrc
	$(Q)$(RCC) -name $(basename $<) -o $@ $<

ui_%.h: %.ui
	$(Q)$(UIC) -o $@ $<

moc_%.cpp: %.h
	$(Q)$(MOC) --no-notes $(DEFINES) -o $@ $<

#
# Targets
#
all : $(TARGET_LIB_FILES) 

ifeq ($(WITH_OSG),yes)
all: qtenv-osg
qtenv-osg: $(TARGET_LIB_FILES)
	$(Q)$(MAKE) -C osg
endif

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
	$(qecho) Cleaning qtenv
	$(Q)rm -rf $O $(GENERATED_SOURCES) $(TARGET_LIB_FILES)
	$(Q)$(MAKE) clean -C osg

$(RESOURCES_OUT): $(wildcard icons/tools/* icons/logo/* icons/objects/*)

-include $(OBJS:%.o=%.d)
