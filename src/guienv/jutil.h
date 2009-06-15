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
#include "guienvdefs.h"
#include "cobject.h"

NAMESPACE_BEGIN

/**
 * Convenience methods for working with JNI.
 */
namespace JUtil {

std::string fromJavaString(jstring stringObject);
jmethodID findMethod(jclass clazz, const char *clazzName, const char *methodName, const char *methodSig);
void checkExceptions();
template<typename T> T checkException(T a)  {checkExceptions(); return a;}

};

const char *getObjectShortTypeName(cObject *object);
const char *getObjectFullTypeName(cObject *object);

NAMESPACE_END

#endif



