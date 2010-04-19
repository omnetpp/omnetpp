%module Simkernel

%{
#include <omnetpp.h>
#include "innerclasses.h"
#include "JSimpleModule.h"
#include "JMessage.h"

// for debugging:
#include <stdio.h>
#define LOG_JNI_CALL() (void)0
//#define LOG_JNI_CALL() {printf("DEBUG: entered JNI method %s, jarg1=%lx\n", __FUNCTION__, (long)jarg1);fflush(stdout);}
//jlong jarg1 = -1; // fallback for LOG_JNI_CALL() in JNI functions with no jarg1 arg
%}

%pragma(java) jniclasscode=%{
  static {
    // get classes for System.out.println() loaded now (needed if we want to log in startup code)
    System.out.println("Simkernel.jar loaded.");
  }
%}

%exception {
    try {
        $action
    } catch (std::exception& e) {
        SWIG_JavaThrowException(jenv, SWIG_JavaRuntimeException, const_cast<char*>(e.what()));
        return $null;
    }
}

%include "std_common.i"
%include "std_string.i"
%include "std_map.i"    // cXMLElement
%include "std_vector.i" // cXMLElement

%include commondefs.i
%include "PlainMemoryManagement.i"

%include "Reflection.i"

// hide some macros from swig (copied from nativelibs/common.i)
#define COMMON_API
#define ENVIR_API
#define OPP_DLLEXPORT
#define OPP_DLLIMPORT

#define NAMESPACE_BEGIN
#define NAMESPACE_END
#define USING_NAMESPACE
#define OPP

#define _OPPDEPRECATED

#pragma SWIG nowarn=516;  // "Overloaded method x ignored. Method y used."

// ignore/rename some operators (some have method equivalents)
%rename(set) operator=;
%rename(incr) operator++;
%ignore operator +=;
%ignore operator [];
%ignore operator <<;
%ignore operator ();

// ignore conversion operators (they all have method equivalents)
%ignore operator bool;
%ignore operator const char *;
%ignore operator char;
%ignore operator unsigned char;
%ignore operator short;
%ignore operator unsigned short;
%ignore operator int;
%ignore operator unsigned int;
%ignore operator long;
%ignore operator unsigned long;
%ignore operator double;
%ignore operator long double;
%ignore operator void *;
%ignore operator cOwnedObject *;
%ignore operator cXMLElement *;
%ignore cSimulation::operator=;
%ignore cEnvir::printf;

// ignore methods that are useless from Java
%ignore processMessage;  //cChannel
%ignore netPack;
%ignore netUnpack;
%ignore doPacking;
%ignore doUnpacking;
%ignore saveToFile;
%ignore loadFromFile;
%ignore createWatch;
%ignore opp_typename;

// ignore non-inspectable classes and those that cause problems
%ignore eMessageKind;
%ignore cLinkedList;
%ignore cCommBuffer;
%ignore cContextSwitcher;
%ignore cContextTypeSwitcher;
%ignore cOutputVectorManager;
%ignore cOutputScalarManager;
%ignore cSnapshotManager;
%ignore cScheduler;
%ignore cRealTimeScheduler;
%ignore cParsimCommunications;
%ignore ModNameParamResolver;
%ignore StringMapParamResolver;
%ignore cStackCleanupException;
%ignore cTerminationException;
%ignore cEndModuleException;
%ignore cStaticFlag;
%ignore ExecuteOnStartup;

%typemap(javacode) cModule %{
  public static cEnvir ev = Simkernel.getEv();
%};

%typemap(javacode) Simkernel %{
  public static cEnvir ev = getEv();
%};

%ignore cObject::getDescriptor;
%ignore cObject::createDescriptor;
%ignore cObject::info(char *buf);

%ignore cOwnedObject::cmpbyname;
%ignore cOwnedObject::removeFromOwnershipTree;
%ignore cOwnedObject::setDefaultOwner;

