#ifndef _JMESSAGE_H_
#define _JMESSAGE_H_

#include <jni.h>
#include <stdio.h>
#include <assert.h>
#include <omnetpp.h>


/**
 * Implements a message class that can be extended in Java
 */
class JMessage : public cMessage
{
  protected:
    jobject javaPeer;
    jmethodID cloneMethod;

  protected:
    void checkExceptions() const;
    template<typename T> T checkException(T a) const {checkExceptions(); return a;}
    void getMethodOrField(const char *fieldName, const char *methodPrefix,
                          const char *methodsig, const char *fieldsig,
                          jmethodID& methodID, jfieldID& fieldID) const;

  public:
    explicit JMessage(const char *name, int kind, int dummy);
    JMessage(const JMessage& msg);
    virtual ~JMessage();
    JMessage& operator=(const JMessage& msg);
    virtual cPolymorphic *dup() const  {return new JMessage(*this);}
    void swigSetJavaPeer(jobject msgObject);
    jobject swigJavaPeer() {return javaPeer;}
    static jobject swigJavaPeerOf(cPolymorphic *object);

    jboolean getBooleanJavaField(const char *fieldName) const;
    jbyte getByteJavaField(const char *fieldName) const;
    jchar getCharJavaField(const char *fieldName) const;
    jshort getShortJavaField(const char *fieldName) const;
    jint getIntJavaField(const char *fieldName) const;
    jlong getLongJavaField(const char *fieldName) const;
    jfloat getFloatJavaField(const char *fieldName) const;
    jdouble getDoubleJavaField(const char *fieldName) const;
    std::string getStringJavaField(const char *fieldName) const;

    void setBooleanJavaField(const char *fieldName, jboolean value);
    void setByteJavaField(const char *fieldName, jbyte value);
    void setCharJavaField(const char *fieldName, jchar value);
    void setShortJavaField(const char *fieldName, jshort value);
    void setIntJavaField(const char *fieldName, jint value);
    void setLongJavaField(const char *fieldName, jlong value);
    void setFloatJavaField(const char *fieldName, jfloat value);
    void setDoubleJavaField(const char *fieldName, jdouble value);
    void setStringJavaField(const char *fieldName, const char *value);

    //FIXME info() should return Java's toString()

};

#endif


