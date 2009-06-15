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

//#define DEBUGPRINTF printf
#define DEBUGPRINTF (void)

namespace JUtil {

std::string fromJavaString(jstring stringObject)
{
    if (!stringObject)
        return "<null>";
    JNIEnv *jenv = GUIEnv::getJEnv();
    jboolean isCopy;
    const char *buf = jenv->GetStringUTFChars(stringObject, &isCopy);
    std::string str = buf ? buf : "";
    jenv->ReleaseStringUTFChars(stringObject, buf);
    return str;
}

void checkExceptions()
{
    JNIEnv *jenv = GUIEnv::getJEnv();
    jthrowable exceptionObject = jenv->ExceptionOccurred();
    if (exceptionObject)
    {
        DEBUGPRINTF("JObjectAccess: exception occurred:\n");
        jenv->ExceptionDescribe();
        jenv->ExceptionClear();

        jclass throwableClass = jenv->GetObjectClass(exceptionObject);
        jmethodID method = jenv->GetMethodID(throwableClass, "toString", "()Ljava/lang/String;");
        jstring msg = (jstring)jenv->CallObjectMethod(exceptionObject, method);
        opp_error(eCUSTOM, fromJavaString(msg).c_str());
    }
}

jmethodID findMethod(jclass clazz, const char *clazzName, const char *methodName, const char *methodSig)
{
    JNIEnv *jenv = GUIEnv::getJEnv();
    jmethodID ret = jenv->GetMethodID(clazz, methodName, methodSig);
    jenv->ExceptionClear();
    if (!ret)
        opp_error("No `%s' %s method in Java class `%s'", methodName, methodSig, clazzName);
    return ret;
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


