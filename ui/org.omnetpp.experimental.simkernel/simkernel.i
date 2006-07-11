%module simkernel

//
// PROBLEMS:
//
// Crash scenario:
// 1. create a new cDisplayString() object in Java
// 2. pass it to cModule::setDisplayString()
// 3. when the Java object gets garbage collected, it'll delete the underlying C++ object
// 4. cModule will crash when it tries to access the display string object
// Solution: disown typemap or obj.disown() java method
//
// Memory leak (reverse scenario of the above):
// 1. call a C++ method from Java
// 2. C++ method creates and returns a new object
// 3. its Java proxy won't be owner, so C++ object will never get deleted
// Solution: use %newobject
//



%{
#include "include/omnetpp.h"
%}

%include "std_common.i"
%include "std_string.i"

#pragma SWIG nowarn=516;  // "Overloaded method x ignored. Method y used."

// ignore/rename some operators (some have method equivalents)
%rename(assign) operator=;
%rename(plusPlus) operator++;
%ignore operator +=;
%ignore operator [];
%ignore operator <<;
%ignore operator ();

// ignore conversion operators (they all have method equivalents)
%ignore operator bool;
%ignore operator const char *;
%ignore operator char;
%ignore operator unsigned char;
%ignore operator int;
%ignore operator unsigned int;
%ignore operator long;
%ignore operator unsigned long;
%ignore operator double;
%ignore operator long double;
%ignore operator void *;
%ignore operator cObject *;
%ignore operator cXMLElement *;

// ignore methods that are useless from Java
%ignore writeContents;
%ignore netPack;
%ignore netUnpack;

// ignore non-inspectable classes
%ignore cCommBuffer;
%ignore cContextSwitcher;
%ignore cContextTypeSwitcher;
%ignore cEnvir;
%ignore cConfiguration;
%ignore cOutputVectorManager;
%ignore cOutputScalarManager;
%ignore cOutputSnapshotManager;
%ignore cScheduler;
%ignore cRealTimeScheduler;
%ignore cParsimCommunications;
%ignore ModNameParamResolver;
%ignore StringMapParamResolver;

// SWIG doesn't understand nested classes, turn off corresponding warnings
//%warnfilter(312) cTopology::Node; -- this doesn't seem to work
//%warnfilter(312) cTopology; -- nor this

// now include all header files
%include "include/defs.h"
%include "include/cpolymorphic.h"
%include "include/cobject.h"
%include "include/cvisitor.h"
//%include "include/cenvir.h" -- no need to wrap
//%include "include/util.h" -- no need to wrap
%include "include/opp_string.h"
%include "include/random.h"
%include "include/distrib.h"
%include "include/cexception.h"
%include "include/cdefaultlist.h"
%include "include/csimul.h"
//%include "include/macros.h" -- no need to wrap
%include "include/ctypes.h"
%include "include/carray.h"
%include "include/cqueue.h"
%include "include/cllist.h"
%include "include/globals.h"
//%include "include/cwatch.h" -- no need to wrap
//%include "include/cstlwatch.h" -- no need to wrap
%include "include/cpar.h"
%include "include/cgate.h"
%include "include/cmessage.h"
%include "include/cpacket.h"
%include "include/cmsgheap.h"
%include "include/cmodule.h"
%include "include/csimplemodule.h"
%include "include/cstat.h"
%include "include/cdensity.h"
%include "include/chist.h"
%include "include/cvarhist.h"
%include "include/cpsquare.h"
%include "include/cksplit.h"
%include "include/coutvect.h"
%include "include/cdetect.h"
%include "include/ctopo.h"
%include "include/cfsm.h"
%include "include/protocol.h"
%include "include/cenum.h"
%include "include/cstruct.h"
//%include "include/onstartup.h" -- no need to wrap
%include "include/cchannel.h"
%include "include/cdispstr.h"
//%include "include/envirext.h" -- no need to wrap
//%include "include/cconfig.h" -- no need to wrap
%include "include/cxmlelement.h"
//%include "include/cstrtokenizer.h" -- no need to wrap
//%include "include/cscheduler.h" -- no need to wrap
//%include "include/compat.h" -- no need to wrap
//%include "include/cparsimcomm.h" -- no need to wrap
//%include "include/ccommbuffer.h" -- no need to wrap
//%include "include/crng.h" -- no need to wrap

