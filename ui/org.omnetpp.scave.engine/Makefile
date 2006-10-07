
# Global definitions
#

#include omnetppconfig

JDK=/opt/java/jdk1.5.0_04
SWIG=swig
CFLAGS=-g -Wall

#
# Local definitions
#
COPTS=$(CFLAGS) -I$(JDK)/include

ENGINE_OBJS =  engine/idlist.o \
               engine/arraybuilder.o \
               engine/channel.o \
               engine/commonnodes.o \
               engine/compoundfilter.o \
               engine/dataflowmanager.o \
               engine/datasorter.o \
               engine/diffquot.o \
               engine/exception.o \
               engine/filetokenizer.o \
               engine/filewriter.o \
               engine/filternodes.o \
               engine/mergernodes.o \
               engine/nodetype.o \
               engine/nodetyperegistry.o \
               engine/patmatch.o \
               engine/resultfilemanager.o \
               engine/slidingwinavg.o \
               engine/util.o \
               engine/cstrtokenizer.o \
               engine/vectorfilereader.o \
               engine/vectorfilewriter.o \
               engine/windowavg.o \
               engine/xyplotnode.o

TESTER_OBJS = test1.o test2.o test3.o

WRAPPER_OBJS = engine_wrap.o

OBJS = $(WRAPPER_OBJS) $(ENGINE_OBJS)

