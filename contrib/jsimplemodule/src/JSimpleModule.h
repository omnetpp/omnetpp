#ifndef _JAVASIMPLEMODULE_H_
#define _JAVASIMPLEMODULE_H_

#include <jni.h>
#include <stdio.h>
#include <assert.h>
#include <omnetpp.h>

/**
 * Implements a Java-based simple module.
 */
class JSimpleModule : public cSimpleModule
{
  protected:
    jobject javaObject;
    jmethodID numInitStagesMethod;
    jmethodID initializeStageMethod;
    jmethodID doHandleMessageMethod;
    jmethodID finishMethod;
    cMessage *msgToBeHandled;

  protected:
    void createJavaObject();

  public:
    JSimpleModule();
    virtual ~JSimpleModule();
    cMessage *retrieveMsgToBeHandled() {return msgToBeHandled;}  // helper for Java

  protected:
    virtual int numInitStages() const;
    virtual void initialize(int stage);
    virtual void handleMessage(cMessage *msg);
    virtual void finish();
};

#endif


