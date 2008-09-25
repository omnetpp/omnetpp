#include "JUtil.h"
#include "JSimpleModule.h"

//#define DEBUGPRINTF printf
#define DEBUGPRINTF (void)

// This will come from the generated SimkernelJNI_registerNatives.cc
void SimkernelJNI_registerNatives(JNIEnv *jenv);

JavaVM *JUtil::vm;
JNIEnv *JUtil::jenv;


void JUtil::initJVM()
{
    DEBUGPRINTF("Starting JVM...\n");
    JavaVMInitArgs vm_args;
    JavaVMOption options[10];

    int n = 0;
    const char *classpath = getenv("CLASSPATH");
    if (!classpath)
        opp_error("CLASSPATH environment variable is not set");
    std::string classpathOption = std::string("-Djava.class.path=")+(classpath ? classpath : "");
    options[n++].optionString = (char *)classpathOption.c_str(); /* user classes */
    options[n++].optionString = "-Djava.library.path=.";   /* set native library path */
    //options[n++].optionString = "-Djava.compiler=NONE";    /* disable JIT */
    //options[n++].optionString = "-verbose:jni";            /* print JNI-related messages */
    //options[n++].optionString = "-verbose:class";          /* print class loading messages */

    vm_args.version = JNI_VERSION_1_2;
    vm_args.options = options;
    vm_args.nOptions = n;
    vm_args.ignoreUnrecognized = true;

    int res = JNI_CreateJavaVM(&vm, (void **)&jenv, &vm_args);
    if (res<0)
        opp_error("Could not create Java VM: JNI_CreateJavaVM returned %d", res);

    DEBUGPRINTF("Registering native methods...\n");
    SimkernelJNI_registerNatives(jenv);
    DEBUGPRINTF("Done.\n");
}

std::string JUtil::fromJavaString(jstring stringObject)
{
    if (!stringObject)
        return "<null>";
    jboolean isCopy;
    const char *buf = JUtil::jenv->GetStringUTFChars(stringObject, &isCopy);
    std::string str = buf ? buf : "";
    JUtil::jenv->ReleaseStringUTFChars(stringObject, buf);
    return str;
}

void JUtil::checkExceptions()
{
    jthrowable exceptionObject = JUtil::jenv->ExceptionOccurred();
    if (exceptionObject)
    {
        DEBUGPRINTF("JObjectAccess: exception occurred:\n");
        JUtil::jenv->ExceptionDescribe();
        JUtil::jenv->ExceptionClear();

        jclass throwableClass = JUtil::jenv->GetObjectClass(exceptionObject);
        jmethodID method = JUtil::jenv->GetMethodID(throwableClass, "toString", "()Ljava/lang/String;");
        jstring msg = (jstring)JUtil::jenv->CallObjectMethod(exceptionObject, method);
        opp_error(eCUSTOM, fromJavaString(msg).c_str());
    }
}

jmethodID JUtil::findMethod(jclass clazz, const char *clazzName, const char *methodName, const char *methodSig)
{
    jmethodID ret = jenv->GetMethodID(clazz, methodName, methodSig);
    jenv->ExceptionClear();
    if (!ret)
        opp_error("No `%s' %s method in Java class `%s'", methodName, methodSig, clazzName);
    return ret;
}

//---

using namespace JUtil;

JObjectAccess::JObjectAccess(jobject object)
{
    javaPeer = object;
    toStringMethod = 0;
}

void JObjectAccess::setObject(jobject object)
{
    javaPeer = object;
    toStringMethod = 0;
}

std::string JObjectAccess::toString() const
{
    if (!toStringMethod)
    {
        jclass clazz = jenv->GetObjectClass(javaPeer);
        toStringMethod = jenv->GetMethodID(clazz, "toString", "()Ljava/lang/String;");
        checkExceptions();
    }
    jstring stringObject = (jstring)jenv->CallObjectMethod(javaPeer, toStringMethod);
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

    jclass clazz = jenv->GetObjectClass(javaPeer);
    checkExceptions();
    fieldID = 0;
    methodID = jenv->GetMethodID(clazz, methodName, methodsig);
    if (methodID)
        return;
    jenv->ExceptionClear();
    fieldID = jenv->GetFieldID(clazz, fieldName, fieldsig);
    if (fieldID)
        return;
    jenv->ExceptionClear();
    opp_error("Java object has neither method `%s' nor field `%s' with the given type",
              methodName, fieldName);
}

#define GETTER_SETTER(Type, jtype, CODE)  \
  jtype JObjectAccess::get##Type##JavaField(const char *fieldName) const \
  { \
      jmethodID methodID; jfieldID fieldID; \
      getMethodOrField(fieldName, "get", "()" CODE, CODE, methodID, fieldID); \
      return checkException(methodID ? jenv->Call##Type##Method(javaPeer, methodID) : jenv->Get##Type##Field(javaPeer, fieldID)); \
  } \
  void JObjectAccess::set##Type##JavaField(const char *fieldName, jtype value) \
  { \
      jmethodID methodID; jfieldID fieldID; \
      getMethodOrField(fieldName, "set", "(" CODE ")V", CODE, methodID, fieldID); \
      methodID ? jenv->Call##Type##Method(javaPeer, methodID, value) : jenv->Set##Type##Field(javaPeer, fieldID, value); \
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
    jstring str = (jstring) checkException(methodID ? jenv->CallObjectMethod(javaPeer, methodID) : jenv->GetObjectField(javaPeer, fieldID));
    return fromJavaString(str);
}

void JObjectAccess::setStringJavaField(const char *fieldName, const char *value)
{
    jmethodID methodID; jfieldID fieldID;
    getMethodOrField(fieldName, "set", "(" JSTRING ")V", JSTRING, methodID, fieldID);
    jstring str = jenv->NewStringUTF(value);
    methodID ? jenv->CallObjectMethod(javaPeer, methodID, str) : jenv->SetObjectField(javaPeer, fieldID, str);
    checkExceptions();
}