HEADERS = engine/*.h

#
# Automatic rules
#
.SUFFIXES : .cc .cxx

.cc.o:
	$(CXX) $(COPTS) -c $< -o $@

.cxx.o:
	$(CXX) $(COPTS) -c $< -o $@


#
# Targets
#
#default: scave_engine.so
default: tester

engine_wrap.cxx: engine.i plove.i $(HEADERS)
	-rm -f java/*.java
	$(SWIG) -c++ -java -package com.omnest.scave.engine -outdir java engine.i

scave_engine.so : $(OBJS)
	$(SHLIB_LD) -o scave_engine.so $(OBJS) $(LDFLAGS)

tester: $(ENGINE_OBJS) test3.o
	$(CXX) -o tester $^

depend:
	$(MAKEDEPEND) -I../../include -fMakefile -- *.cc engine/*.cc

clean:
	-rm -r engine_wrap.cxx *.o engine/*.o test scave_engine.so

# DO NOT DELETE THIS LINE -- make depend depends on it.
test1.o: test1.cc \
  engine/resultfilemanager.h \
  engine/datasorter.h \
  engine/exception.h \
  engine/util.h \
  engine/patmatch.h
test2.o: test2.cc \
  engine/node.h \
  engine/commonnodes.h \
  engine/dataflowmanager.h \
  engine/vectorfilereader.h \
  engine/util.h \
  engine/exception.h \
  engine/nodetype.h \
  engine/channel.h \
  engine/filetokenizer.h
engine/arraybuilder.o: engine/arraybuilder.cc \
  engine/channel.h \
  engine/arraybuilder.h \
  engine/util.h \
  engine/node.h \
  engine/exception.h \
  engine/commonnodes.h \
  engine/xyarray.h \
  engine/nodetype.h \
  engine/dataflowmanager.h
engine/channel.o: engine/channel.cc \
  engine/channel.h \
  engine/util.h \
  engine/node.h \
  engine/exception.h
engine/commonnodes.o: engine/commonnodes.cc \
  engine/commonnodes.h \
  engine/channel.h \
  engine/node.h \
  engine/nodetype.h \
  engine/util.h \
  engine/exception.h \
  engine/dataflowmanager.h
engine/compoundfilter.o: engine/compoundfilter.cc \
  engine/util.h \
  engine/compoundfilter.h \
  engine/filternodes.h \
  engine/nodetyperegistry.h \
  engine/exception.h \
  engine/nodetype.h \
  engine/commonnodes.h \
  engine/dataflowmanager.h \
  engine/node.h \
  engine/channel.h
engine/dataflowmanager.o: engine/dataflowmanager.cc \
  engine/channel.h \
  engine/nodetype.h \
  engine/dataflowmanager.h \
  engine/util.h \
  engine/node.h \
  engine/exception.h
engine/datasorter.o: engine/datasorter.cc \
  engine/resultfilemanager.h \
  engine/util.h \
  engine/datasorter.h \
  engine/exception.h \
  engine/patmatch.h
engine/diffquot.o: engine/diffquot.cc \
  engine/channel.h \
  engine/diffquot.h \
  engine/util.h \
  engine/node.h \
  engine/exception.h \
  engine/commonnodes.h \
  engine/nodetype.h \
  engine/dataflowmanager.h
engine/exception.o: engine/exception.cc \
  engine/exception.h
engine/filetokenizer.o: engine/filetokenizer.cc \
  engine/filetokenizer.h
engine/filewriter.o: engine/filewriter.cc \
  engine/channel.h \
  engine/filewriter.h \
  engine/util.h \
  engine/node.h \
  engine/exception.h \
  engine/commonnodes.h \
  engine/nodetype.h \
  engine/dataflowmanager.h
engine/filternodes.o: engine/filternodes.cc \
  engine/channel.h \
  engine/filternodes.h \
  engine/util.h \
  engine/node.h \
  engine/exception.h \
  engine/commonnodes.h \
  engine/nodetype.h \
  engine/dataflowmanager.h
engine/mergernodes.o: engine/mergernodes.cc \
  engine/channel.h \
  engine/mergernodes.h \
  engine/util.h \
  engine/node.h \
  engine/exception.h \
  engine/commonnodes.h \
  engine/nodetype.h \
  engine/dataflowmanager.h
engine/nodetype.o: engine/nodetype.cc \
  engine/util.h \
  engine/nodetype.h \
  engine/exception.h \
  engine/dataflowmanager.h \
  engine/node.h \
  engine/channel.h
engine/nodetyperegistry.o: engine/nodetyperegistry.cc \
  engine/util.h \
  engine/nodetyperegistry.h \
  engine/arraybuilder.h \
  engine/vectorfilereader.h \
  engine/vectorfilewriter.h \
  engine/filewriter.h \
  engine/windowavg.h \
  engine/slidingwinavg.h \
  engine/filternodes.h \
  engine/mergernodes.h \
  engine/xyplotnode.h \
  engine/diffquot.h \
  engine/exception.h \
  engine/commonnodes.h \
  engine/xyarray.h \
  engine/node.h \
  engine/nodetype.h \
  engine/dataflowmanager.h \
  engine/channel.h \
  engine/filetokenizer.h
engine/patmatch.o: engine/patmatch.cc \
  engine/patmatch.h
engine/resultfilemanager.o: engine/resultfilemanager.cc \
  engine/patmatch.h \
  engine/resultfilemanager.h \
  engine/filetokenizer.h \
  engine/exception.h
engine/slidingwinavg.o: engine/slidingwinavg.cc \
  engine/channel.h \
  engine/slidingwinavg.h \
  engine/util.h \
  engine/node.h \
  engine/exception.h \
  engine/commonnodes.h \
  engine/nodetype.h \
  engine/dataflowmanager.h
engine/util.o: engine/util.cc \
  engine/util.h \
  engine/exception.h
engine/vectorfilereader.o: engine/vectorfilereader.cc \
  engine/channel.h \
  engine/vectorfilereader.h \
  engine/util.h \
  engine/node.h \
  engine/exception.h \
  engine/nodetype.h \
  engine/filetokenizer.h \
  engine/dataflowmanager.h
engine/vectorfilewriter.o: engine/vectorfilewriter.cc \
  engine/channel.h \
  engine/vectorfilewriter.h \
  engine/util.h \
  engine/node.h \
  engine/exception.h \
  engine/nodetype.h \
  engine/dataflowmanager.h
engine/windowavg.o: engine/windowavg.cc \
  engine/channel.h \
  engine/windowavg.h \
  engine/util.h \
  engine/node.h \
  engine/exception.h \
  engine/commonnodes.h \
  engine/nodetype.h \
  engine/dataflowmanager.h
engine/xyplotnode.o: engine/xyplotnode.cc \
  engine/channel.h \
  engine/xyplotnode.h \
  engine/util.h \
  engine/node.h \
  engine/exception.h \
  engine/commonnodes.h \
  engine/nodetype.h \
  engine/dataflowmanager.h
