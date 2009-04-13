//==========================================================================
//  JCALLBACK.CC - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  contains:  cEnvir Java callback
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

#define SIMTIME_JWRAP(x) (jdouble)SIMTIME_DBL(x)

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
    ccomponentClass(jenv, PKGPREFIX "cComponent"),
    cmoduleClass(jenv, PKGPREFIX "cModule"),
    cmessageClass(jenv, PKGPREFIX "cMessage"),
    cgateClass(jenv, PKGPREFIX "cGate")
{
    assert(jcallbackobj!=NULL);
    this->jenv = jenv;
    this->jcallbackobj = jenv->NewGlobalRef(jcallbackobj);

    jclass clazz = jenv->GetObjectClass(jcallbackobj);

#define T    "D"  //XXX use "L" PKGPREFIX "SimTime;"
#define OBJ  "L" PKGPREFIX "cObject;"
#define COM  "L" PKGPREFIX "cComponent;"
#define MOD  "L" PKGPREFIX "cModule;"
#define MSG  "L" PKGPREFIX "cMessage;"
#define GAT  "L" PKGPREFIX "cGate;"
#define STR  "Ljava/lang/String;"

    objectDeleted_ID = jenv->GetMethodID(clazz, "objectDeleted", "(" OBJ ")V");
    simulationEvent_ID = jenv->GetMethodID(clazz, "simulationEvent", "(" MSG ")V");

    messageScheduled_ID = jenv->GetMethodID(clazz, "messageScheduled", "(" MSG ")V");
    messageCancelled_ID = jenv->GetMethodID(clazz, "messageCancelled", "(" MSG ")V");
    beginSend_ID = jenv->GetMethodID(clazz, "beginSend", "(" MSG ")V");
    messageSendDirect_ID = jenv->GetMethodID(clazz, "messageSendDirect", "(" MSG GAT T T ")V");
    messageSendHop_ID = jenv->GetMethodID(clazz, "messageSendHop", "(" MSG GAT ")V");
    messageSendHop2_ID = jenv->GetMethodID(clazz, "messageSendHop", "(" MSG GAT T T ")V");
    endSend_ID = jenv->GetMethodID(clazz, "endSend", "(" MSG ")V");
    messageDeleted_ID = jenv->GetMethodID(clazz, "messageDeleted", "(" MSG ")V");

    moduleReparented_ID = jenv->GetMethodID(clazz, "moduleReparented", "(" MOD MOD ")V");
    componentMethodBegin_ID = jenv->GetMethodID(clazz, "gateCreated", "(" COM COM STR ")V");
    componentMethodEnd_ID = jenv->GetMethodID(clazz, "componentMethodEnd", "()V");
    moduleCreated_ID = jenv->GetMethodID(clazz, "moduleCreated", "(" MOD ")V");
    moduleDeleted_ID = jenv->GetMethodID(clazz, "moduleDeleted", "(" MOD ")V");
    gateCreated_ID = jenv->GetMethodID(clazz, "gateCreated", "(" GAT ")V");
    gateDeleted_ID = jenv->GetMethodID(clazz, "gateDeleted", "(" GAT ")V");
    connectionCreated_ID = jenv->GetMethodID(clazz, "connectionCreated", "(" GAT ")V");
    connectionDeleted_ID = jenv->GetMethodID(clazz, "connectionDeleted", "(" GAT ")V");
    displayStringChanged_ID = jenv->GetMethodID(clazz, "displayStringChanged", "(" COM ")V");
    undisposedObject_ID = jenv->GetMethodID(clazz, "undisposedObject", "(" OBJ ")V");
    bubble_ID = jenv->GetMethodID(clazz, "bubble", "(" MOD STR ")V");
    gets_ID = jenv->GetMethodID(clazz, "gets", "(" STR STR ")" STR);
    idle_ID = jenv->GetMethodID(clazz, "idle", "()Z");

#undef T
#undef OBJ
#undef COM
#undef MOD
#undef MSG
#undef GAT
#undef STR
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
    TRACE("objectDeleted");
    jenv->CallVoidMethod(jcallbackobj, objectDeleted_ID, cobjectClass.wrap(object));
    checkExceptions();
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
    jenv->CallVoidMethod(jcallbackobj, messageSendDirect_ID, cmessageClass.wrap(msg), cgateClass.wrap(toGate), SIMTIME_JWRAP(propagationDelay), SIMTIME_JWRAP(transmissionDelay));
    checkExceptions();
}