%ignore cMsgPar::setDoubleValue(ExprElem *, int);
%ignore cMsgPar::setDoubleValue(cStatistic *);
%ignore cMsgPar::setDoubleValue(MathFuncNoArg);
%ignore cMsgPar::setDoubleValue(MathFunc1Arg, double);
%ignore cMsgPar::setDoubleValue(MathFunc2Args, double, double);
%ignore cMsgPar::setDoubleValue(MathFunc3Args, double, double, double);
%ignore cMsgPar::setDoubleValue(MathFunc4Args, double, double, double, double);
%ignore cMsgPar::setPointerValue;
%ignore cMsgPar::getPointerValue;
%ignore cMsgPar::configPointer;
%ignore cMsgPar::operator=(void *);
%ignore cMsgPar::operator=(long double);
%ignore cMsgPar::cmpbyvalue;

%ignore cDefaultList::doGC;

%ignore cComponent::setRNGMap;
%ignore cComponent::addPar;

%ignore cModule::pause_in_sendmsg;
%ignore cModule::lastmodulefullpath;
%ignore cModule::lastmodulefullpathmod;
%ignore cModule::gatev;
%ignore cModule::paramv;
%ignore cModule::setRNGMap;
%ignore cModule::rng;

%ignore cSimpleModule::pause;
%ignore cSimpleModule::receive;
%ignore cSimpleModule::hasStackOverflow;
%ignore cSimpleModule::getStackSize;
%ignore cSimpleModule::getStackUsage;

%ignore cMessage::setContextPointer;
%ignore cMessage::getContextPointer;
%ignore cMessage::getInsertOrder;

%ignore cChannel::cChannel(const char *, cChannelType *);
%ignore cChannel::channelType;

%ignore cQueue::setup;
%ignore cQueue::cQueue(const char *, CompareFunc);
%ignore cQueue::cQueue(const char *, CompareFunc, bool);

%ignore cOutVector::setCallback;

%ignore cSimulation::msgQueue;
%ignore cSimulation::getMessageQueue;
%ignore cSimulation::setScheduler;
%ignore cSimulation::getScheduler;
%ignore cSimulation::getHasher;
%ignore cSimulation::setHasher;
%ignore cSimulation::setupNetwork;
%ignore cSimulation::startRun;
%ignore cSimulation::callFinish;
%ignore cSimulation::endRun;
%ignore cSimulation::deleteNetwork;
%ignore cSimulation::transferTo;
%ignore cSimulation::transferToMain;
%ignore cSimulation::setGlobalContext;
%ignore cSimulation::setContext;
%ignore cSimulation::getNetworkType;
%ignore cSimulation::registerModule;
%ignore cSimulation::deregisterModule;
%ignore cSimulation::setSystemModule;
%ignore cSimulation::loadNedFile;
%ignore cSimulation::setSimTime;
%ignore cSimulation::selectNextModule;
%ignore cSimulation::guessNextEvent;
%ignore cSimulation::guessNextModule;
%ignore cSimulation::guessNextSimtime;
%ignore cSimulation::doOneEvent;
%ignore cSimulation::setContextModule;
%ignore cSimulation::setContextType;

%ignore cStatistic::td;
%ignore cStatistic::ra;
%ignore cStatistic::addTransientDetection;
%ignore cStatistic::addAccuracyDetection;
%ignore cStatistic::getTransientDetectionObject;
%ignore cStatistic::getAccuracyDetectionObject;

%ignore cDisplayString::setRoleToConnection;
%ignore cDisplayString::setRoleToModule;
%ignore cDisplayString::setRoleToModuleBackground;

%ignore cXMLElement::getDocumentElementByPath;
%ignore cXMLElement::getElementByPath;

%ignore cObjectFactory::cObjectFactory;


// ignore deprecated methods
%ignore cChannelType::createIdealChannel;
%ignore cChannelType::createDelayChannel;
%ignore cChannelType::createDatarateChannel;
%ignore cMsgPar::getAsText;
%ignore cMsgPar::setFromText;
%ignore cMsgPar::setFromText;

