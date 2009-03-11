%module Simkernel

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

%pragma(java) jniclasscode=%{
  static {
    try {
      System.loadLibrary("simkernel");
    }
    catch (UnsatisfiedLinkError e) {
      System.err.println("Native code library failed to load. \n" + e);
      System.exit(1);
    }
  }
%}


%{
#include "omnetpp.h"
//XXX #include "javaenv/javaenv.h"

#include <direct.h>
inline void changeToDir(const char *dir)  //XXX
{
    printf("changing to: %s\n", dir);
    _chdir(dir);

    //char buffer[_MAX_PATH];
    //if (_getcwd( buffer, _MAX_PATH)==NULL)
    //   strcpy(buffer,"???");
    //printf("current working directory: %s\n", buffer);
}

inline void evSetup(const char *inifile) { //XXX
    char *argv[] = {"exe", "-f", (char *)inifile, NULL};
    int argc = 3;
//XXX    ev.setup(argc, argv);
}

%}

%include "std_common.i"
%include "std_string.i"

// hide some macros from swig (copied from nativelibs/common.i)
#define COMMON_API
#define OPP_DLLEXPORT
#define OPP_DLLIMPORT

#define NAMESPACE_BEGIN
#define NAMESPACE_END
#define USING_NAMESPACE

#define _OPPDEPRECATED


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
%ignore cSimulation::operator=;
%ignore cEnvir::printf;

// ignore methods that are useless from Java
%ignore parsimPack;
%ignore parsimUnpack;

// ignore non-inspectable classes
%ignore cCommBuffer;
%ignore cContextSwitcher;
%ignore cContextTypeSwitcher;
%ignore cConfiguration;
%ignore cOutputVectorManager;
%ignore cOutputScalarManager;
%ignore cOutputSnapshotManager;
%ignore cScheduler;
%ignore cRealTimeScheduler;
%ignore cParsimCommunications;
%ignore ModNameParamResolver;
%ignore StringMapParamResolver;
%ignore cSubModIterator;

// ignore global variables
%ignore defaultList;
%ignore componentTypes;
%ignore nedFunctions;
%ignore classes;
%ignore enums;
%ignore classDescriptors;
%ignore configOptions;

// ignore macros that confuse swig
/*
#define GATEID_LBITS  20
#define GATEID_HBITS  (8*sizeof(int)-GATEID_LBITS)   // usually 12
#define GATEID_HMASK  ((~0)<<GATEID_LBITS)           // usually 0xFFF00000
#define GATEID_LMASK  (~GATEID_HMASK)                // usually 0x000FFFFF
*/
%ignore MAX_VECTORGATES;
%ignore MAX_SCALARGATES;
%ignore MAX_VECTORGATESIZE;


// typemaps to wrap Javaenv::setJCallback(JNIEnv *jenv, jobject jcallbackobj):
// %typemap(in, numinputs=0): unfortunately, generated java code doesn't compile
%typemap(in) JNIEnv *jenv {
    $1 = jenv;
}
%typemap(in) jobject jcallbackobj {
    $1 = j$1;
}

%typemap(javainterfaces) cSimulation "org.omnetpp.common.simulation.model.IRuntimeSimulation";

%typemap(javainterfaces) cModule "org.omnetpp.common.simulation.model.IRuntimeModule";
%typemap(javaimports) cModule
  "import org.omnetpp.common.displaymodel.DisplayString;\n"
  "import org.omnetpp.common.displaymodel.IDisplayString;";

%typemap(javacode) cModule %{
  public IDisplayString getDisplayString() {
    return new DisplayString(null, null, displayString().getString());
  }

  @Override
  public int hashCode() {
    final int PRIME = 31;
    int result = 1;
    result = PRIME * result + getId();
    return result;
  }

  @Override
  public boolean equals(Object obj) {
    if (this == obj)
      return true;
    if (obj == null)
      return false;
    if (getClass() != obj.getClass())
      return false;
    final cModule other = (cModule) obj;
    if (getId() != other.getId())
      return false;
    return true;
  }
%};

%typemap(javainterfaces) cGate "org.omnetpp.common.simulation.model.IRuntimeGate";
%typemap(javaimports) cGate
  "import org.omnetpp.common.displaymodel.DisplayString;\n"
  "import org.omnetpp.common.displaymodel.IDisplayString;";

%typemap(javacode) cGate %{
  public IDisplayString getDisplayString() {
    return new DisplayString(null, null, displayString().getString());
  }
%};

%typemap(javainterfaces) cMessage "org.omnetpp.common.simulation.model.IRuntimeMessage";

// SWIG doesn't understand nested classes, turn off corresponding warnings
//%warnfilter(312) cTopology::Node; -- this doesn't seem to work
//%warnfilter(312) cTopology; -- nor this

// now include all header files
%include "simkerneldefs.h"
%include "cobject.h"
%include "cnamedobject.h"
%include "cownedobject.h"
%include "cvisitor.h"
//%include "opp_string.h"
//%include "random.h"
//%include "distrib.h"
%include "cexception.h"
%include "cdefaultlist.h"
%include "csimulation.h"
//%include "ctypes.h"
//%include "carray.h"
//%include "cqueue.h"
//%include "cllist.h"
%include "globals.h"
//%include "cpar.h"
%include "cgate.h"
%include "cmessage.h"
//%include "cpacket.h"
//%include "cmsgheap.h"
%include "cmodule.h"
%include "csimplemodule.h"
//%include "cstat.h"
//%include "cdensity.h"
//%include "chist.h"
//%include "cvarhist.h"
//%include "cpsquare.h"
//%include "cksplit.h"
//%include "coutvector.h"
//%include "cdetect.h"
//%include "ctopo.h"
//%include "cfsm.h"
//%include "protocol.h"
//%include "cenum.h"
//%include "cstruct.h"
//%include "cchannel.h"
%include "cdisplaystring.h"
//%include "cxmlelement.h"
%include "cenvir.h"

//XXX %include "javaenv/javaenv.h"

//%include "util.h" -- no need to wrap
//%include "macros.h" -- no need to wrap
//%include "cwatch.h" -- no need to wrap
//%include "cstlwatch.h" -- no need to wrap
//%include "onstartup.h" -- no need to wrap
//%include "envirext.h" -- no need to wrap
//%include "cconfiguration.h" -- no need to wrap
//%include "cstrtokenizer.h" -- no need to wrap
//%include "cscheduler.h" -- no need to wrap
//%include "compat.h" -- no need to wrap
//%include "cparsimcomm.h" -- no need to wrap
//%include "ccommbuffer.h" -- no need to wrap
//%include "crng.h" -- no need to wrap



void evSetup(const char *inifile); //XXX
void changeToDir(const char *dir); //XXX

