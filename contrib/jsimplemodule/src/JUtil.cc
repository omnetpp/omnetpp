#include "JUtil.h"
#include "JSimpleModule.h"

//#define DEBUGPRINTF printf
#define DEBUGPRINTF (void)

#define JENV   JSimpleModule::jenv


std::string fromJavaString(jstring stringObject)
{
    if (!stringObject)
        return "<null>";
    jboolean isCopy;
    const char *buf = JENV->GetStringUTFChars(stringObject, &isCopy);
    std::string str = buf ? buf : "";
    JENV->ReleaseStringUTFChars(stringObject, buf);
    return str;
}

void checkExceptions()
{
    jthrowable exceptionObject = JENV->ExceptionOccurred();
    if (exceptionObject)
    {
        DEBUGPRINTF("JObjectAccess: exception occurred:\n");
        JENV->ExceptionDescribe();
        JENV->ExceptionClear();

        jclass throwableClass = JENV->GetObjectClass(exceptionObject);
        jmethodID method = JENV->GetMethodID(throwableClass, "toString", "()Ljava/lang/String;");
        jstring msg = (jstring)JENV->CallObjectMethod(exceptionObject, method);
        opp_error(eCUSTOM, fromJavaString(msg).c_str());
    }
}

//---

JObjectAccess::JObjectAccess(jobject object)
{
    javaPeer = object;
    toStringMethod = 0;
}

std::string JObjectAccess::toString() const
{
    if (!toStringMethod)
    {
        jclass clazz = JENV->GetObjectClass(javaPeer);
        toStringMethod = JENV->GetMethodID(clazz, "toString", "()Ljava/lang/String;");
        checkExceptions();
    }
    jstring stringObject = (jstring)JENV->CallObjectMethod(javaPeer, toStringMethod);
    checkExceptions();
    return fromJavaString(stringObject);
}


void JObjectAccess::getMethodOrField(const char *fieldName, const char *methodPrefix,
                                const char *methodsig, const char *fieldsig,
                                jmethodID& methodID, jfieldID& fieldID) const
{
    if (!fieldName || !fieldName[0] || strlen(fieldName)>80)
        opp_error("field name empty or too long: `%s'", fieldName);
    char methodName[100];
    strcpy(methodName, methodPrefix);
    char *p = methodName + strlen(methodName);
    *p = toupper(fieldName[0]);
    strcpy(p+1, fieldName+1);

    jclass clazz = JENV->GetObjectClass(javaPeer);
    checkExceptions();
    fieldID = 0;
    methodID = JENV->GetMethodID(clazz, methodName, methodsig);
    if (methodID)
        return;
    JENV->ExceptionClear();
    fieldID = JENV->GetFieldID(clazz, fieldName, fieldsig);
    if (fieldID)
        return;
    JENV->ExceptionClear();
    opp_error("Java object has neither method `%s' nor field `%s' with the given type",
              methodName, fieldName);
}

#define GETTER_SETTER(Type, jtype, CODE)  \
  jtype JObjectAccess::get##Type##JavaField(const char *fieldName) const \
  { \
      jmethodID methodID; jfieldID fieldID; \
      getMethodOrField(fieldName, "get", "()" CODE, CODE, methodID, fieldID); \
      return checkException(methodID ? JENV->Call##Type##Method(javaPeer, methodID) : JENV->Get##Type##Field(javaPeer, fieldID)); \
  } \
  void JObjectAccess::set##Type##JavaField(const char *fieldName, jtype value) \
  { \
      jmethodID methodID; jfieldID fieldID; \
      getMethodOrField(fieldName, "set", "(" CODE ")V", CODE, methodID, fieldID); \
      methodID ? JENV->Call##Type##Method(javaPeer, methodID, value) : JENV->Set##Type##Field(javaPeer, fieldID, value); \
      checkExceptions(); \
  }

GETTER_SETTER(Boolean, jboolean, "Z")
GETTER_SETTER(Byte,    jbyte,    "B")
GETTER_SETTER(Char,    jchar,    "C")
GETTER_SETTER(Short,   jshort,   "S")
GETTER_SETTER(Int,     jint,     "I")
GETTER_SETTER(Long,    jlong,    "J")
GETTER_SETTER(Float,   jfloat,   "F")
GETTER_SETTER(Double,  jdouble,  "D")

#define JSTRING "Ljava/lang/String;"

std::string JObjectAccess::getStringJavaField(const char *fieldName) const
{
    jmethodID methodID; jfieldID fieldID;
    getMethodOrField(fieldName, "get", "()" JSTRING, JSTRING, methodID, fieldID);
    jstring str = (jstring) checkException(methodID ? JENV->CallObjectMethod(javaPeer, methodID) : JENV->GetObjectField(javaPeer, fieldID));
    return fromJavaString(str);
}

void JObjectAccess::setStringJavaField(const char *fieldName, const char *value)
{
    jmethodID methodID; jfieldID fieldID;
    getMethodOrField(fieldName, "set", "(" JSTRING ")V", JSTRING, methodID, fieldID);
    jstring str = JENV->NewStringUTF(value);
    methodID ? JENV->CallObjectMethod(javaPeer, methodID, str) : JENV->SetObjectField(javaPeer, fieldID, str);
    checkExceptions();
}



