#
# Definitions to be customized by 'configure'
#

include ../../Makefile.inc

#
# Local definitions
#
COPTS=$(CFLAGS) -I$(OMNETPP_INCL_DIR)/platdep -I. -I../common

OBJS= ievent.o ieventlog.o eventlogfacade.o eventlogtablefacade.o sequencechartfacade.o eventlog.o eventlogindex.o messagedependency.o event.o eventlogentry.o eventlogentries.o filteredevent.o filteredeventlog.o eventlogentryfactory.o
LIBS= -leventlog -lcommon

#
# Automatic rules
#
.SUFFIXES : .cc

.cc.o:
	$(CXX) -c $(COPTS) -o $@ $<

#
# Targets
#
all: eventlogentries.cc libeventlog$(LIB_SUFFIX) eventlogtool

eventlogentries.cc eventlogentries.h eventlogfactory.cc : eventlogentries.txt eventlogentries.pl
	$(PERL) eventlogentries.pl

libeventlog$(A_LIB_SUFFIX) : $(OBJS)
	$(AR) libeventlog$(A_LIB_SUFFIX) $(OBJS)
	$(RANLIB) libeventlog$(A_LIB_SUFFIX)
	../utils/install-lib libeventlog$(A_LIB_SUFFIX).$(OMNETPP_VERSION) $(OMNETPP_LIB_DIR) libeventlog$(A_LIB_SUFFIX)
libeventlog$(SO_LIB_SUFFIX) : $(OBJS)
	$(SHLIB_LD) -o libeventlog$(SO_LIB_SUFFIX) $(OBJS)
	../utils/install-lib libeventlog$(SO_LIB_SUFFIX).$(OMNETPP_VERSION) $(OMNETPP_LIB_DIR) libeventlog$(SO_LIB_SUFFIX)

eventlogtool : eventlogtool.o  libeventlog$(LIB_SUFFIX)
	$(CXX) $(LDFLAGS) -o eventlogtool eventlogtool.o -L$(OMNETPP_LIB_DIR) $(LIBS)
	../utils/install-prog eventlogtool $(OMNETPP_BIN_DIR)

depend:
	$(MAKEDEPEND) -I. -I../common -- *.cc

clean:
	rm -f *.o libeventlog$(A_LIB_SUFFIX) libeventlog$(SO_LIB_SUFFIX) eventlogtool

# DO NOT DELETE THIS LINE -- make depend depends on it.
event.o: event.cc \
  ../common/filereader.h \
  event.h \
  eventlog.h \
  eventlogentry.h \
  ../common/commondefs.h \
  ievent.h \
  eventlogentries.h \
  messagedependency.h \
  eventlogdefs.h \
  ../common/exception.h \
  ../common/commonutil.h \
  ../common/bigdecimal.h \
  ../common/stringpool.h \
  ieventlog.h \
  eventlogindex.h \
  ../common/linetokenizer.h \
  ../common/matchexpression.h
eventlog.o: eventlog.cc \
  ../common/filereader.h \
  ../common/stringpool.h \
  eventlog.h \
  ../common/commondefs.h \
  event.h \
  ieventlog.h \
  eventlogindex.h \
  ievent.h \
  eventlogentry.h \
  eventlogentries.h \
  messagedependency.h \
  ../common/matchexpression.h \
  eventlogdefs.h \
  ../common/linetokenizer.h \
  ../common/exception.h \
  ../common/commonutil.h \
  ../common/bigdecimal.h
eventlogentries.o: eventlogentries.cc \
  event.h \
  eventlogentries.h \
  ../common/stringutil.h \
  ievent.h \
  ../common/filereader.h \
  eventlogentry.h \
  messagedependency.h \
  ../common/commondefs.h \
  ../common/matchexpression.h \
  eventlogdefs.h \
  ../common/linetokenizer.h \
  ../common/exception.h \
  ../common/commonutil.h \
  ../common/bigdecimal.h
eventlogentry.o: eventlogentry.cc \
  eventlog.h \
  eventlogentry.h \
  eventlogentryfactory.h \
  ../common/stringpool.h \
  ../common/filereader.h \
  event.h \
  ieventlog.h \
  eventlogindex.h \
  ../common/commondefs.h \
  ievent.h \
  eventlogentries.h \
  messagedependency.h \
  eventlogdefs.h \
  ../common/exception.h \
  ../common/commonutil.h \
  ../common/bigdecimal.h \
  ../common/linetokenizer.h \
  ../common/matchexpression.h
eventlogentryfactory.o: eventlogentryfactory.cc \
  event.h \
  eventlogentryfactory.h \
  ievent.h \
  ../common/filereader.h \
  eventlogentry.h \
  eventlogentries.h \
  messagedependency.h \
  ../common/commondefs.h \
  ../common/matchexpression.h \
  eventlogdefs.h \
  ../common/linetokenizer.h \
  ../common/exception.h \
  ../common/commonutil.h \
  ../common/bigdecimal.h
