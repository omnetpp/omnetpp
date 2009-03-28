//==========================================================================
//  JCALLBACK.CC - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  contains:  OmnetTkApp member functions
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#include <assert.h>
#include "jcallback.h"


#define PKGPREFIX "org/omnetpp/experimental/simkernel/swig/"

JCallback::SWIGWrapper::SWIGWrapper(JNIEnv *jenv, const char *jclassName)
{
name = jclassName; //XXX remove, once "stale jclass" mystery is solved
    this->jenv = jenv;
    clazz = jenv->FindClass(jclassName);
    ctor = jenv->GetMethodID(clazz, "<init>", "(JZ)V");
    if (!clazz || !ctor) {
        fprintf(stderr, "JCallback initialization failed: class or its ctor not found: %s\n", jclassName);
        exit(1);
    }
}

jobject JCallback::SWIGWrapper::wrap(void *cptr)
{
    if (cptr==NULL) return 0;

    clazz = jenv->FindClass(name); //XXX remove, once "stale jclass" mystery is solved
    ctor = jenv->GetMethodID(clazz, "<init>", "(JZ)V"); //XXX remove, once "stale jclass" mystery is solved

    return jenv->NewObject(clazz, ctor, (jlong)cptr, false);
}


JCallback::JCallback(JNIEnv *jenv, jobject jcallbackobj) :
    cobjectClass(jenv, PKGPREFIX "cObject"),
    cmessageClass(jenv, PKGPREFIX "cMessage"),
    cmoduleClass(jenv, PKGPREFIX "cModule"),
    cgateClass(jenv, PKGPREFIX "cGate")
{
    assert(jcallbackobj!=NULL);
    this->jenv = jenv;
    this->jcallbackobj = jenv->NewGlobalRef(jcallbackobj);

    jclass clazz = jenv->GetObjectClass(jcallbackobj);

#define OBJ  "L" PKGPREFIX "cObject;"
#define MOD  "L" PKGPREFIX "cModule;"
#define MSG  "L" PKGPREFIX "cMessage;"
#define GAT  "L" PKGPREFIX "cGate;"
#define STR  "Ljava/lang/String;"

    objectDeleted_ID = jenv->GetMethodID(clazz, "objectDeleted", "(" OBJ ")V");
    simulationEvent_ID = jenv->GetMethodID(clazz, "simulationEvent", "(" MSG ")V");

    messageScheduled_ID = jenv->GetMethodID(clazz, "messageScheduled", "(" MSG ")V");
    messageCancelled_ID = jenv->GetMethodID(clazz, "messageCancelled", "(" MSG ")V");
    beginSend_ID = jenv->GetMethodID(clazz, "beginSend", "(" MSG ")V");
    messageSendDirect_ID = jenv->GetMethodID(clazz, "messageSendDirect", "(" MSG GAT "DD)V");
    messageSendHop1_ID = jenv->GetMethodID(clazz, "messageSendHop", "(" MSG GAT "D)V");
    messageSendHop3_ID = jenv->GetMethodID(clazz, "messageSendHop", "(" MSG GAT "DDD)V");

    idle_ID = jenv->GetMethodID(clazz, "idle", "()Z");
    bubble_ID = jenv->GetMethodID(clazz, "bubble", "(" MOD STR ")V");
    breakpointHit_ID = jenv->GetMethodID(clazz, "breakpointHit", "(" STR MOD ")V");
    moduleMethodCalled_ID = jenv->GetMethodID(clazz, "moduleMethodCalled", "(" MOD MOD STR ")V");
    moduleCreated_ID = jenv->GetMethodID(clazz, "moduleCreated", "(" MOD ")V");
    moduleDeleted_ID = jenv->GetMethodID(clazz, "moduleDeleted", "(" MOD ")V");
    moduleReparented_ID = jenv->GetMethodID(clazz, "moduleReparented", "(" MOD MOD ")V");
    connectionCreated_ID = jenv->GetMethodID(clazz, "connectionCreated", "(" GAT ")V");
    connectionRemoved_ID = jenv->GetMethodID(clazz, "connectionRemoved", "(" GAT ")V");
    gateDisplayStringChanged_ID = jenv->GetMethodID(clazz, "displayStringChanged", "(" GAT ")V");
    moduleDisplayStringChanged_ID = jenv->GetMethodID(clazz, "displayStringChanged", "(" MOD ")V");
    backgroundDisplayStringChanged_ID = jenv->GetMethodID(clazz, "backgroundDisplayStringChanged", "(" MOD ")V");

#undef OBJ
#undef MOD
#undef MSG
#undef GAT
}

