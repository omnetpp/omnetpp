//==========================================================================
//  JUTIL.H - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.

*--------------------------------------------------------------*/

#ifndef __JUTIL_H
#define __JUTIL_H

#include <jni.h>
#include <vector>
#include <string>
#include "guienvdefs.h"
#include "cobject.h"

NAMESPACE_BEGIN

/**
 * Convenience methods for working with JNI.
 */
namespace JUtil {

jclass findClass(JNIEnv *jenv, const char *className);
jobject newObject(JNIEnv *jenv, jclass clazz); // uses argless ctor
jmethodID getMethodID(JNIEnv *jenv, jclass clazz, const char *methodName, const char *methodSig);
jmethodID getStaticMethodID(JNIEnv *jenv, jclass clazz, const char *methodName, const char *methodSig);
void checkExceptions(JNIEnv *jenv);
std::string fromJavaString(JNIEnv *jenv, jstring stringObject);
std::string toString(JNIEnv *jenv, jobject object);
jobjectArray toJavaArray(JNIEnv *jenv, const std::vector<std::string>& v);

};

const char *getObjectShortTypeName(cObject *object);
const char *getObjectFullTypeName(cObject *object);

NAMESPACE_END

#endif