void JCallback::messageSendHop(cMessage *msg, cGate *srcGate)
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
    jenv->CallVoidMethod(jcallbackobj, messageSendHop_ID, cmessageClass.wrap(msg), cgateClass.wrap(srcGate));
    checkExceptions();
}

void JCallback::messageSendHop(cMessage *msg, cGate *srcGate, simtime_t propagationDelay, simtime_t transmissionDelay)
{
    TRACE("messageSendHop");
    jenv->CallVoidMethod(jcallbackobj, messageSendHop2_ID, cmessageClass.wrap(msg), cgateClass.wrap(srcGate), SIMTIME_JWRAP(propagationDelay), SIMTIME_JWRAP(transmissionDelay));
    checkExceptions();
}

void JCallback::endSend(cMessage *msg)
{
    TRACE("endSend");
    jenv->CallVoidMethod(jcallbackobj, endSend_ID, cmessageClass.wrap(msg));
    checkExceptions();
}

void JCallback::messageDeleted(cMessage *msg)
{
    TRACE("messageDeleted");
    jenv->CallVoidMethod(jcallbackobj, messageDeleted_ID, cmessageClass.wrap(msg));
    checkExceptions();
}

void JCallback::moduleReparented(cModule *module, cModule *oldparent)
{
    TRACE("moduleReparented");
    jenv->CallVoidMethod(jcallbackobj, moduleReparented_ID, cmoduleClass.wrap(module), cmoduleClass.wrap(oldparent));
    checkExceptions();
}

void JCallback::componentMethodBegin(cComponent *from, cComponent *to, const char *method)
{
    TRACE("componentMethodBegin");
    jenv->CallVoidMethod(jcallbackobj, componentMethodBegin_ID, ccomponentClass.wrap(from), ccomponentClass.wrap(to), jenv->NewStringUTF(method));
    checkExceptions();
}

void JCallback::componentMethodEnd()
{
    TRACE("componentMethodEnd");
    jenv->CallVoidMethod(jcallbackobj, componentMethodEnd_ID);
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

void JCallback::gateCreated(cGate *newGate)
{
    TRACE("gateCreated");
    jenv->CallVoidMethod(jcallbackobj, gateCreated_ID, cgateClass.wrap(newGate));
    checkExceptions();
}

void JCallback::gateDeleted(cGate *gate)
{
    TRACE("gateDeleted");
    jenv->CallVoidMethod(jcallbackobj, gateDeleted_ID, cgateClass.wrap(gate));
    checkExceptions();
}

void JCallback::connectionCreated(cGate *srcgate)
{
    TRACE("connectionCreated");
    jenv->CallVoidMethod(jcallbackobj, connectionCreated_ID, cgateClass.wrap(srcgate));
    checkExceptions();
}

void JCallback::connectionDeleted(cGate *srcgate)
{
    TRACE("connectionDeleted");
    jenv->CallVoidMethod(jcallbackobj, connectionDeleted_ID, cgateClass.wrap(srcgate));
    checkExceptions();
}

void JCallback::displayStringChanged(cComponent *component)
{
    TRACE("displayStringChanged");
    jenv->CallVoidMethod(jcallbackobj, displayStringChanged_ID, ccomponentClass.wrap(component));
    checkExceptions();
}

void JCallback::undisposedObject(cObject *obj)
{
    TRACE("undisposedObject");
    jenv->CallVoidMethod(jcallbackobj, undisposedObject_ID, cobjectClass.wrap(obj));
    checkExceptions();
}

void JCallback::bubble(cModule *mod, const char *text)
{
    TRACE("bubble");
    jenv->CallVoidMethod(jcallbackobj, bubble_ID, cmoduleClass.wrap(mod), jenv->NewStringUTF(text));
    checkExceptions();
}

std::string JCallback::gets(const char *prompt, const char *defaultreply)
{
    TRACE("gets");
    jstring str = (jstring)jenv->CallObjectMethod(jcallbackobj, gets_ID, jenv->NewStringUTF(prompt), jenv->NewStringUTF(defaultreply));

    jsize len = jenv->GetStringLength(str);
    const jchar *chars = jenv->GetStringChars(str, NULL);
    std::string result((const char *)chars, len);
    jenv->ReleaseStringChars(str, chars);

    checkExceptions();
    return result;
}


bool JCallback::idle()
{
    TRACE("idle");
    bool ret = jenv->CallBooleanMethod(jcallbackobj, idle_ID);
    checkExceptions();
    return ret;
}