// ignore cEnvir methods that are not for model code
%ignore cEnvir::disable_tracing;
%ignore cEnvir::suppress_notifications;
%ignore cEnvir::debug_on_errors;
%ignore cEnvir::objectDeleted;
%ignore cEnvir::simulationEvent;
%ignore cEnvir::messageSent_OBSOLETE;
%ignore cEnvir::messageScheduled;
%ignore cEnvir::messageCancelled;
%ignore cEnvir::beginSend;
%ignore cEnvir::messageSendDirect;
%ignore cEnvir::messageSendHop;
%ignore cEnvir::endSend;
%ignore cEnvir::messageDeleted;
%ignore cEnvir::moduleReparented;
%ignore cEnvir::componentMethodBegin;
%ignore cEnvir::componentMethodEnd;
%ignore cEnvir::moduleCreated;
%ignore cEnvir::moduleDeleted;
%ignore cEnvir::gateCreated;
%ignore cEnvir::gateDeleted;
%ignore cEnvir::connectionCreated;
%ignore cEnvir::connectionDeleted;
%ignore cEnvir::displayStringChanged;
%ignore cEnvir::undisposedObject;
%ignore cEnvir::bubble;
%ignore cEnvir::readParameter;
%ignore cEnvir::isModuleLocal;
%ignore cEnvir::getRNGMappingFor;
%ignore cEnvir::registerOutputVector;
%ignore cEnvir::deregisterOutputVector;
%ignore cEnvir::setVectorAttribute;
%ignore cEnvir::recordInOutputVector;
%ignore cEnvir::recordScalar;
%ignore cEnvir::recordStatistic;
%ignore cEnvir::getStreamForSnapshot;
%ignore cEnvir::releaseStreamForSnapshot;
%ignore cEnvir::getArgCount;
%ignore cEnvir::getArgVector;
%ignore cEnvir::idle;
%ignore cEnvir::getOStream;
%ignore cEnvir::getConfig;
%ignore cEnvir::getConfigEx;

%ignore cCoroutine;
%ignore cRunnableEnvir;
%ignore cConfiguration;
%ignore cConfigurationEx;

%ignore cPar::setImpl;
%ignore cPar::impl;
%ignore cPar::copyIfShared;

%ignore cRNG::initialize;
%ignore cLCG32;
%ignore cMersenneTwister;


namespace std {
   specialize_std_map_on_both(std::string,,,,std::string,,,);
   //specialize_std_vector(cXMLElement*);

   %template(StringMap) map<string,string>;

   %ignore vector<cXMLElement*>::vector;
   %ignore vector<cXMLElement*>::resize;
   %ignore vector<cXMLElement*>::reserve;
   %ignore vector<cXMLElement*>::capacity;
   %ignore vector<cXMLElement*>::clear;
   %ignore vector<cXMLElement*>::add;  //XXX this one doesn't work (because it was added later in Java)
   %ignore vector<cXMLElement*>::set;
   %template(cXMLElementVector) vector<cXMLElement*>;

   // std::vector<const char*> is only used as return value --> ignore setters
   %extend vector<const char *> {
       const char *get(int i) {return self->at(i);}
   }
   %ignore vector<const char *>::vector;
   %ignore vector<const char *>::resize;
   %ignore vector<const char *>::reserve;
   %ignore vector<const char *>::capacity;
   %ignore vector<const char *>::clear;
   %ignore vector<const char *>::add;  //XXX this one doesn't work (because it was added later in Java)
   %ignore vector<const char *>::set;
   %ignore vector<const char *>::get;
   %template(StringVector) vector<const char *>;
};

%extend SimTime {
   const SimTime add(const SimTime& x) {return *self + x;}
   const SimTime substract(const SimTime& x) {return *self - x;}
   const SimTime add(double x) {return *self + x;}
   const SimTime substract(double x) {return *self - x;}
}

