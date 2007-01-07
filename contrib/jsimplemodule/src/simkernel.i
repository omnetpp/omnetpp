%module Simkernel

%{
#include <omnetpp.h>
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
    } catch (cException *e) {
        SWIG_exception(SWIG_RuntimeError,const_cast<char*>(e->message()));
    }
}

%include "std_common.i"
%include "std_string.i"
%include "std_map.i"    // cXMLElement
%include "std_vector.i" // cXMLElement

%include "memorymgmt_plain.i"
//%include "memorymgmt_msg_one2one.i"

%include "reflect.i"

#pragma SWIG nowarn=516;  // "Overloaded method x ignored. Method y used."

// SWIG doesn't understand nested classes, turn off corresponding warnings
//%warnfilter(312) cTopology::Node; -- this doesn't seem to work
//%warnfilter(312) cTopology; -- nor this

// ignore/rename some operators (some have method equivalents)
%rename(assign) operator=;
%rename(increment) operator++;
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
%ignore operator cObject *;
%ignore operator cXMLElement *;
%ignore cSimulation::operator=;
%ignore cEnvir::printf;

// ignore methods that are useless from Java
%ignore netPack;
%ignore netUnpack;
%ignore writeTo;
%ignore writeContents;
%ignore saveToFile;
%ignore loadFromFile;

// ignore non-inspectable classes
%ignore eMessageKind;
%ignore cBag;
%ignore cLinkedList;
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
%ignore cStackCleanupException;
%ignore cTerminationException;
%ignore cEndModuleException;
%ignore cStaticFlag;

%typemap(javacode) cModule %{
  public static cEnvir ev = Simkernel.getEv();
%};

%typemap(javacode) Simkernel %{
  public static cEnvir ev = getEv();
%};

%ignore cPolymorphic::getDescriptor;

%ignore cObject::cmpbyname;
%ignore cObject::removeFromOwnershipTree;
%ignore cObject::totalObjectCount;
%ignore cObject::liveObjectCount;
%ignore cObject::resetObjectCounters;

%ignore cPar::setDoubleValue(ExprElem *, int);
%ignore cPar::setDoubleValue(cStatistic *);
%ignore cPar::setDoubleValue(MathFuncNoArg);
%ignore cPar::setDoubleValue(MathFunc1Arg, double);
%ignore cPar::setDoubleValue(MathFunc2Args, double, double);
%ignore cPar::setDoubleValue(MathFunc3Args, double, double, double);
%ignore cPar::setDoubleValue(MathFunc4Args, double, double, double, double);
%ignore cPar::setPointerValue;
%ignore cPar::pointerValue;
%ignore cPar::configPointer;
%ignore cPar::operator=(void *);
%ignore cPar::operator=(long double);
%ignore cPar::cmpbyvalue;

%ignore cDefaultList::doGC;

%ignore cModule::pause_in_sendmsg;
%ignore cModule::lastmodulefullpath;
%ignore cModule::lastmodulefullpathmod;
%ignore cModule::gatev;
%ignore cModule::paramv;
%ignore cModule::setRNGMap;
%ignore cModule::rng;

%ignore cSimpleModule::phase;
%ignore cSimpleModule::pause;
%ignore cSimpleModule::receive;
%ignore cSimpleModule::stackOverflow;
%ignore cSimpleModule::stackSize;
%ignore cSimpleModule::stackUsage;
%ignore cSimpleModule::moduleState;

%ignore cMessage::setContextPointer;
%ignore cMessage::contextPointer;
%ignore cMessage::cmpbydelivtime;
%ignore cMessage::cmpbypriority;
%ignore cMessage::insertOrder;

%ignore cGate::setLink;

%ignore cChannel::cChannel(const char *, cChannelType *);
%ignore cChannel::channelType;

%ignore cQueue::setup;
%ignore cQueue::cQueue(const char *, CompareFunc);
%ignore cQueue::cQueue(const char *, CompareFunc, bool);

%ignore cOutVector::setCallback;

%ignore cSimulation::msgQueue;
%ignore cSimulation::messageQueue;
%ignore cSimulation::setScheduler;
%ignore cSimulation::scheduler;
%ignore cSimulation::hasher;
%ignore cSimulation::setHasher;
%ignore cSimulation::setupNetwork;
%ignore cSimulation::startRun;
%ignore cSimulation::callFinish;
%ignore cSimulation::endRun;
%ignore cSimulation::deleteNetwork;
%ignore cSimulation::init;
%ignore cSimulation::shutdown;
%ignore cSimulation::transferTo;
%ignore cSimulation::transferToMain;
%ignore cSimulation::setGlobalContext;
%ignore cSimulation::setContext;
%ignore cSimulation::networkType;
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
%ignore cStatistic::transientDetectionObject;
%ignore cStatistic::accuracyDetectionObject;

