//==========================================================================
//  GUIENV.CC - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>

#include "appreg.h"
#include "guienv.h"


//
// Register the user interface
//
Register_OmnetApp("GUIEnv", GUIEnv, 30, "RCP-based graphical user interface");

JNIEnv *GUIEnv::jenv;
jobject GUIEnv::javaApp;


void GUIEnv::run()
{
    jclass clazz = jenv->GetObjectClass(javaApp);
    jmethodID doStartMethodID = jenv->GetMethodID(clazz, "doStart", "()V");
    if (doStartMethodID==NULL) {
        fprintf(stderr, "GUIEnv initialization failed: application object has no method doStart()\n");
        exit(1);
    }
    jenv->CallVoidMethod(javaApp, doStartMethodID);  // this goes back Java and runs the appliation. it only returns when the application exits
    jenv->DeleteGlobalRef(javaApp);
}

void GUIEnv::setJCallback(JNIEnv *jenv, jobject jcallbackobj)
{
    if (jcallbackobj)
    {
        jcallback = new JCallback(jenv, jcallbackobj);
    }
    else
    {
        delete jcallback;
        jcallback = NULL;
    }
}

void GUIEnv::objectDeleted(cObject *object)
{
    //cEnvirBase::objectDeleted(object);
    wrapperTable.objectDeleted(object);
    //if (jcallback) jcallback->objectDeleted(object);
}

/*XXX already in header
void GUIEnv::simulationEvent(cMessage *msg)
{
    //cEnvirBase::simulationEvent(msg);
    if (ev.disable_tracing) return; //XXX
    if (jcallback) jcallback->simulationEvent(msg);
}
*/

void GUIEnv::messageScheduled(cMessage *msg)
{
    //cEnvirBase::messageScheduled(msg);
    if (ev.disable_tracing) return; //XXX
    if (jcallback) jcallback->messageScheduled(msg);
}

void GUIEnv::messageCancelled(cMessage *msg)
{
    //cEnvirBase::messageCancelled(msg);
    if (ev.disable_tracing) return; //XXX
    if (jcallback) jcallback->messageCancelled(msg);
}

void GUIEnv::beginSend(cMessage *msg)
{
    //cEnvirBase::beginSend(msg);
    if (ev.disable_tracing) return; //XXX
    if (jcallback) jcallback->beginSend(msg);
}

void GUIEnv::messageSendDirect(cMessage *msg, cGate *toGate, simtime_t propagationDelay, simtime_t transmissionDelay)
{
    //cEnvirBase::messageSendDirect(msg, toGate, propagationDelay, transmissionDelay);
    if (ev.disable_tracing) return; //XXX
    if (jcallback) jcallback->messageSendDirect(msg, toGate, propagationDelay, transmissionDelay);
}

void GUIEnv::messageSendHop(cMessage *msg, cGate *srcGate)
{
    //cEnvirBase::messageSendHop(msg, srcGate);
    if (ev.disable_tracing) return; //XXX
    if (jcallback) jcallback->messageSendHop(msg, srcGate);
}

void GUIEnv::messageSendHop(cMessage *msg, cGate *srcGate, simtime_t propagationDelay, simtime_t transmissionDelay)
{
    //cEnvirBase::messageSendHop(msg, srcGate, propagationDelay, transmissionDelay, transmissionStartTime);
    if (ev.disable_tracing) return; //XXX
    if (jcallback) jcallback->messageSendHop(msg, srcGate, propagationDelay, transmissionDelay);
}

void GUIEnv::endSend(cMessage *msg)
{
    //cEnvirBase::endSend(msg);
    if (ev.disable_tracing) return; //XXX
    if (jcallback) jcallback->endSend(msg);
}

void GUIEnv::messageDeleted(cMessage *msg)
{
    //cEnvirBase::messageDeleted(msg);
    if (ev.disable_tracing) return; //XXX
    if (jcallback) jcallback->messageDeleted(msg);
}

void GUIEnv::moduleReparented(cModule *module, cModule *oldparent)
{
    //cEnvirBase::moduleReparented(module, oldparent);
    if (jcallback) jcallback->moduleReparented(module, oldparent);
}

void GUIEnv::componentMethodBegin(cComponent *from, cComponent *to, const char *methodFmt, va_list va)
{
const char *method = methodFmt; //FIXME vsprintf()
    //cEnvirBase::componentMethodBegin(from, to, method);
    if (ev.disable_tracing) return; //XXX
    if (jcallback) jcallback->componentMethodBegin(from, to, method);
}

void GUIEnv::componentMethodEnd()
{
    //cEnvirBase::componentMethodEnd();
    if (ev.disable_tracing) return; //XXX
    if (jcallback) jcallback->componentMethodEnd();
}

void GUIEnv::moduleCreated(cModule *newmodule)
{
    //cEnvirBase::moduleCreated(newmodule);
    if (jcallback) jcallback->moduleCreated(newmodule);
}

void GUIEnv::moduleDeleted(cModule *module)
{
    //cEnvirBase::moduleDeleted(module);
    if (jcallback) jcallback->moduleDeleted(module);
}

void GUIEnv::gateCreated(cGate *srcgate)
{
    //cEnvirBase::gateCreated(srcgate);
    if (jcallback) jcallback->gateCreated(srcgate);
}

void GUIEnv::gateDeleted(cGate *srcgate)
{
    //cEnvirBase::gateDeleted(srcgate);
    if (jcallback) jcallback->gateDeleted(srcgate);
}

void GUIEnv::connectionCreated(cGate *srcgate)
{
    //cEnvirBase::connectionCreated(srcgate);
    if (jcallback) jcallback->connectionCreated(srcgate);
}

void GUIEnv::connectionDeleted(cGate *srcgate)
{
    //cEnvirBase::connectionDeleted(srcgate);
    if (jcallback) jcallback->connectionDeleted(srcgate);
}

void GUIEnv::displayStringChanged(cComponent *component)
{
    //cEnvirBase::displayStringChanged(component);
    if (jcallback) jcallback->displayStringChanged(component);
}

void GUIEnv::undisposedObject(cObject *obj)
{
    //cEnvirBase::undisposedObject(component);
    if (jcallback) jcallback->undisposedObject(obj);
}

void GUIEnv::bubble(cModule *mod, const char *text)
{
    //cEnvirBase::bubble(mod, text);
    if (jcallback) jcallback->bubble(mod, text);
}

std::string GUIEnv::gets(const char *prompt, const char *defaultreply)
{
    ASSERT(jcallback);
    return jcallback->gets(prompt, defaultreply);
}

bool GUIEnv::idle()
{
    bool stop = false;
    if (jcallback)
        stop = jcallback->idle();
    return stop;
}