JCallback::~JCallback()
{
    jenv->DeleteGlobalRef(jcallbackobj);
}

jmethodID JCallback::getMethodID(jclass clazz, const char *name, const char *sig)
{
    jmethodID ret = jenv->GetMethodID(clazz, name, sig);
    if (ret==NULL) {
        fprintf(stderr, "JCallback initialization failed: callback object has no method %s%s\n", name, sig);
        exit(1);
    }
    return ret;
}

void JCallback::checkExceptions()
{
    if (jenv->ExceptionOccurred())
    {
        fprintf(stderr, "JCallback: exception occurred:\n");
        jenv->ExceptionDescribe();
        exit(1);
    }
}


void JCallback::objectDeleted(cObject *object)
{
    //XXX this crashes?
    // TRACE("objectDeleted");
    // jenv->CallVoidMethod(jcallbackobj, objectDeleted_ID, cobjectClass.wrap(object));
    // checkExceptions();
}

void JCallback::simulationEvent(cMessage *msg)
{
    TRACE("simulationEvent");
    jenv->CallVoidMethod(jcallbackobj, simulationEvent_ID, cmessageClass.wrap(msg));
    checkExceptions();
}

void JCallback::messageScheduled(cMessage *msg)
{
    TRACE("messageScheduled");
    jenv->CallVoidMethod(jcallbackobj, messageScheduled_ID, cmessageClass.wrap(msg));
    checkExceptions();
}

void JCallback::messageCancelled(cMessage *msg)
{
    TRACE("messageCancelled");
    jenv->CallVoidMethod(jcallbackobj, messageCancelled_ID, cmessageClass.wrap(msg));
    checkExceptions();
}

void JCallback::beginSend(cMessage *msg)
{
    TRACE("beginSend");
    jenv->CallVoidMethod(jcallbackobj, beginSend_ID, cmessageClass.wrap(msg));
    checkExceptions();
}

void JCallback::messageSendDirect(cMessage *msg, cGate *toGate, simtime_t propagationDelay, simtime_t transmissionDelay)
{
    TRACE("messageSendDirect");
    jenv->CallVoidMethod(jcallbackobj, messageSendDirect_ID, cmessageClass.wrap(msg), cgateClass.wrap(toGate), (jdouble)propagationDelay, (jdouble)transmissionDelay);
    checkExceptions();
}

void JCallback::messageSendHop(cMessage *msg, cGate *srcGate, simtime_t propagationDelay)
{
//__asm int 3;
//
//jclass clazz = jenv->FindClass(PKGPREFIX "cGate");
//jmethodID ctor = jenv->GetMethodID(clazz, "<init>", "(JZ)V");
//jenv->NewObject(clazz, ctor, (jlong)(void *)srcGate, false);
//
//jclass clazz2 = jenv->FindClass(PKGPREFIX "cGate");
//jclass clazz3 = jenv->FindClass(PKGPREFIX "cGate");
//jenv->NewObject(clazz3, ctor, (jlong)(void *)srcGate, false);
//
//SWIGWrapper x(jenv, PKGPREFIX "cGate");
//x.wrap(srcGate);
//
//cgateClass.wrap(srcGate);

    TRACE("messageSendHop");
    jenv->CallVoidMethod(jcallbackobj, messageSendHop1_ID, cmessageClass.wrap(msg), cgateClass.wrap(srcGate), (jdouble)propagationDelay);
    checkExceptions();
}