%typemap(javacode) cEnvir %{
  public void print(String s) {
    puts(s);
  }

  public void println(String s) {
    puts(s+"\n");
  }
%}

%extend cEnvir
{
    void puts(const char *s) {printf("%s", s);}
};

cEnvir *getEv();
%{ inline cEnvir *getEv() {return &ev;} %}

// ignore/rename some operators (some have method equivalents)
%ignore cPar::operator=;
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
%ignore cGate::setChannel;

// ignore methods that are useless from Java
%ignore parsimPack;
%ignore parsimUnpack;

// ignore non-inspectable and deprecated classes
%ignore cCommBuffer;
%ignore cContextSwitcher;
%ignore cContextTypeSwitcher;
%ignore cOutputVectorManager;
%ignore cOutputScalarManager;
%ignore cOutputSnapshotManager;
%ignore cScheduler;
%ignore cRealTimeScheduler;
%ignore cParsimCommunications;
%ignore ModNameParamResolver;
%ignore StringMapParamResolver;
%ignore cSubModIterator;
%ignore cLinkedList;

// ignore global variables but add accessors for them
%ignore defaultList;
%ignore componentTypes;
%ignore nedFunctions;
%ignore classes;
%ignore enums;
%ignore classDescriptors;
%ignore configOptions;

%{
cDefaultList& getDefaultList() {return defaultList;}
cRegistrationList *getRegisteredComponentTypes() {return componentTypes.getInstance();}
cRegistrationList *getRegisteredNedFunctions() {return nedFunctions.getInstance();}
cRegistrationList *getRegisteredClasses() {return classes.getInstance();}
cRegistrationList *getRegisteredEnums() {return enums.getInstance();}
cRegistrationList *getRegisteredClassDescriptors() {return classDescriptors.getInstance();}
cRegistrationList *getRegisteredConfigOptions() {return configOptions.getInstance();}
%}
cDefaultList& getDefaultList();
cRegistrationList *getRegisteredComponentTypes();
cRegistrationList *getRegisteredNedFunctions();
cRegistrationList *getRegisteredClasses();
cRegistrationList *getRegisteredEnums();
cRegistrationList *getRegisteredClassDescriptors();
cRegistrationList *getRegisteredConfigOptions();

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


// ignore problematic methods/class
%ignore cDynamicExpression::evaluate; // returns inner type (swig is not prepared to handle them)
%ignore cDensityEstBase::getCellInfo; // returns inner type (swig is not prepared to handle them)
%ignore cKSplit;  // several methods are problematic
%ignore cPacketQueue;  // Java compile problems (cMessage/cPacket conversion)
%ignore cTopology; // would need to wrap its inner classes too
%ignore cDynamicExpression;
%ignore cAccuracyDetection;
%ignore cADByStddev;
%ignore cTransientDetection;
%ignore cTDExpandingWindows;

%ignore critfunc_const;
%ignore critfunc_depth;
%ignore divfunc_const;
%ignore divfunc_babak;

%ignore SimTime::ttoa;
%ignore SimTime::str(char *buf);
%ignore SimTime::parse(const char *, const char *&);

%ignore cMsgPar::operator=(void*);

%typemap(javacode) cClassDescriptor %{
  public static long getCPtr(cObject obj) { // make method public
    return cObject.getCPtr(obj);
  }
%}

%extend cClassDescriptor {
   cObject *getFieldAsCObject(void *object, int field, int index) {
       return self->getFieldIsCObject(object,field) ? (cObject *)self->getFieldStructPointer(object,field,index) : NULL;
   }
}

// prevent generating setSimulation() method
%ignore ::simulation;
cSimulation *getSimulation();
%{ inline cSimulation *getSimulation() {return &simulation;} %}


// JSimpleModule
%newobject JSimpleModule::retrieveMsgToBeHandled;
%ignore JSimpleModule::JSimpleModule;
%ignore JSimpleModule::vm;
%ignore JSimpleModule::jenv;

