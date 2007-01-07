#ifndef _JUTIL_H_
#define _JUTIL_H_

#include <jni.h>
#include <stdio.h>
#include <assert.h>
#include <omnetpp.h>

/**
 * Convenience methods for working with JNI.
 */
namespace JUtil
{
  extern JavaVM *vm;
  extern JNIEnv *jenv;

  void initJVM();
  std::string fromJavaString(jstring stringObject);
  jmethodID findMethod(jclass clazz, const char *clazzName, const char *methodName,
                       const char *methodSig);
  void checkExceptions();
  template<typename T> T checkException(T a)  {checkExceptions(); return a;}
};


/**
 * Makes it convenient to access members of a Java class from C++.
 */
class JObjectAccess
{
  protected:
    jobject javaPeer;
    mutable jmethodID toStringMethod;

  protected:
    void getMethodOrField(const char *fieldName, const char *methodPrefix,
                          const char *methodsig, const char *fieldsig,
                          jmethodID& methodID, jfieldID& fieldID) const;

  public:
    explicit JObjectAccess(jobject object=0);
    void setObject(jobject object);
    std::string toString() const;

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
};

#endif