void JCallback::messageSendHop(cMessage *msg, cGate *srcGate, simtime_t propagationDelay, simtime_t transmissionDelay, simtime_t transmissionStartTime)
{
    TRACE("messageSendHop");
    jenv->CallVoidMethod(jcallbackobj, messageSendHop3_ID, cmessageClass.wrap(msg), cgateClass.wrap(srcGate), (jdouble)propagationDelay, (jdouble)transmissionDelay, (jdouble)transmissionStartTime);
    checkExceptions();
}

bool JCallback::idle()
{
    TRACE("idle");
    bool ret = jenv->CallBooleanMethod(jcallbackobj, idle_ID);
    checkExceptions();
    return ret;
}

void JCallback::bubble(cModule *mod, const char *text)
{
    TRACE("bubble");
    jenv->CallVoidMethod(jcallbackobj, bubble_ID, cmoduleClass.wrap(mod), jenv->NewStringUTF(text));
    checkExceptions();
}

void JCallback::breakpointHit(const char *lbl, cSimpleModule *mod)
{
    TRACE("breakpointHit");
    jenv->CallVoidMethod(jcallbackobj, breakpointHit_ID, jenv->NewStringUTF(lbl), cmoduleClass.wrap((cModule*)mod));
    checkExceptions();
}

void JCallback::moduleMethodCalled(cModule *from, cModule *to, const char *method)
{
    TRACE("moduleMethodCalled");
    jenv->CallVoidMethod(jcallbackobj, moduleMethodCalled_ID, cmoduleClass.wrap(from), cmoduleClass.wrap(to), jenv->NewStringUTF(method));
    checkExceptions();
}

void JCallback::moduleCreated(cModule *newmodule)
{
    TRACE("moduleCreated");
    jenv->CallVoidMethod(jcallbackobj, moduleCreated_ID, cmoduleClass.wrap(newmodule));
    checkExceptions();
}

void JCallback::moduleDeleted(cModule *module)
{
    TRACE("moduleDeleted");
    jenv->CallVoidMethod(jcallbackobj, moduleDeleted_ID, cmoduleClass.wrap(module));
    checkExceptions();
}

void JCallback::moduleReparented(cModule *module, cModule *oldparent)
{
    TRACE("moduleReparented");
    jenv->CallVoidMethod(jcallbackobj, moduleReparented_ID, cmoduleClass.wrap(module), cmoduleClass.wrap(oldparent));
    checkExceptions();
}

void JCallback::connectionCreated(cGate *srcgate)
{
    TRACE("connectionCreated");
    jenv->CallVoidMethod(jcallbackobj, connectionCreated_ID, cgateClass.wrap(srcgate));
    checkExceptions();
}

void JCallback::connectionRemoved(cGate *srcgate)
{
    TRACE("connectionRemoved");
    jenv->CallVoidMethod(jcallbackobj, connectionRemoved_ID, cgateClass.wrap(srcgate));
    checkExceptions();
}

void JCallback::displayStringChanged(cGate *gate)
{
    TRACE("displayStringChanged");
    jenv->CallVoidMethod(jcallbackobj, gateDisplayStringChanged_ID, cgateClass.wrap(gate));
    checkExceptions();
}

void JCallback::displayStringChanged(cModule *submodule)
{
    TRACE("displayStringChanged");
    jenv->CallVoidMethod(jcallbackobj, moduleDisplayStringChanged_ID, cmoduleClass.wrap(submodule));
    checkExceptions();
}

void JCallback::backgroundDisplayStringChanged(cModule *parentmodule)
{
    TRACE("backgroundDisplayStringChanged");
    jenv->CallVoidMethod(jcallbackobj, backgroundDisplayStringChanged_ID, cmoduleClass.wrap(parentmodule));
    checkExceptions();
}