%ignore cDisplayString::setRoleToConnection;
%ignore cDisplayString::setRoleToModule;
%ignore cDisplayString::setRoleToModuleBackground;

%ignore cXMLElement::getDocumentElementByPath;
%ignore cXMLElement::getElementByPath;

namespace std {
   specialize_std_map_on_both(std::string,,,,std::string,,,);
   specialize_std_vector(cXMLElement*);

   %template(StringMap) map<string,string>;

   %ignore vector<cXMLElement*>::vector;
   %ignore vector<cXMLElement*>::resize;
   %ignore vector<cXMLElement*>::reserve;
   %ignore vector<cXMLElement*>::capacity;
   %ignore vector<cXMLElement*>::clear;
   %ignore vector<cXMLElement*>::add;  //XXX this one doesn't work (because it was added later in Java)
   %ignore vector<cXMLElement*>::set;
   %template(cXMLElementVector) vector<cXMLElement*>;
};

%typemap(javacode) cEnvir %{
  public void print(String s) {
    puts(s);
    //System.out.print("*** "+s);
  }

  public void println(String s) {
    puts(s+"\n");
    //System.out.println("*** "+s);
  }
%}

class cEnvir
{
  public:
    bool isGUI();
    void puts(const char *s);
    long getUniqueNumber();
};

cEnvir *getEv();
%{ inline cEnvir *getEv() {return &ev;} %}

// ignore global vars
%ignore ::defaultList;
%ignore ::networks;
%ignore ::modinterfaces;
%ignore ::modtypes;
%ignore ::channeltypes;
%ignore ::functions;
%ignore ::classes;
%ignore ::enums;
%ignore ::classDescriptors;

// ignore (some) global functions
//%ignore ::findNetwork;
//%ignore ::findModuleType;
%ignore ::findModuleInterface;
//%ignore ::findChannelType;
%ignore ::findLink;
%ignore ::findFunction;
%ignore ::findEnum;
%ignore ::findfunctionbyptr;
%ignore ::connect;
%ignore ::__contextModuleRNG;

%ignore ::genk_uniform;
%ignore ::genk_intuniform;
%ignore ::genk_exponential;
%ignore ::genk_normal;
%ignore ::genk_truncnormal;

// prevent generatig setSimulation() method
%ignore ::simulation;
cSimulation *getSimulation();
%{ inline cSimulation *getSimulation() {return &simulation;} %}

//%ignore cNetworkType;
//%ignore cModuleType;
%ignore cModuleInterface;
//%ignore cChannelType;
%ignore cFunctionType;
%ignore cLinkType;
%ignore cClassRegister;

%ignore cModuleType::cModuleType;
%ignore cNetworkType::cNetworkType;
%ignore cChannelType::cChannelType;

%ignore cModuleType::moduleInterface;

// JSimpleModule
%newobject JSimpleModule::retrieveMsgToBeHandled;
%ignore JSimpleModule::JSimpleModule;
%ignore JSimpleModule::vm;
%ignore JSimpleModule::jenv;

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
    //System.out.print("\n\n--- NEW EVENT ---\n\n");
    cMessage msg = retrieveMsgToBeHandled();
    //System.out.println("*** module "+getName()+" processing "+msg.getName());
    handleMessage(msg);
  }

  protected void handleMessage(cMessage msg) {
    error("handleMessage() should be overridden in module classes");
  }

  protected void finish() {
    // can be overridden by the user
  }
%}

// JMessage
%javamethodmodifiers JMessage::JMessage "private";
%javamethodmodifiers JMessage::swigSetJavaPeer "private";
%javamethodmodifiers JMessage::swigJavaPeerOf "protected";

