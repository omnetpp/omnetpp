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
#include "ccomponent.h"
#include "ccomponenttype.h"

NAMESPACE_BEGIN

/*XXX
//#define DEBUGPRINTF printf
#define DEBUGPRINTF (void)

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
*/

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


