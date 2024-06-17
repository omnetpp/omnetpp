#
# Definitions to be customized by 'configure'
#

CONFIGFILE = ../../../Makefile.inc
include $(CONFIGFILE)

#
# Local definitions
#
LIBNAME=scave_bindings$D
O=$(OMNETPP_OUT_DIR)/$(CONFIGNAME)/src/scave/python
SCAVE_PYTHON_DIR=$(OMNETPP_PYTHON_DIR)/omnetpp/scave
TARGET_LIB_FILES=$(SCAVE_PYTHON_DIR)/$(LIBNAME)$(PYTHON_EXT_SUFFIX)

CFLAGS += -DOMNETPP_MODE_SUFFIX=$D

NANOBIND_DIR=$(OMNETPP_SRC_DIR)/scave/python/3rdparty/nanobind
NBO=$(O)/3rdparty/nanobind
NANOBIND_OBJS=$(NBO)/common.o $(NBO)/implicit.o $(NBO)/nb_func.o $(NBO)/nb_ndarray.o \
    $(NBO)/nb_static_property.o $(NBO)/trampoline.o $(NBO)/error.o $(NBO)/nb_enum.o \
    $(NBO)/nb_internals.o $(NBO)/nb_type.o


INCL_FLAGS=-I"$(OMNETPP_INCL_DIR)" -I"$(OMNETPP_SRC_DIR)" \
    -I"$(NANOBIND_DIR)/ext/robin_map/include" -I"$(NANOBIND_DIR)/include"

COPTS=$(CFLAGS) $(PYTHON_CFLAGS) $(INCL_FLAGS)

IMPLIBS=-loppscave$D -loppcommon$D

OBJS=$O/$(LIBNAME).o

# we want to recompile everything if COPTS changes,
# so we store COPTS into $COPTS_FILE (if COPTS has changed since last build)
# and make the object files depend on it
COPTS_FILE=$O/.last-copts
ifneq ("$(COPTS)","$(shell cat $(COPTS_FILE) 2>/dev/null || echo '')")
  $(shell $(MKPATH) "$O")
  $(file >$(COPTS_FILE),$(COPTS))
endif

.PHONY: all clean
.SECONDARY: $(OBJS) $(NANOBIND_OBJS) $O/$(LIBNAME)$(PYTHON_EXT_SUFFIX)

# disabling all implicit rules
.SUFFIXES:

#
# Targets
#
all: $(TARGET_LIB_FILES)

#
# Automatic rules
#

$O/%$D.o: %.cc $(CONFIGFILE) $(COPTS_FILE)
	@mkdir -p $O
	$(qecho) "$<"
	$(Q)$(CXX) -c $(CXXFLAGS) $(COPTS) $(EXPORT_DEFINES) $(IMPORT_DEFINES) -o $@ $<

$(NBO)/%.o: $(NANOBIND_DIR)/src/%.cpp $(CONFIGFILE) $(COPTS_FILE)
	@mkdir -p $(NBO)
	$(qecho) "$<"
	$(Q)$(CXX) -c $(CXXFLAGS) $(COPTS) $(EXPORT_DEFINES) $(IMPORT_DEFINES) -o $@ $<

# dynamically linked library (on all platforms)
$O/%$(PYTHON_EXT_SUFFIX) : $(OBJS) $(NANOBIND_OBJS)
	@mkdir -p $O
	@echo Creating shared library: $@
	$(Q)$(SHLIB_LD) $(LDFLAGS) -o $O/$(LIBNAME)$(PYTHON_EXT_SUFFIX) \
	    $(OBJS) $(NANOBIND_OBJS) $(IMPLIBS) $(PYTHON_LDFLAGS)

# copy the module to the python directory from the out directory
$(SCAVE_PYTHON_DIR)/%: $O/% $(CONFIGFILE)
	@echo Copying to: $@
	$(Q)cp $< $@
	$(Q)$(SHLIB_POSTPROCESS) $@
	$(Q)cp $(SCAVE_PYTHON_DIR)/scave_bindings.pyi.dist $(SCAVE_PYTHON_DIR)/scave_bindings$D.pyi

clean:
	$(qecho) Cleaning scave/python
	$(Q)rm -rf $O $(NBO) $(SCAVE_PYTHON_DIR)/*$(PYTHON_EXT_SUFFIX) $(SCAVE_PYTHON_DIR)/*.pyi

-include $(OBJS:%=%.d)
-include $(NANOBIND_OBJS:%=%.d)
