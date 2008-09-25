#ifndef _JMESSAGE_H_
#define _JMESSAGE_H_

#include <jni.h>
#include <stdio.h>
#include <assert.h>
#include <omnetpp.h>
#include "JUtil.h"


/**
 * Implements a message class that can be extended in Java.
 *
 * Data in the Java object can be easily accessed from C++ too,
 * using the methods inherited from JObjectAccess: getIntJavaField(),
 * getLongJavaField(), getStringJavaField(), setIntJavaField(), etc.
 */
class JMessage : public cMessage, public JObjectAccess
{
  protected:
    jobject javaPeer;
    mutable jmethodID cloneMethod;

  public:
    explicit JMessage(const char *name, int kind, int dummy);
    JMessage(const JMessage& msg);
    virtual ~JMessage();

    virtual cMessage *dup() const  {return new JMessage(*this);}
    virtual std::string info() const;
    virtual std::string detailedInfo() const;
    JMessage& operator=(const JMessage& msg);

    void swigSetJavaPeer(jobject msgObject);
    jobject swigJavaPeer() {return javaPeer;}
    static jobject swigJavaPeerOf(cMessage *object);

    // Also note methods inherited from JObjectAccess: getIntJavaField(), etc.
};

#endif


