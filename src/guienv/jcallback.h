//==========================================================================
//  JCALLBACK.H - part of
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

#ifndef __JCALLBACK_H
#define __JCALLBACK_H

#include <stdio.h>
#include "guienvdefs.h"
#include "omnetpp.h"
#include "jni.h"

class JCallback
{
    private:
        JNIEnv *jenv;
        jobject jcallbackobj;

        jmethodID simulationEvent_ID;

        jmethodID messageScheduled_ID;
        jmethodID messageCancelled_ID;
        jmethodID beginSend_ID;
        jmethodID messageSendDirect_ID;
        jmethodID messageSendHop_ID;
        jmethodID messageSendHop2_ID;
        jmethodID endSend_ID;
        jmethodID messageDeleted_ID;

        jmethodID moduleReparented_ID;
        jmethodID componentMethodBegin_ID;
        jmethodID componentMethodEnd_ID;
        jmethodID moduleCreated_ID;
        jmethodID moduleDeleted_ID;
        jmethodID gateCreated_ID;
        jmethodID gateDeleted_ID;
        jmethodID connectionCreated_ID;
        jmethodID connectionDeleted_ID;
        jmethodID displayStringChanged_ID;
        jmethodID undisposedObject_ID;
        jmethodID bubble_ID;
        jmethodID gets_ID;
        jmethodID idle_ID;

        class SWIGWrapper {
           private:
               JNIEnv *jenv;
               jclass clazz;
               jmethodID ctor;
           public:
               SWIGWrapper(JNIEnv *jenv, const char *jclassName);
               ~SWIGWrapper();
               jobject wrap(void *cptr);
        };

        SWIGWrapper cobjectClass;
        SWIGWrapper ccomponentClass;
        SWIGWrapper cmoduleClass;
        SWIGWrapper cmessageClass;
        SWIGWrapper cgateClass;


    protected:
        jmethodID getMethodID(jclass clazz, const char *name, const char *sig);
        void checkExceptions();

    public:
        JCallback(JNIEnv *jenv, jobject jcallbackobj);
        ~JCallback();

        void simulationEvent(cMessage *msg);

        void messageScheduled(cMessage *msg);
        void messageCancelled(cMessage *msg);
        void beginSend(cMessage *msg);
        void messageSendDirect(cMessage *msg, cGate *toGate, simtime_t propagationDelay, simtime_t transmissionDelay);
        void messageSendHop(cMessage *msg, cGate *srcGate);
        void messageSendHop(cMessage *msg, cGate *srcGate, simtime_t propagationDelay, simtime_t transmissionDelay);
        void endSend(cMessage *msg);
        void messageDeleted(cMessage *msg);

        void moduleReparented(cModule *module, cModule *oldparent);
        void componentMethodBegin(cComponent *from, cComponent *to, const char *method);
        void componentMethodEnd();
        void moduleCreated(cModule *newmodule);
        void moduleDeleted(cModule *module);
        void gateCreated(cGate *newgate);
        void gateDeleted(cGate *gate);
        void connectionCreated(cGate *srcgate);
        void connectionDeleted(cGate *srcgate);
        void displayStringChanged(cComponent *component);
        void undisposedObject(cObject *obj);
        void bubble(cModule *mod, const char *text);
        std::string gets(const char *prompt, const char *defaultreply);  //XXX putsmsg, askyesno, etc
        bool idle();
};

#endif


