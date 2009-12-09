#ifndef _JAVASIMPLEMODULE_H_
#define _JAVASIMPLEMODULE_H_

#include <jni.h>
#include <stdio.h>
#include <assert.h>
#include <omnetpp.h>
#include "JUtil.h"

/**
 * Implements a Java-based simple module. It instantiates the Java class
 * based on the module's fully qualified name, and delegates handleMessage()
 * and other methods to it.
 *
 * From JObjectAccess it inherits methods that facilitate accessing data
 * members of the Java class, should it become necessary: getIntJavaField(),
 * getLongJavaField(), getStringJavaField(), setIntJavaField(), etc.
 */
class JSimpleModule : public cSimpleModule, public JObjectAccess
{
  protected:
    jobject javaPeer;
    jmethodID numInitStagesMethod;
    jmethodID initializeStageMethod;
    jmethodID doHandleMessageMethod;
    jmethodID finishMethod;
    cMessage *msgToBeHandled;

  protected:
    void createJavaModuleObject();

  public:
    JSimpleModule();
    virtual ~JSimpleModule();
    cMessage *retrieveMsgToBeHandled() {return msgToBeHandled;}  // helper for Java

    jobject swigJavaPeer() {
         if (javaPeer==0) createJavaModuleObject();
         return javaPeer;
    }
    static jobject swigJavaPeerOf(cModule *object);

  protected:
    virtual int numInitStages() const;
    virtual void initialize(int stage);
    virtual void handleMessage(cMessage *msg);
    virtual void finish();
};

#endif


