//==========================================================================
//  JUTIL.CC - part of
//
//                    OMNeT++/OMNEST
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include "jutil.h"
#include "guienv.h"
#include "ccomponent.h"
#include "ccomponenttype.h"

NAMESPACE_BEGIN


namespace JUtil {

jclass findClass(JNIEnv *jenv, const char *className)
{
    jclass clazz = jenv->FindClass(className);
    if (!clazz)
        opp_error("No such Java class: `%s'", className);
    return clazz;
}

jobject newObject(JNIEnv *jenv, jclass clazz)
{
    jmethodID ctorMethodId = getMethodID(jenv, clazz, "<init>", "()V");
    jobject object = jenv->NewObject(clazz, ctorMethodId);
    checkExceptions(jenv);
    if (!object)
        opp_error("Cannot instantiate Java class: `%s'", toString(jenv, clazz).c_str());
    return object;
}

jmethodID getMethodID(JNIEnv *jenv, jclass clazz, const char *methodName, const char *methodSig)
{
    jmethodID ret = jenv->GetMethodID(clazz, methodName, methodSig);
    jenv->ExceptionClear();
    if (!ret)
        opp_error("No `%s' %s method in Java class `%s'", methodName, methodSig, toString(jenv, clazz).c_str());
    return ret;
}

jmethodID getStaticMethodID(JNIEnv *jenv, jclass clazz, const char *methodName, const char *methodSig)
{
    jmethodID ret = jenv->GetStaticMethodID(clazz, methodName, methodSig);
    jenv->ExceptionClear();
    if (!ret)
        opp_error("No `%s' %s static method in Java class `%s'", methodName, methodSig, toString(jenv, clazz).c_str());
    return ret;
}

void checkExceptions(JNIEnv *jenv)
{
    jthrowable exceptionObject = jenv->ExceptionOccurred();
    if (exceptionObject)
    {
        jenv->ExceptionDescribe();
        jenv->ExceptionClear();
        opp_error("Java exception: %s", toString(jenv, exceptionObject).c_str());
    }
}

std::string fromJavaString(JNIEnv *jenv, jstring stringObject)
{
    if (!stringObject)
        return "<null>";
    jboolean isCopy;
    const char *buf = jenv->GetStringUTFChars(stringObject, &isCopy);
    std::string str = buf ? buf : "";
    jenv->ReleaseStringUTFChars(stringObject, buf);
    return str;
}

std::string toString(JNIEnv *jenv, jobject object)
{
    jclass clazz = jenv->GetObjectClass(object);
    jmethodID method = jenv->GetMethodID(clazz, "toString", "()Ljava/lang/String;");
    jstring string = (jstring)jenv->CallObjectMethod(object, method);
    checkExceptions(jenv);
    return fromJavaString(jenv, string);
}

jobjectArray toJavaArray(JNIEnv *jenv, const std::vector<std::string>& v)
{
    jclass stringClazz = jenv->FindClass("java/lang/String");
    jobjectArray array = jenv->NewObjectArray(v.size(), stringClazz, NULL);
    for (int i=0; i<(int)v.size(); i++) {
        jstring string = jenv->NewStringUTF(v[i].c_str());
        jenv->SetObjectArrayElement(array, i, string);
    }
    return array;
}


}; // namespace

//----


#define TRY2(CODE)  try { CODE; } catch (std::exception& e) {printf("<!> Warning: %s\n", e.what());}

const char *getObjectShortTypeName(cObject *object)
{
    if (dynamic_cast<cComponent*>(object))
        TRY2( return ((cComponent*)object)->getComponentType()->getName() );
    return object->getClassName();
}

const char *getObjectFullTypeName(cObject *object)
{
    if (dynamic_cast<cComponent*>(object))
        TRY2( return ((cComponent*)object)->getComponentType()->getFullName() );
    return object->getClassName();
}

NAMESPACE_END


