#
# Global definitions
#

include ../../Makefile.inc

ifeq ("$(strip $(CONFIGNAME))",".")
L=$(OMNETPP_LIB_DIR)
else
L=$(OMNETPP_LIB_DIR)/$(CONFIGNAME)
endif

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

SWIGFILES = common.i nedxml.i scave.i layout.i eventlog.i
CXXFILES = $(SWIGFILES:.i=.cxx)
OBJS = $(CXXFILES:.cxx=.o) jprogressmonitor.o

LIBS=-L$L -lcommon -lnedxml -lscave -llayout -leventlog

DLL = libopplibs$(SO_LIB_SUFFIX)

DLL_TARGET_DIR = ../org.omnetpp.ide.nativelibs.linux.x86


#
# Local definitions
#
INCLUDES=-I$(COMMON) -I$(NEDXML) -I$(SCAVE) -I$(LAYOUT) -I$(EVENTLOG) -I$(OMNETPP_INCL_DIR)/platdep
COPTS=$(CFLAGS) $(JAVA_CFLAGS) $(INCLUDES)


#
# Automatic rules
#
.SUFFIXES : .cc .cxx

.cc.o:
	$(CXX) -c $(COPTS) $<

.cxx.o:
	$(CXX) -c $(COPTS) $<

#
# Targets
#
all: $(JNILIBS_IF_POSSIBLE)

jnilibs: $(DLL)

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

$(DLL): $(OBJS)
	$(SHLIB_LD) -L$(OMNETPP_LIB_DIR) -leventlog -llayout -lscave -lnedxml -lcommon -o $(DLL) $(OBJS)
	cp $(DLL) $(DLL_TARGET_DIR)

missing-dependency:
	@echo '  *** Missing required dependency -- skipping build of UI support library.'
	@echo '  *** Install missing packages (probably SWIG and/or JDK), then run ./configure again.'

depend:
	$(MAKEDEPEND) $(INCLUDES) -fMakefile -- *.cxx *.cc

clean:
	-rm -f *.cxx *.o *.a *.so

# DO NOT DELETE THIS LINE -- make depend depends on it.

