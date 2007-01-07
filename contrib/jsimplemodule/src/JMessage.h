#ifndef _JMESSAGE_H_
#define _JMESSAGE_H_

#include <jni.h>
#include <stdio.h>
#include <assert.h>
#include <omnetpp.h>
#include "JUtil.h"


/**
 * Implements a message class that can be extended in Java
 */
class JMessage : public cMessage, public JObjectAccess
{
  protected:
    jobject javaPeer;
    mutable jmethodID cloneMethod;

  protected:
    void getMethodOrField(const char *fieldName, const char *methodPrefix,
                          const char *methodsig, const char *fieldsig,
                          jmethodID& methodID, jfieldID& fieldID) const;

  public:
    explicit JMessage(const char *name, int kind, int dummy);
    JMessage(const JMessage& msg);
    virtual ~JMessage();
    JMessage& operator=(const JMessage& msg);
    virtual cPolymorphic *dup() const  {return new JMessage(*this);}
    std::string info() const;
    std::string detailedInfo() const;
    void swigSetJavaPeer(jobject msgObject);
    jobject swigJavaPeer() {return javaPeer;}
    static jobject swigJavaPeerOf(cPolymorphic *object);
};

#endif


