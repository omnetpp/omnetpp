#
# Definitions to be customized by 'configure'
#

CONFIGFILE = ../../Makefile.inc
include $(CONFIGFILE)

#
# Local definitions
#
LIBNAME=$(LIB_PREFIX)oppqtenv$D
O=$(OMNETPP_OUT_DIR)/$(TOOLCHAIN_NAME)-$(MODE)/src/qtenv
L=$(OMNETPP_LIB_DIR)

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
  DEFINES += -DQT_COMPILING_QSTRING_COMPAT_CPP
endif

ifeq ($(WITH_OSG),yes)
  IMPLIBS += $(OSG_LIBS)
endif

ifeq ($(WITH_OSGEARTH),yes)
  IMPLIBS += $(OSGEARTH_LIBS)
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
GENERATED_FILES=$(UI_OUT) $(MOC_OUT) $(RESOURCES_OUT)

OBJS=$(SOURCES:%.cc=$O/%.o) $(MOC_OUT:%.cpp=$O/%.o) $(RESOURCES_OUT:%.cpp=$O/%.o)

# macro is used in $(EXPORT_DEFINES) with clang/c2 when building a shared lib
EXPORT_MACRO = -DQTENV_EXPORT

.PHONY: all clean qtenv qmake skip build install
.SECONDARY:

#
# Automatic rules
#
.SUFFIXES : .cc .cpp .ui .qrc .h

$O/%.o: %.cc $(CONFIGFILE) | $(GENERATED_FILES)
	@mkdir -p $O
	$(qecho) "$<"
	$(Q)$(CXX) -c $(CXXFLAGS) $(CFLAGS) $(EXPORT_DEFINES) $(IMPORT_DEFINES) -o $@ $<

$O/%.o: %.cpp $(CONFIGFILE) | $(GENERATED_FILES)
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
all: $(QTENV_TARGET)

qtenv : install

install: build $(CONFIGFILE)
	@mkdir -p $L
	-$(Q)cp $O/$(LIBNAME)$(A_LIB_SUFFIX) $L 2>/dev/null || :
	-$(Q)cp $O/$(LIBNAME)$(SO_LIB_SUFFIX) $L 2>/dev/null || :
	-$(Q)$(SHLIB_POSTPROCESS) $L/$(LIBNAME)$(SO_LIB_SUFFIX) 2>/dev/null || :
	-$(Q)cp $O/$(LIBNAME)$(DLL_LIB_SUFFIX) $(OMNETPP_BIN_DIR) 2>/dev/null || :

build : $O/$(LIBNAME)$(LIB_SUFFIX)

$O/$(LIBNAME)$(A_LIB_SUFFIX): $(OBJS)
	@mkdir -p $O
	@echo Creating static library: $@
	$(Q)$(AR) $(ARFLAG_OUT)$O/$(LIBNAME)$(A_LIB_SUFFIX) $(O)/*.o
	$(Q)$(RANLIB) $O/$(LIBNAME)$(A_LIB_SUFFIX)

$O/$(LIBNAME)$(SO_LIB_SUFFIX): $(OBJS)
	@mkdir -p $O
	@echo Creating shared library: $@
	$(Q)$(SHLIB_LD) $(LDFLAGS) -o $O/$(LIBNAME)$(SO_LIB_SUFFIX) $(O)/*.o $(IMPLIBS)

$O/$(LIBNAME)$(DLL_LIB_SUFFIX): $(OBJS)
	@mkdir -p $O
	@echo Creating DLL: $@
	$(Q)$(SHLIB_LD) $(LDFLAGS) -o $O/$(LIBNAME)$(DLL_LIB_SUFFIX) $(O)/*.o $(IMPLIBS) $(LDFLAG_IMPLIB)$O/$(LIBNAME)$(A_LIB_SUFFIX)

clean:
	$(qecho) Cleaning...
	$(Q)rm -f $(GENERATED_FILES)
	$(Q)rm -rf $O
	$(Q)rm -f $L/$(LIBNAME).* $(OMNETPP_BIN_DIR)/$(LIBNAME).*

skip:
	@echo '  *** WITH_QTENV=no -- skipping Qtenv.'

-include $(OBJS:%.o=%.d)