eventlogfacade.o: eventlogfacade.cc \
  ievent.h \
  ieventlog.h \
  event.h \
  eventlog.h \
  filteredevent.h \
  filteredeventlog.h \
  eventlogfacade.h \
  eventlogentry.h \
  messagedependency.h \
  ../common/matchexpression.h \
  eventlogdefs.h \
  ../common/linetokenizer.h \
  ../common/commondefs.h \
  ../common/exception.h \
  ../common/commonutil.h \
  ../common/bigdecimal.h \
  ../common/filereader.h \
  eventlogentries.h \
  ../common/stringpool.h \
  eventlogindex.h \
  ../common/patternmatcher.h
eventlogindex.o: eventlogindex.cc \
  eventlogindex.h \
  ../common/exception.h \
  eventlogdefs.h \
  ../common/filereader.h \
  ../common/linetokenizer.h \
  ../common/commonutil.h \
  ../common/bigdecimal.h \
  ../common/commondefs.h
eventlogtablefacade.o: eventlogtablefacade.cc \
  ievent.h \
  ieventlog.h \
  event.h \
  eventlogtablefacade.h \
  eventlogentry.h \
  messagedependency.h \
  ../common/matchexpression.h \
  eventlogdefs.h \
  ../common/linetokenizer.h \
  ../common/commondefs.h \
  ../common/exception.h \
  ../common/commonutil.h \
  ../common/bigdecimal.h \
  ../common/filereader.h \
  eventlogentries.h \
  eventlogfacade.h
eventlogtool.o: eventlogtool.cc \
  ../utils/ver.h \
  ../common/filereader.h \
  ../common/linetokenizer.h \
  eventlogindex.h \
  eventlog.h \
  filteredeventlog.h \
  ../common/commondefs.h \
  eventlogdefs.h \
  ../common/exception.h \
  ../common/commonutil.h \
  ../common/bigdecimal.h \
  ../common/stringpool.h \
  event.h \
  ieventlog.h \
  ievent.h \
  eventlogentry.h \
  eventlogentries.h \
  messagedependency.h \
  ../common/matchexpression.h \
  ../common/patternmatcher.h \
  filteredevent.h
filteredevent.o: filteredevent.cc \
  event.h \
  filteredevent.h \
  filteredeventlog.h \
  ievent.h \
  ../common/filereader.h \
  eventlogentry.h \
  eventlogentries.h \
  messagedependency.h \
  ../common/commondefs.h \
  ../common/matchexpression.h \
  eventlogdefs.h \
  ../common/linetokenizer.h \
  ../common/exception.h \
  ../common/commonutil.h \
  ../common/bigdecimal.h \
  ../common/patternmatcher.h \
  ieventlog.h \
  eventlog.h \
  ../common/stringpool.h \
  eventlogindex.h
filteredeventlog.o: filteredeventlog.cc \
  filteredeventlog.h \
  ../common/patternmatcher.h \
  ../common/matchexpression.h \
  eventlogdefs.h \
  ieventlog.h \
  eventlog.h \
  filteredevent.h \
  ../common/commondefs.h \
  ../common/exception.h \
  ../common/commonutil.h \
  ../common/bigdecimal.h \
  ievent.h \
  eventlogentry.h \
  messagedependency.h \
  ../common/linetokenizer.h \
  ../common/filereader.h \
  eventlogentries.h \
  ../common/stringpool.h \
  event.h \
  eventlogindex.h
ievent.o: ievent.cc \
  ievent.h \
  eventlogentry.h \
  messagedependency.h \
  ../common/matchexpression.h \
  eventlogdefs.h \
  ../common/linetokenizer.h \
  ../common/commondefs.h \
  ../common/exception.h \
  ../common/commonutil.h \
  ../common/bigdecimal.h \
  ../common/filereader.h \
  eventlogentries.h
ieventlog.o: ieventlog.cc \
  ieventlog.h \
  eventlogdefs.h \
  ievent.h \
  ../common/exception.h \
  ../common/commonutil.h \
  ../common/bigdecimal.h \
  ../common/commondefs.h \
  eventlogentry.h \
  messagedependency.h \
  ../common/matchexpression.h \
  ../common/linetokenizer.h \
  ../common/filereader.h \
  eventlogentries.h
messagedependency.o: messagedependency.cc \
  eventlogdefs.h \
  ievent.h \
  ieventlog.h \
  eventlogentry.h \
  messagedependency.h \
  ../common/exception.h \
  ../common/commonutil.h \
  ../common/bigdecimal.h \
  ../common/commondefs.h \
  ../common/matchexpression.h \
  ../common/linetokenizer.h \
  ../common/filereader.h \
  eventlogentries.h
sequencechartfacade.o: sequencechartfacade.cc \
  ../common/lcgrandom.h \
  ievent.h \
  ieventlog.h \
  event.h \
  messagedependency.h \
  sequencechartfacade.h \
  ../common/commondefs.h \
  ../common/exception.h \
  eventlogentry.h \
  ../common/matchexpression.h \
  eventlogdefs.h \
  ../common/linetokenizer.h \
  ../common/commonutil.h \
  ../common/bigdecimal.h \
  ../common/filereader.h \
  eventlogentries.h \
  eventlogfacade.h
