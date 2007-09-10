#
# Definitions to be customized by 'configure'
#

include ../../Makefile.inc

#
# Local definitions
#
COPTS=$(CFLAGS) $(XML_CFLAGS) -I. -I../common -I$(OMNETPP_INCL_DIR)/platdep

OBJS= arraybuilder.o channel.o commonnodes.o compoundfilter.o \
      dataflowmanager.o datasorter.o diffquot.o \
      filewriter.o filternodes.o customfilter.o stddev.o \
      idlist.o mergernodes.o nodetype.o nodetyperegistry.o \
      resultfilemanager.o slidingwinavg.o \
      vectorfilereader.o vectorfilewriter.o windowavg.o \
      xyplotnode.o indexedvectorfile.o \
      vectorfileindexer.o indexfile.o scaveutils.o \
      scaveexception.o enumtype.o export.o teenode.o \
      indexedvectorfilereader.o xyarray.o

#
# Automatic rules
#
.SUFFIXES : .cc

.cc.o:
	$(CXX) -c $(COPTS) -o $@ $<

#
# Targets
#
default : scavelib

scavelib : libscave$(LIB_SUFFIX)

libscave$(A_LIB_SUFFIX): $(OBJS)
	$(AR) libscave$(A_LIB_SUFFIX) $(OBJS)
	$(RANLIB) libscave$(A_LIB_SUFFIX)
	../utils/install-lib libscave$(A_LIB_SUFFIX).$(OMNETPP_VERSION) $(OMNETPP_LIB_DIR) libscave$(A_LIB_SUFFIX)

libscave$(SO_LIB_SUFFIX) : $(OBJS)
	$(SHLIB_LD) -o libscave$(SO_LIB_SUFFIX) $(OBJS)
	../utils/install-lib libscave$(SO_LIB_SUFFIX).$(OMNETPP_VERSION) $(OMNETPP_LIB_DIR) libscave$(SO_LIB_SUFFIX)

depend:
	$(MAKEDEPEND) -I. -I../common -- *.cc

doc:
	# NOTE: doxy.cfg has the output directory hardcoded!
	$(DOXYGEN) doxy.cfg

clean:
	rm -f *.o
	rm -f libscave$(A_LIB_SUFFIX) libscave$(SO_LIB_SUFFIX)

# DO NOT DELETE THIS LINE -- make depend depends on it.
arraybuilder.o: arraybuilder.cc \
  channel.h \
  arraybuilder.h \
  ../common/commonutil.h \
  node.h \
  ../common/commondefs.h \
  ../common/exception.h \
  scavedefs.h \
  commonnodes.h \
  xyarray.h \
  nodetype.h \
  dataflowmanager.h
channel.o: channel.cc \
  channel.h \
  ../common/commonutil.h \
  node.h \
  ../common/commondefs.h \
  ../common/exception.h \
  scavedefs.h
commonnodes.o: commonnodes.cc \
  commonnodes.h \
  channel.h \
  node.h \
  nodetype.h \
  scavedefs.h \
  ../common/commonutil.h \
  ../common/commondefs.h \
  ../common/exception.h \
  dataflowmanager.h
compoundfilter.o: compoundfilter.cc \
  ../common/commonutil.h \
  compoundfilter.h \
  filternodes.h \
  nodetyperegistry.h \
  ../common/commondefs.h \
  ../common/exception.h \
  nodetype.h \
  commonnodes.h \
  dataflowmanager.h \
  node.h \
  channel.h \
  scavedefs.h
dataflowmanager.o: dataflowmanager.cc \
  channel.h \
  nodetype.h \
  dataflowmanager.h \
  ../common/commonutil.h \
  node.h \
  ../common/commondefs.h \
  ../common/exception.h \
  scavedefs.h
datasorter.o: datasorter.cc \
  resultfilemanager.h \
  ../common/commonutil.h \
  datasorter.h \
  ../common/stringutil.h \
  idlist.h \
  ../common/exception.h \
  scavedefs.h \
  ../common/commondefs.h \
  ../common/patternmatcher.h
diffquot.o: diffquot.cc \
  channel.h \
  diffquot.h \
  ../common/commonutil.h \
  node.h \
  ../common/commondefs.h \
  ../common/exception.h \
  scavedefs.h \
  commonnodes.h \
  nodetype.h \
  dataflowmanager.h
filewriter.o: filewriter.cc \
  channel.h \
  filewriter.h \
  ../common/commonutil.h \
  node.h \
  ../common/commondefs.h \
  ../common/exception.h \
  scavedefs.h \
  commonnodes.h \
  nodetype.h \
  dataflowmanager.h
filternodes.o: filternodes.cc \
  channel.h \
  filternodes.h \
  ../common/commonutil.h \
  node.h \
  ../common/commondefs.h \
  ../common/exception.h \
  scavedefs.h \
  commonnodes.h \
  nodetype.h \
  dataflowmanager.h
idlist.o: idlist.cc \
  idlist.h \
  resultfilemanager.h \
  ../common/stringutil.h \
  scavedefs.h \
  ../common/exception.h \
  ../common/commonutil.h \
  ../common/commondefs.h
indexedvectorfile.o: indexedvectorfile.cc \
  ../common/exception.h \
  ../common/linetokenizer.h \
  channel.h \
  ../common/stringutil.h \
  indexedvectorfile.h \
  scaveutils.h \
  ../common/commondefs.h \
  ../common/commonutil.h \
  node.h \
  scavedefs.h \
  ../common/filereader.h \
  indexfile.h \
  nodetype.h \
  resultfilemanager.h \
  dataflowmanager.h \
  idlist.h
