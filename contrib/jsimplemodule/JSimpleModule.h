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
  public:
    static JavaVM *vm;
    static JNIEnv *jenv;

  protected:
    jobject javaObject;
    jmethodID numInitStagesMethod;
    jmethodID initializeStageMethod;
    jmethodID doHandleMessageMethod;
    jmethodID finishMethod;
    cMessage *msgToBeHandled;

  protected:
    void createJavaObject();
    void initJVM();
    void checkExceptions() const;
    jmethodID findMethod(jclass clazz, const char *clazzName, const char *methodName, const char *methodSig);

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