%javamethodmodifiers JSimpleModule::swigSetJavaPeer "private";
%javamethodmodifiers JSimpleModule::swigJavaPeerOf "protected";

%typemap(javacode) JSimpleModule %{

  public JSimpleModule() {
    this(0, false);  // and C++ code will call setCPtr() later
  }

  protected int numInitStages() {
    return 1;
  }

  protected void initialize(int stage) {
    if (stage==0)
      initialize();
  }

  protected void initialize() {
    // can be overridden by the user
  }

  private void doHandleMessage() {
    cMessage msg = retrieveMsgToBeHandled();
    handleMessage(msg);
  }

  protected void handleMessage(cMessage msg) {
    error("handleMessage() should be overridden in module classes");
  }

  protected void finish() {
    // can be overridden by the user
  }

  protected SimTime simTime() {
    return Simkernel.simTime();
  }

  public static JSimpleModule cast(cModule object) {
    return (JSimpleModule) JSimpleModule.swigJavaPeerOf(object);
  }
%}

%ignore JSimpleModule::swigJavaPeer;


// JMessage
%javamethodmodifiers JMessage::JMessage "private";
%javamethodmodifiers JMessage::swigSetJavaPeer "private";
%javamethodmodifiers JMessage::swigJavaPeerOf "protected";

%typemap(javacode) JMessage %{
  public JMessage() {this(null, 0, 99); swigSetJavaPeer(this); }
  public JMessage(String name) {this(name, 0, 99); swigSetJavaPeer(this); }
  public JMessage(String name, int kind) {this(name, kind, 99); swigSetJavaPeer(this); }

  public static JMessage cast(cMessage object) {
    return (JMessage) JMessage.swigJavaPeerOf(object);
  }
%}

%ignore JMessage::swigJavaPeer;

// hide toString() C++ methods which call back into Java: we don't want to
// create infinite mutual recursion between them
%ignore JMessage::toString;
%ignore JSimpleModule::toString;

// hide the following JObjectAccess methods (from JMessage and JSimpleModule too)
%ignore getBooleanJavaField;
%ignore getByteJavaField;
%ignore getCharJavaField;
%ignore getShortJavaField;
%ignore getIntJavaField;
%ignore getLongJavaField;
%ignore getFloatJavaField;
%ignore getDoubleJavaField;
%ignore getStringJavaField;
%ignore setBooleanJavaField;
%ignore setByteJavaField;
%ignore setCharJavaField;
%ignore setShortJavaField;
%ignore setIntJavaField;
%ignore setLongJavaField;
%ignore setFloatJavaField;
%ignore setDoubleJavaField;
%ignore setStringJavaField;


// Note: we MUST NOT rename dup() to clone(), because then JMessage's dup()
// would go into infinite mutual recursion between Java clone() and C++ dup()!
//%rename dup clone;


// The BASECLASS(), DERIVEDCLASS() macros should come from the memorymgmt_xxx.i file
BASECLASS(SimTime);
BASECLASS(cObject);
BASECLASS(cDisplayString);
BASECLASS(cEnvir);
BASECLASS(cException);
BASECLASS(cExpression);
BASECLASS(cSubModIterator);
BASECLASS(cVisitor);
BASECLASS(cXMLElement);
BASECLASS(std::vector<cXMLElement*>);
BASECLASS(cObjectFactory);
//BASECLASS(std::map<std::string,std::string>);
DERIVEDCLASS(cArray, cObject);
DERIVEDCLASS(cComponentType, cObject);
DERIVEDCLASS(cChannelType, cObject);
DERIVEDCLASS(cModuleType, cObject);
DERIVEDCLASS(cComponent, cObject);
DERIVEDCLASS(cChannel, cObject);
DERIVEDCLASS(cIdealChannel, cObject);
DERIVEDCLASS(cDelayChannel, cObject);
DERIVEDCLASS(cDatarateChannel, cObject);
DERIVEDCLASS(cModule, cObject);
DERIVEDCLASS(cSimpleModule, cObject);
DERIVEDCLASS(cCompoundModule, cObject);
DERIVEDCLASS(cDefaultList, cObject);
//DERIVEDCLASS(cDoubleExpression, cExpression);
DERIVEDCLASS(cGate, cObject);
DERIVEDCLASS(cMessage, cObject);
DERIVEDCLASS(cPacket, cObject);
DERIVEDCLASS(cPar, cObject);
DERIVEDCLASS(cObject, cObject);
DERIVEDCLASS(cOutVector, cObject);
DERIVEDCLASS(cMsgPar, cObject);
DERIVEDCLASS(cObject, cObject);
DERIVEDCLASS(cQueue, cObject);
DERIVEDCLASS(cRuntimeError, cException);
DERIVEDCLASS(cSimulation, cObject);
DERIVEDCLASS(cStatistic, cObject);
DERIVEDCLASS(cStdDev, cObject);
DERIVEDCLASS(cProperties, cObject);
DERIVEDCLASS(cProperty, cObject);

