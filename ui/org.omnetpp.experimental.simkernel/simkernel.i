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
#include "omnetpp.h"
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
%include "defs.h"
%include "cpolymorphic.h"
%include "cobject.h"
%include "cvisitor.h"
//%include "cenvir.h" -- no need to wrap
//%include "util.h" -- no need to wrap
%include "opp_string.h"
%include "random.h"
%include "distrib.h"
%include "cexception.h"
%include "cdefaultlist.h"
%include "csimul.h"
//%include "macros.h" -- no need to wrap
%include "ctypes.h"
%include "carray.h"
%include "cqueue.h"
%include "cllist.h"
%include "globals.h"
//%include "cwatch.h" -- no need to wrap
//%include "cstlwatch.h" -- no need to wrap
%include "cpar.h"
%include "cgate.h"
%include "cmessage.h"
%include "cpacket.h"
%include "cmsgheap.h"
%include "cmodule.h"
%include "csimplemodule.h"
%include "cstat.h"
%include "cdensity.h"
%include "chist.h"
%include "cvarhist.h"
%include "cpsquare.h"
%include "cksplit.h"
%include "coutvect.h"
%include "cdetect.h"
%include "ctopo.h"
%include "cfsm.h"
%include "protocol.h"
%include "cenum.h"
%include "cstruct.h"
//%include "onstartup.h" -- no need to wrap
%include "cchannel.h"
%include "cdispstr.h"
//%include "envirext.h" -- no need to wrap
//%include "cconfig.h" -- no need to wrap
%include "cxmlelement.h"
//%include "cstrtokenizer.h" -- no need to wrap
//%include "cscheduler.h" -- no need to wrap
//%include "compat.h" -- no need to wrap
//%include "cparsimcomm.h" -- no need to wrap
//%include "ccommbuffer.h" -- no need to wrap
//%include "crng.h" -- no need to wrap

