#
# Global definitions
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

L=$(OMNETPP_LIB_DIR)/$(TOOLCHAIN_NAME)
O=$(OMNETPP_OUT_DIR)/$(TOOLCHAIN_NAME)-$(MODE)/ui

#
# Local definitions
#
COMMON=$(OMNETPP_ROOT)/src/common
NEDXML=$(OMNETPP_ROOT)/src/nedxml
SCAVE=$(OMNETPP_ROOT)/src/scave
LAYOUT=$(OMNETPP_ROOT)/src/layout
EVENTLOG=$(OMNETPP_ROOT)/src/eventlog

COMMON_JAVAPKG = org.omnetpp.common.engine
NEDXML_JAVAPKG = org.omnetpp.ned.engine
SCAVE_JAVAPKG = org.omnetpp.scave.engine
LAYOUT_JAVAPKG = org.omnetpp.layout.engine
EVENTLOG_JAVAPKG = org.omnetpp.eventlog.engine

COMMON_JAVADIR = src/$(subst .,/,$(COMMON_JAVAPKG))
NEDXML_JAVADIR = src/$(subst .,/,$(NEDXML_JAVAPKG))
SCAVE_JAVADIR = src/$(subst .,/,$(SCAVE_JAVAPKG))
LAYOUT_JAVADIR = src/$(subst .,/,$(LAYOUT_JAVAPKG))
EVENTLOG_JAVADIR = src/$(subst .,/,$(EVENTLOG_JAVAPKG))

OBJS = $O/nedxml.o $O/scave.o $O/layout.o $O/eventlog.o $O/common.o $O/jprogressmonitor.o
LIBS=-L$L -lnedxml$D -lscave$D -llayout$D -leventlog$D -lcommon$D

ifeq ("$(JNI_LIB_SUFFIX)",".dll")
# on windows we dont need the lib prefix for a JNI library 
DLL = opplibs$(JNI_LIB_SUFFIX)
else 
DLL = $(LIB_PREFIX)opplibs$(JNI_LIB_SUFFIX)
endif
DLL_TARGET_DIR = ../org.omnetpp.ide.nativelibs.$(ECLIPSE_PLATFORM)

#
# Local definitions
#
INCLUDES=-I$(COMMON) -I$(NEDXML) -I$(SCAVE) -I$(LAYOUT) -I$(EVENTLOG) -I$(OMNETPP_INCL_DIR)/platdep
COPTS=$(CFLAGS) $(JAVA_CFLAGS) $(INCLUDES)


#
# Automatic rules
#
.SUFFIXES : .cc .cxx

$O/%.o: %.cc
	@mkdir -p $O
	$(CXX) -c $(COPTS) -o $@ $<

$O/%.o: %.cxx
	@mkdir -p $O
	$(CXX) -c $(COPTS) -o $@ $<

#
# Targets
#
all: $(JNILIBS_IF_POSSIBLE)

jnilibs: $O/$(DLL) $(DLL_TARGET_DIR)/$(DLL)

common.cxx : common.i
	-rm -f $(COMMON_JAVADIR)/*.java
	-mkdir -p $(COMMON_JAVADIR)
	$(SWIG) -c++ -java $(INCLUDES) -package $(COMMON_JAVAPKG) -outdir $(COMMON_JAVADIR) -o $@ $<

nedxml.cxx : nedxml.i
	-rm -f $(NEDXML_JAVADIR)/*.java
	-mkdir -p $(NEDXML_JAVADIR)
	$(SWIG) -c++ -java $(INCLUDES) -package $(NEDXML_JAVAPKG) -outdir $(NEDXML_JAVADIR) -o $@ $<

scave.cxx : scave.i
	-rm -f $(SCAVE_JAVADIR)/*.java
	-mkdir -p $(SCAVE_JAVADIR)
	$(SWIG) -c++ -java $(INCLUDES) -package $(SCAVE_JAVAPKG) -outdir $(SCAVE_JAVADIR) -o $@ $<

layout.cxx : layout.i
	-rm -f $(LAYOUT_JAVADIR)/*.java
	-mkdir -p $(LAYOUT_JAVADIR)
	$(SWIG) -c++ -java $(INCLUDES) -package $(LAYOUT_JAVAPKG) -outdir $(LAYOUT_JAVADIR) -o $@ $<

eventlog.cxx : eventlog.i
	-rm -f $(EVENTLOG_JAVADIR)/*.java
	-mkdir -p $(EVENTLOG_JAVADIR)
	$(SWIG) -c++ -java $(INCLUDES) -package $(EVENTLOG_JAVAPKG) -outdir $(EVENTLOG_JAVADIR) -o $@ $<

$O/$(DLL): $(OBJS)
	$(SHLIB_LD) -o $O/$(DLL) $(OBJS) $(LIBS) $(LDFLAGS)

$(DLL_TARGET_DIR)/$(DLL): $O/$(DLL)
	cp $O/$(DLL) $(DLL_TARGET_DIR)


missing-dependency:
	@echo '  *** Missing required dependency -- skipping build of UI support library.'
	@echo '  *** Install missing packages (probably SWIG and/or JDK), then run ./configure again.'

depend:
	$(MAKEDEPEND) $(INCLUDES) -fMakefile -- *.cxx *.cc

clean:
	-rm -f *.cxx *.o *.a *.so $(DLL_TARGET_DIR)/*.dll $(DLL_TARGET_DIR)/*.so $(DLL_TARGET_DIR)/*.jnilib

# DO NOT DELETE THIS LINE -- make depend depends on it.

