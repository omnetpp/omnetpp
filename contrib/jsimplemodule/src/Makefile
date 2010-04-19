#
# OMNeT++/OMNEST Makefile for jsimplemodule SWIG code generation
#
#

# Output directory
PROJECT_OUTPUT_DIR = out
O = $(PROJECT_OUTPUT_DIR)/$(CONFIGNAME)/

#------------------------------------------------------------------------------
# Pull in OMNeT++ configuration (Makefile.inc or configuser.vc)

ifneq ("$(OMNETPP_CONFIGFILE)","")
CONFIGFILE = $(OMNETPP_CONFIGFILE)
else
ifneq ("$(OMNETPP_ROOT)","")
CONFIGFILE = $(OMNETPP_ROOT)/Makefile.inc
else
CONFIGFILE = $(shell opp_configfilepath)
endif
endif

ifeq ("$(wildcard $(CONFIGFILE))","")
$(error Config file '$(CONFIGFILE)' does not exist -- add the OMNeT++ bin directory to the path so that opp_configfilepath can be found, or set the OMNETPP_CONFIGFILE variable to point to Makefile.inc)
endif

include $(CONFIGFILE)

JAVAC=javac
JAR=jar

OUTDIR   = out
JAVAPKG  = org.omnetpp.simkernel
JAVADIR  = $(OUTDIR)/java/$(subst .,/,$(JAVAPKG))
CLASSDIR = $(OUTDIR)/classes
DISTDIR  = ../jsimple

GENERATED_FILES = $(OUTDIR)/simkernel_wrap.cc  $(OUTDIR)/SimkernelJNI_registerNatives.cc

DIST_SRC_FILES = \
    ChangeLog \
    commondefs.i \
    PlainMemoryManagement.i \
    Reflection.i \
    Simkernel.i \
    registernatives.pl \
    innerclasses.h \
    JMessage.cc \
    JMessage.h \
    JSimpleModule.cc \
    JSimpleModule.h \
    JSimpleModule.ned \
    JUtil.cc \
    JUtil.h \
    Makefile.vc \
    Makefile

# the following files are not distributed by default
EXPERIMENTAL_SRC_FILES = \
  memorymgmt_msg_one2one.i \
  patchcastfrom.pl \
  patchownership.pl \
  ProxyObjectMap.java.in

all: dist

$(GENERATED_FILES) : Simkernel.i Makefile $(OMNETPP_ROOT)/include/*.h  # and many oher things
	-mkdir -p $(OUTDIR)/java
	-mkdir -p $(OUTDIR)/java/org
	-mkdir -p $(OUTDIR)/java/org/omnetpp
	-mkdir -p $(OUTDIR)/java/org/omnetpp/simkernel
	-rm $(JAVADIR)/*.java
	$(SWIG) -c++ -java -package $(JAVAPKG) -I$(OMNETPP_INCL_DIR) -I$(OMNETPP_ROOT)/src/envir -I$(OMNETPP_ROOT) -outdir $(JAVADIR) -o $(OUTDIR)/simkernel_wrap.cc Simkernel.i
	$(PERL) -i.bak -pe "s/JNIEXPORT +//g" $(OUTDIR)/simkernel_wrap.cc
	$(PERL) -i.bak -pe "s/JNICALL +//g" $(OUTDIR)/simkernel_wrap.cc
	$(PERL) -i.bak -pe "s/jenv;/jenv; LOG_JNI_CALL();/g" $(OUTDIR)/simkernel_wrap.cc
	$(PERL) registernatives.pl $(JAVADIR)/SimkernelJNI.java $(OUTDIR)
	@# $(PERL) patchownership.pl $(JAVADIR)
	@# $(PERL) patchcastfrom.pl $(JAVADIR)

javafiles:
	@# cp ProxyObjectMap.java.in java/org/omnetpp/simkernel/ProxyObjectMap.java
	find $(JAVADIR) -name '*.java' >listfile.tmp
	-rm -rf $(CLASSDIR)
	mkdir -p $(CLASSDIR)
	$(JAVAC) -sourcepath $(OUTDIR)/java -d $(CLASSDIR) @listfile.tmp
	-rm listfile.tmp

dist: $(GENERATED_FILES) javafiles
	-mkdir $(DISTDIR)
	$(JAR) cvf $(DISTDIR)/simkernel.jar -C $(CLASSDIR) org
	$(JAR) cvf $(DISTDIR)/simkernel-src.jar -C $(OUTDIR)/java org
	$(JAR) cvfM $(DISTDIR)/src.zip $(DIST_SRC_FILES)
	rm -f *_m.*
	cp *.cc $(DISTDIR)
	cp *.h $(DISTDIR)
	cp $(OUTDIR)/*.cc $(DISTDIR)
	cp JSimpleModule.ned $(DISTDIR)

clean:
	-rm -rf $(PROJECT_OUTPUT_DIR)

cleanall: clean
	rm $(DISTDIR)/*.cc $(DISTDIR)/*.h $(DISTDIR)/*.jar $(DISTDIR)/*.zip $(DISTDIR)/*.ned