%typemap(javacode) JMessage %{
  public JMessage() {this(null, 0, 99); swigSetJavaPeer(this); }
  public JMessage(String name) {this(name, 0, 99); swigSetJavaPeer(this); }
  public JMessage(String name, int kind) {this(name, kind, 99); swigSetJavaPeer(this); }

  public static JMessage castFrom(cPolymorphic object) {
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


//
// Add "get" to getter method names.
//
%rename cPolymorphic::name getName;
%rename cPolymorphic::fullName getFullName;
%rename cPolymorphic::fullPath getFullPath;
%rename cPolymorphic::className getClassName;
%rename cPolymorphic::info getInfo;
%rename cPolymorphic::detailedInfo getDetailedInfo;

%rename cObject::owner  getOwner;

%rename cDefaultList::defaultOwner getDefaultOwner;

%rename cGate::id getId;
%rename cGate::index getIndex;
%rename cGate::size getSize;
%rename cGate::ownerModule getOwnerModule;

%rename cMessage::kind getKind;
%rename cMessage::priority getPriority;
%rename cMessage::length getLength;
%rename cMessage::byteLength getByteLength;
%rename cMessage::shareCount getShareCount;
%rename cMessage::timestamp getTimestamp;
%rename cMessage::encapsulatedMsg getEncapsulatedMsg;

%rename cMessage::senderModuleId getSenderModuleId;
%rename cMessage::senderGateId getSenderGateId;
%rename cMessage::arrivalModuleId getArrivalModuleId;
%rename cMessage::arrivalGateId getArrivalGateId;
%rename cMessage::senderModule getSenderModule;
%rename cMessage::senderGate getSenderGate;
%rename cMessage::arrivalModule getArrivalModule;
%rename cMessage::arrivalGate getArrivalGate;
%rename cMessage::creationTime getCreationTime;
%rename cMessage::sendingTime getSendingTime;
%rename cMessage::arrivalTime getArrivalTime;
%rename cMessage::id getId;
%rename cMessage::treeId getTreeId;
%rename cMessage::encapsulationId getEncapsulationId;
%rename cMessage::encapsulationTreeId getEncapsulationTreeId;
%rename cMessage::encapsulationTreeId getEncapsulationTreeId;
%rename cMessage::srcProcId getSrcProcId;
%rename cMessage::controlInfo  getControlInfo;
%rename cMessage::parList  getParList;
%rename cMessage::displayString  getDisplayString;
%rename cMessage::totalMessageCount  getTotalMessageCount;
%rename cMessage::liveMessageCount  getLiveMessageCount;

%rename cSimulation::systemModule getSystemModule;
%rename cSimulation::moduleByPath getModuleByPath;
%rename cSimulation::module getModuleByID;
%rename cSimulation::lastModuleId  getLastModuleId;
%rename cSimulation::runNumber  getRunNumber;
%rename cSimulation::simTime  getSimTime;
%rename cSimulation::eventNumber  getEventNumber;
%rename cSimulation::runningModule  getRunningModule;
%rename cSimulation::contextModule  getContextModule;
%rename cSimulation::contextType  getContextType;
%rename cSimulation::contextSimpleModule  getContextSimpleModule;

%rename cModule::id getId;
%rename cModule::index getIndex;
%rename cModule::size getSize;
%rename cModule::parentModule getParentModule;
%rename cModule::submodule getSubmodule;
%rename cModule::gates getNumGates;
%rename cModule::gate getGate;
%rename cModule::moduleType  getModuleType;
%rename cModule::params  getParams;
%rename cModule::displayString  getDisplayString;
%rename cModule::backgroundDisplayString  getBackgroundDisplayString;

%rename cQueue::length getLength;
%rename cQueue::empty isEmpty;

%rename cArray::empty isEmpty;

%rename cChannel::delay getDelay;
%rename cChannel::error getError;
%rename cChannel::datarate getDatarate;
%rename cChannel::disabled getDisabled;
%rename cChannel::fromGate  getFromGate;

%rename cBasicChannel::delay getDelay;
%rename cBasicChannel::error getError;
%rename cBasicChannel::datarate getDatarate;
%rename cBasicChannel::disabled getDisabled;
%rename cBasicChannel::transmissionFinishes  getTransmissionFinishes;

%rename cGate::delay getDelay;
%rename cGate::error getError;
%rename cGate::datarate getDatarate;
%rename cGate::disabled getDisabled;
%rename cGate::channel  getChannel;
%rename cGate::type  getType;
%rename cGate::fromGate  getFromGate;
%rename cGate::toGate  getToGate;
%rename cGate::sourceGate  getSourceGate;
%rename cGate::destinationGate  getDestinationGate;
%rename cGate::displayString  getDisplayString;

%rename cPar::type  getType;
%rename cPar::prompt  getPrompt;
%rename cModulePar::ownerModule  getOwnerModule;

%rename cOutVector::valuesReceived  getValuesReceived;
%rename cOutVector::valuesStored  getValuesStored;
%rename cOutVector::tuple  getTuple;

// statistics classes
%rename samples getSamples;
%rename weights getWeights;
%rename sum getSum;
%rename sqrSum getSqrSum;
%rename min getMin;
%rename max getMax;
%rename mean getMean;
%rename stddev getStddev;
%rename variance getVariance;
%rename random getRandom;

%rename cException::errorCode getErrorCode;
%rename cException::message getMessage;
%rename cException::moduleClassName getModuleClassName;
%rename cException::moduleFullPath getModuleFullPath;
%rename cException::moduleID getModuleID;

// The BASECLASS(), DERIVEDCLASS() macros should come from the memorymgmt_xxx.i file
BASECLASS(cPolymorphic);
BASECLASS(cDisplayString);
BASECLASS(cEnvir);
BASECLASS(cException);
BASECLASS(cExpression);
BASECLASS(cSubModIterator);
BASECLASS(cVisitor);
BASECLASS(cXMLElement);
BASECLASS(std::vector<cXMLElement*>);
//BASECLASS(std::map<std::string,std::string>);
DERIVEDCLASS(cArray, cPolymorphic);
DERIVEDCLASS(cBasicChannel, cPolymorphic);
DERIVEDCLASS(cChannel, cPolymorphic);
DERIVEDCLASS(cChannelType, cPolymorphic);
DERIVEDCLASS(cCompoundModule, cPolymorphic);
DERIVEDCLASS(cDefaultList, cPolymorphic);
DERIVEDCLASS(cDoubleExpression, cExpression);
DERIVEDCLASS(cGate, cPolymorphic);
DERIVEDCLASS(cMessage, cPolymorphic);
DERIVEDCLASS(cModule, cPolymorphic);
DERIVEDCLASS(cModulePar, cPolymorphic);
DERIVEDCLASS(cModuleType, cPolymorphic);
DERIVEDCLASS(cNetworkType, cPolymorphic);
DERIVEDCLASS(cObject, cPolymorphic);
DERIVEDCLASS(cOutVector, cPolymorphic);
DERIVEDCLASS(cPar, cPolymorphic);
DERIVEDCLASS(cPolymorphic, cPolymorphic);
DERIVEDCLASS(cQueue, cPolymorphic);
DERIVEDCLASS(cRuntimeError, cException);
DERIVEDCLASS(cSimpleModule, cPolymorphic);
DERIVEDCLASS(cSimulation, cPolymorphic);
DERIVEDCLASS(cStatistic, cPolymorphic);
DERIVEDCLASS(cStdDev, cPolymorphic);
DERIVEDCLASS(cWeightedStdDev, cPolymorphic);


// now include all header files
%include "defs.h"
%include "cpolymorphic.h"
%include "cobject.h"
%include "cvisitor.h"
//%include "opp_string.h"
%include "random.h"
%include "distrib.h"
%include "cexception.h"
%include "cdefaultlist.h"
%include "csimul.h"
%include "ctypes.h"
%include "carray.h"
%include "cqueue.h"
//%include "cllist.h"
%include "globals.h"
%include "cpar.h"
%include "cgate.h"
%include "cmessage.h"
//%include "cpacket.h"
//%include "cmsgheap.h"
%include "cmodule.h"
%include "csimplemodule.h"
%include "cstat.h"
//%include "cdensity.h"
//%include "chist.h"
//%include "cvarhist.h"
//%include "cpsquare.h"
//%include "cksplit.h"
%include "coutvect.h"
//%include "cdetect.h"
//%include "ctopo.h"
//%include "cfsm.h"
//%include "protocol.h"
//%include "cenum.h"
//%include "cstruct.h"
%include "cchannel.h"
%include "cdispstr.h"
%include "cxmlelement.h"
//%include "cenvir.h" -- see cEnvir above

//%include "util.h" -- no need to wrap
//%include "macros.h" -- no need to wrap
//%include "cwatch.h" -- no need to wrap
//%include "cstlwatch.h" -- no need to wrap
//%include "onstartup.h" -- no need to wrap
//%include "envirext.h" -- no need to wrap
//%include "cconfig.h" -- no need to wrap
//%include "cstrtokenizer.h" -- no need to wrap
//%include "cscheduler.h" -- no need to wrap
//%include "compat.h" -- no need to wrap
//%include "cparsimcomm.h" -- no need to wrap
//%include "ccommbuffer.h" -- no need to wrap
//%include "crng.h" -- no need to wrap

%include "JSimpleModule.h"
%include "JMessage.h"