indexfile.o: indexfile.cc \
  ../common/exception.h \
  ../common/filereader.h \
  ../common/linetokenizer.h \
  ../common/stringutil.h \
  scaveutils.h \
  indexfile.h \
  ../common/commondefs.h \
  ../common/commonutil.h \
  scavedefs.h
mergernodes.o: mergernodes.cc \
  channel.h \
  mergernodes.h \
  ../common/commonutil.h \
  node.h \
  ../common/commondefs.h \
  ../common/exception.h \
  scavedefs.h \
  commonnodes.h \
  nodetype.h \
  dataflowmanager.h
nodetype.o: nodetype.cc \
  ../common/commonutil.h \
  nodetype.h \
  ../common/commondefs.h \
  ../common/exception.h \
  dataflowmanager.h \
  node.h \
  channel.h \
  scavedefs.h
nodetyperegistry.o: nodetyperegistry.cc \
  ../common/commonutil.h \
  nodetyperegistry.h \
  ../common/stringtokenizer.h \
  arraybuilder.h \
  vectorfilereader.h \
  vectorfilewriter.h \
  indexedvectorfile.h \
  filewriter.h \
  windowavg.h \
  slidingwinavg.h \
  filternodes.h \
  mergernodes.h \
  xyplotnode.h \
  diffquot.h \
  stddev.h \
  ../common/commondefs.h \
  ../common/exception.h \
  node.h \
  nodetype.h \
  scavedefs.h \
  dataflowmanager.h \
  channel.h \
  commonnodes.h \
  xyarray.h \
  ../common/filereader.h \
  ../common/linetokenizer.h \
  resultfilemanager.h \
  idlist.h \
  indexfile.h
octaveexport.o: octaveexport.cc \
  octaveexport.h \
  scavedefs.h \
  xyarray.h \
  ../common/exception.h \
  ../common/commondefs.h
resultfilemanager.o: resultfilemanager.cc \
  ../common/matchexpression.h \
  ../common/patternmatcher.h \
  ../common/filereader.h \
  ../common/linetokenizer.h \
  ../common/stringtokenizer.h \
  indexfile.h \
  scaveutils.h \
  resultfilemanager.h \
  ../common/commondefs.h \
  ../common/exception.h \
  scavedefs.h \
  idlist.h \
  ../common/commonutil.h
scavetool.o: scavetool.cc \
  ../utils/ver.h \
  resultfilemanager.h \
  nodetype.h \
  nodetyperegistry.h \
  dataflowmanager.h \
  vectorfilereader.h \
  vectorfilewriter.h \
  vectorfileindexer.h \
  filternodes.h \
  filewriter.h \
  arraybuilder.h \
  octaveexport.h \
  ../common/stringutil.h \
  idlist.h \
  ../common/exception.h \
  ../common/commonutil.h \
  scavedefs.h \
  ../common/commondefs.h \
  node.h \
  channel.h \
  ../common/filereader.h \
  ../common/linetokenizer.h \
  commonnodes.h \
  xyarray.h
scaveutils.o: scaveutils.cc \
  scaveutils.h
slidingwinavg.o: slidingwinavg.cc \
  channel.h \
  slidingwinavg.h \
  ../common/commonutil.h \
  node.h \
  ../common/commondefs.h \
  ../common/exception.h \
  scavedefs.h \
  commonnodes.h \
  nodetype.h \
  dataflowmanager.h
stddev.o: stddev.cc \
  channel.h \
  stddev.h \
  ../common/commonutil.h \
  node.h \
  ../common/commondefs.h \
  ../common/exception.h \
  scavedefs.h \
  commonnodes.h \
  xyarray.h \
  nodetype.h \
  dataflowmanager.h
vectorfileindexer.o: vectorfileindexer.cc \
  ../common/stringutil.h \
  resultfilemanager.h \
  nodetype.h \
  nodetyperegistry.h \
  dataflowmanager.h \
  vectorfilereader.h \
  indexedvectorfile.h \
  indexfile.h \
  vectorfileindexer.h \
  ../common/commondefs.h \
  ../common/commonutil.h \
  ../common/exception.h \
  idlist.h \
  scavedefs.h \
  node.h \
  channel.h \
  ../common/filereader.h \
  ../common/linetokenizer.h
vectorfilereader.o: vectorfilereader.cc \
  channel.h \
  scaveutils.h \
  vectorfilereader.h \
  ../common/commonutil.h \
  node.h \
  ../common/commondefs.h \
  ../common/exception.h \
  scavedefs.h \
  nodetype.h \
  ../common/filereader.h \
  ../common/linetokenizer.h \
  resultfilemanager.h \
  dataflowmanager.h \
  idlist.h
vectorfilewriter.o: vectorfilewriter.cc \
  channel.h \
  vectorfilewriter.h \
  ../common/stringutil.h \
  ../common/commonutil.h \
  node.h \
  ../common/commondefs.h \
  ../common/exception.h \
  scavedefs.h \
  nodetype.h \
  resultfilemanager.h \
  dataflowmanager.h \
  idlist.h
windowavg.o: windowavg.cc \
  channel.h \
  windowavg.h \
  ../common/commonutil.h \
  node.h \
  ../common/commondefs.h \
  ../common/exception.h \
  scavedefs.h \
  commonnodes.h \
  nodetype.h \
  dataflowmanager.h
xyplotnode.o: xyplotnode.cc \
  channel.h \
  xyplotnode.h \
  ../common/commonutil.h \
  node.h \
  ../common/commondefs.h \
  ../common/exception.h \
  scavedefs.h \
  commonnodes.h \
  nodetype.h \
  dataflowmanager.h
