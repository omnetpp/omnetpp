//==========================================================================
//  JAVAENV.H - part of
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

#ifndef __JAVAENV_H
#define __JAVAENV_H


#include <vector>
#include <list>
#include <stdio.h>
#include "envirbase.h"
#include "logbuffer.h"
#include "jcallback.h"


#define MAX_OBJECTFULLPATH 1024
#define MAX_CLASSNAME      100

#define LL  INT64_PRINTF_FORMAT

class Javaenv : public EnvirBase
{
  private:
    static JNIEnv *jenv;
    static jobject javaApp;

    LogBuffer logBuffer;
    JCallback *jcallback;

  public:
    static void setJavaApplication(JNIEnv *jenv_, jobject javaApp_) {
        jenv = jenv_;
        javaApp = jenv->NewGlobalRef(javaApp_);
    }

    Javaenv() : EnvirBase() {
        jcallback = NULL;
    }

    ~Javaenv() {
        delete jcallback;
    }

    void run();

    void setJCallback(JNIEnv *jenv, jobject jcallbackobj);

    void putsmsg(const char *s) {printf("<!> %s\n",s); fflush(stdout);}
    bool askyesno(const char *s) {printf("%s? NO!\n",s); return false;}
    void messageSent_OBSOLETE(cMessage *,cGate *) {}
    size_t getExtraStackForEnvir() const {return 65536;}
    bool isGUI(void) const {return true;}
    cEnvir& flush() {return *this;}
    void printUISpecificHelp() {}
    void askParameter(cPar *p) {ASSERT(false);}  //FIXME

    void readPerRunOptions()  { EnvirBase::readPerRunOptions();}  // public so that we can call it from Java

    void startRun() {EnvirBase::startRun();}  //wrap to make public
    void endRun() {EnvirBase::endRun();}      //wrap to make public
    cModuleType *resolveNetwork(const char *networkname) {return EnvirBase::resolveNetwork(networkname);} //wrap to make public

/*
    void readParameter(cPar *par) {
        if (par->containsValue())
            par->acceptDefault();
        else
            throw cRuntimeError("no value for parameter %s", par->getFullPath().c_str());
    }
*/

    LogBuffer *getLogBuffer() {
        return &logBuffer;
    }

    virtual void simulationEvent(cMessage *msg) {
        cModule *module = simulation.getContextModule();
        printEventBanner(msg, module);
        if (jcallback) jcallback->simulationEvent(msg);
    }

    // method copied from 4.0 Tkenv
    void printEventBanner(cMessage *msg, cModule *module) {
        bool opt_short_banners = false; //FIXME

        // produce banner text
        char banner[2*MAX_OBJECTFULLPATH+2*MAX_CLASSNAME+60];
        if (opt_short_banners)
            sprintf(banner,"** Event #%"LL"d  T=%s  %s, on `%s'",
                    simulation.getEventNumber(),
                    SIMTIME_STR(simulation.getSimTime()),
                    module->getFullPath().c_str(),
                    msg->getFullName()
                  );
        else
            sprintf(banner,"** Event #%"LL"d  T=%s  %s (%s, id=%d), on %s`%s' (%s, id=%ld)",
                    simulation.getEventNumber(),
                    SIMTIME_STR(simulation.getSimTime()),
                    module->getFullPath().c_str(),
                    module->getComponentType()->getName(),
                    module->getId(),
                    (msg->isSelfMessage() ? "selfmsg " : ""),
                    msg->getFullName(),
                    msg->getClassName(),
                    msg->getId()
                  );

        // insert into log buffer
        logBuffer.addEvent(simulation.getEventNumber(), simulation.getSimTime(), module, banner);
    }

    virtual void sputn(const char *s, int n) {
        EnvirBase::sputn(s, n); fflush(stdout); //XXX just now
        (void) ::fwrite(s,1,n,stdout);
        cModule *module = simulation.getContextModule();
        // note: we must strip the trailing "\n"
        if (module)
            logBuffer.addLogLine(std::string(s,n-1).c_str()); //FIXME too much copying! reuse original string if no quoting needed
        else
            logBuffer.addInfo(std::string(s,n-1).c_str()); //FIXME too much copying! reuse original string if no quoting needed
    }


    void objectDeleted(cObject *object);
    //XXX defined above: void simulationEvent(cMessage *msg);

    void messageScheduled(cMessage *msg);
    void messageCancelled(cMessage *msg);
    void beginSend(cMessage *msg);
    void messageSendDirect(cMessage *msg, cGate *toGate, simtime_t propagationDelay, simtime_t transmissionDelay);
    void messageSendHop(cMessage *msg, cGate *srcGate);
    void messageSendHop(cMessage *msg, cGate *srcGate, simtime_t propagationDelay, simtime_t transmissionDelay);
    void endSend(cMessage *msg);
    void messageDeleted(cMessage *msg);

    void moduleReparented(cModule *module, cModule *oldparent);
    void componentMethodBegin(cComponent *from, cComponent *to, const char *methodFmt, va_list va);
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
    std::string gets(const char *prompt, const char *defaultreply);
    bool idle();

};

#endif


