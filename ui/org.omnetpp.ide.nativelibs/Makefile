#
# Global definitions
#

CONFIGFILE = ../../Makefile.inc
include $(CONFIGFILE)

O=$(OMNETPP_OUT_DIR)/$(TOOLCHAIN_NAME)-$(MODE)/ui

#
# Local definitions
#
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

OBJS = $O/nedxml.o $O/scave.o $O/layout.o $O/eventlog.o $O/common.o $O/jprogressmonitor.o \
  $O/pickler.o $O/resultspickler.o $O/sharedmemory.o $O/sharedmemory_jni.o $O/shmmanager.o
LIBS= -loppnedxml$D -loppscave$D -lopplayout$D -loppeventlog$D -loppcommon$D

ifeq ("$(JNI_LIB_SUFFIX)",".dll")
# on mingw we dont need the lib prefix for a JNI library
DLL = opplibs$(JNI_LIB_SUFFIX)
# the following is needed for building JNI lib with mingw (name mangling issue)
LDFLAGS+= -Wl,--kill-at
LIBS+= -lversion
else
DLL = $(LIB_PREFIX)opplibs$(JNI_LIB_SUFFIX)
endif

# on Linux, we need to link with this library for the POSIX shm_* functions
ifeq ("$(JNI_LIB_SUFFIX)",".so")
LIBS+= -lrt
endif

# on MAC OS X we should strip only the non-public symbols
ifeq ("$(JNI_LIB_SUFFIX)",".jnilib")
STRIP_FLAGS=-x
endif

DLL_TARGET_DIR = ../org.omnetpp.ide.nativelibs.$(PLATFORM)

#
# Local definitions
#
INCLUDES=-I$(OMNETPP_INCL_DIR) -I$(OMNETPP_SRC_DIR)
COPTS=$(CFLAGS) $(CXXFLAGS) $(JAVA_CFLAGS) $(INCLUDES) -DTHREADED $(PTHREAD_CFLAGS)

#
# Automatic rules
#
.SUFFIXES : .cc .cxx

$O/%.o: %.cc $(CONFIGFILE)
	@mkdir -p $O
	$(CXX) -c $(COPTS) -o $@ $<

$O/%.o: %.cxx $(CONFIGFILE)
	@mkdir -p $O
	$(CXX) -c $(COPTS) -o $@ $<

#
# Targets
#
all: $(JNILIBS_IF_POSSIBLE)

jnilibs: $O/$(DLL) $(DLL_TARGET_DIR)/$(DLL) $(CONFIGFILE)

common.cxx : common.i
	-rm -f $(COMMON_JAVADIR)/*.java
	-mkdir -p $(COMMON_JAVADIR)
	$(SWIG) -c++ -java $(INCLUDES) -package $(COMMON_JAVAPKG) -outdir $(COMMON_JAVADIR) -o $@ $<

nedxml.cxx : nedxml.i
	-rm -f $(NEDXML_JAVADIR)/*.java
	-mkdir -p $(NEDXML_JAVADIR)
	$(SWIG) -c++ -java $(INCLUDES) -package $(NEDXML_JAVAPKG) -outdir $(NEDXML_JAVADIR) -o $@ $<
	$(PERL) -pi.bak -e 's/\(omnetpp::nedxml::([A-Z_]+)\)/(NedTagCode.\1.swigValue())/g;' src/org/omnetpp/ned/engine/MsgTagCode.java

scave.cxx : scave.i idlist.i
	-rm -f $(SCAVE_JAVADIR)/*.java
	-mkdir -p $(SCAVE_JAVADIR)
	$(SWIG) -c++ -java $(INCLUDES) -package $(SCAVE_JAVAPKG) -outdir $(SCAVE_JAVADIR) -o $@ scave.i

layout.cxx : layout.i
	-rm -f $(LAYOUT_JAVADIR)/*.java
	-mkdir -p $(LAYOUT_JAVADIR)
	$(SWIG) -c++ -java $(INCLUDES) -package $(LAYOUT_JAVAPKG) -outdir $(LAYOUT_JAVADIR) -o $@ $<

eventlog.cxx : eventlog.i
	-rm -f $(EVENTLOG_JAVADIR)/*.java
	-mkdir -p $(EVENTLOG_JAVADIR)
	$(SWIG) -c++ -java $(INCLUDES) -package $(EVENTLOG_JAVAPKG) -outdir $(EVENTLOG_JAVADIR) -o $@ $<

$O/$(DLL): $(OBJS)
	$(SHLIB_LD) $(LDFLAGS) -o $O/$(DLL) $(OBJS) $(LIBS) $(PTHREAD_LIBS)
ifeq ("$(MODE)","release")
	$(STRIP) $(STRIP_FLAGS) $O/$(DLL)
endif

# copy files to the bin and lib directories from the out directory
$(DLL_TARGET_DIR)/%: $O/% $(CONFIGFILE)
	@mkdir -p $(DLL_TARGET_DIR)
	@echo Copying to: $@
	$(Q)cp $< $@
	$(Q)$(SHLIB_POSTPROCESS) $@

missing-dependency:
	@echo '  *** Missing required dependency -- skipping build of UI support library.'
	@echo '  *** Install missing packages (probably SWIG and/or JDK), then run ./configure again.'

clean:
	-rm -f *.cxx *.o *.a *.so $(DLL_TARGET_DIR)/$(DLL)
	-rm -rf $(COMMON_JAVADIR) $(NEDXML_JAVADIR) $(SCAVE_JAVADIR) $(LAYOUT_JAVADIR) $(EVENTLOG_JAVADIR)

