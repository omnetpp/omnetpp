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
#include "javaenvdefs.h"
#include "omnetpp.h"
#include "jni.h"

class JCallback
{
    private:
        JNIEnv *jenv;
        jobject jcallbackobj;

        jmethodID objectDeleted_ID;
        jmethodID simulationEvent_ID;

        jmethodID messageScheduled_ID;
        jmethodID messageCancelled_ID;
        jmethodID beginSend_ID;
        jmethodID messageSendDirect_ID;
        jmethodID messageSendHop1_ID;
        jmethodID messageSendHop3_ID;

        jmethodID idle_ID;
        jmethodID bubble_ID;
        jmethodID breakpointHit_ID;
        jmethodID moduleMethodCalled_ID;
        jmethodID moduleCreated_ID;
        jmethodID moduleDeleted_ID;
        jmethodID moduleReparented_ID;
        jmethodID connectionCreated_ID;
        jmethodID connectionRemoved_ID;
        jmethodID gateDisplayStringChanged_ID;
        jmethodID moduleDisplayStringChanged_ID;
        jmethodID backgroundDisplayStringChanged_ID;

        class SWIGWrapper {
           private:
               JNIEnv *jenv;
               const char *name; //XXX remove, once "stale jclass" mystery is solved
               jclass clazz;
               jmethodID ctor;
           public:
               SWIGWrapper(JNIEnv *jenv, const char *jclassName);
               jobject wrap(void *cptr);
        };

        SWIGWrapper cobjectClass;
        SWIGWrapper cmessageClass;
        SWIGWrapper cmoduleClass;
        SWIGWrapper cgateClass;


    protected:
        jmethodID getMethodID(jclass clazz, const char *name, const char *sig);
        void checkExceptions();

    public:
        JCallback(JNIEnv *jenv, jobject jcallbackobj);
        ~JCallback();

        void objectDeleted(cObject *object);
        void simulationEvent(cMessage *msg);

        void messageScheduled(cMessage *msg);
        void messageCancelled(cMessage *msg);
        void beginSend(cMessage *msg);
        void messageSendDirect(cMessage *msg, cGate *toGate, simtime_t propagationDelay, simtime_t transmissionDelay);
        void messageSendHop(cMessage *msg, cGate *srcGate);
        void messageSendHop(cMessage *msg, cGate *srcGate, simtime_t propagationDelay, simtime_t transmissionDelay);

        void endSend(cMessage *msg); //new!!!
        void messageDeleted(cMessage *msg);
        void moduleReparented(cModule *module, cModule *oldparent);
        void componentMethodBegin(cComponent *from, cComponent *to, const char *methodFmt, va_list va);
        void componentMethodEnd();

        void bubble(cModule *mod, const char *text);
        void breakpointHit(const char *lbl, cSimpleModule *mod);
        void moduleCreated(cModule *newmodule);
        void moduleDeleted(cModule *module);
        void moduleReparented(cModule *module, cModule *oldparent);
        void connectionCreated(cGate *srcgate);
        void connectionRemoved(cGate *srcgate);
        void displayStringChanged(cGate *gate);
        void displayStringChanged(cModule *submodule);
        void backgroundDisplayStringChanged(cModule *parentmodule);
};

#endif