%ignore JMessage::JMessage(const JMessage&);
%ignore JMessage::operator=(const JMessage&);

typedef SimTime simtime_t;

%include "innerclasses.h"

%include "simkerneldefs.h"
%include "simtime.h"
%include "simtime_t.h"
%include "cobject.h"
%include "cnamedobject.h"
%include "cownedobject.h"
%include "cdefaultlist.h"
%include "ccomponent.h"
%include "cchannel.h"
%include "cdelaychannel.h"
%include "cdataratechannel.h"
%include "cmodule.h"
%include "ccoroutine.h"
%include "csimplemodule.h"
%include "ccompoundmodule.h"
%include "ccomponenttype.h"
%include "carray.h"
//%include "clinkedlist.h"
%include "cqueue.h"
%include "cpacketqueue.h"
%include "cdetect.h"
%include "cstatistic.h"
%include "cstddev.h"
%include "cdensityestbase.h"
%include "chistogram.h"
%include "cksplit.h"
%include "cpsquare.h"
%include "cvarhist.h"
%include "ccoroutine.h"
%include "crng.h"
%include "clcg32.h"
%include "cmersennetwister.h"
%include "cobjectfactory.h"
%include "ccommbuffer.h"
//%include "cconfiguration.h"
//%include "cconfigoption.h"
%include "cdisplaystring.h"
//%include "cdynamicexpression.h"
%include "cenum.h"
%include "cenvir.h"
%include "cexception.h"
%include "cexpression.h"
//%include "chasher.h"
%include "cfsm.h"
//%include "cmathfunction.h"
%include "cgate.h"
%include "cmessage.h"
%include "cmsgpar.h"
%include "cmessageheap.h"
//%include "cnedfunction.h"
//%include "cnullenvir.h"
%include "coutvector.h"
%include "cpar.h"
%include "cparsimcomm.h"
%include "cproperty.h"
%include "cproperties.h"
//%include "cscheduler.h"
%include "csimulation.h"
//%include "cstringtokenizer.h"
%include "cclassdescriptor.h"
//%include "ctopology.h"
%include "cvisitor.h"
//%include "cwatch.h"
%include "cstlwatch.h"
%include "cxmlelement.h"
%include "distrib.h"
%include "envirext.h"
%include "errmsg.h"
%include "globals.h"
%include "onstartup.h"
//%include "opp_string.h"
%include "random.h"
%include "cregistrationlist.h"
%include "regmacros.h"
%include "simutil.h"
//%include "packing.h"

//%include "index.h"
//%include "mersennetwister.h"
//%include "compat.h"
//%include "cparimpl.h"
//%include "cboolparimpl.h"
//%include "cdoubleparimpl.h"
//%include "clongparimpl.h"
//%include "cstringparimpl.h"
//%include "cstringpool.h"
//%include "cxmlparimpl.h"
//%include "nedsupport.h"

%include "JSimpleModule.h"
%include "JMessage.h"


