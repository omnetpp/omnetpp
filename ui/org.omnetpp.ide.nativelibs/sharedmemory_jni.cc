//=========================================================================
//  SHAREDMEMORY_JNI.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2019 Andras Varga
  Copyright (C) 2006-2019 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <iostream>
#include <cstring>

#include <jni.h>

#include "sharedmemory.h"

extern "C" {

// utility function, not called from Java
jint throwJavaRuntimeException(JNIEnv *env, const std::string& message)
{
    jclass exClass = env->FindClass("java/lang/RuntimeException");
    return env->ThrowNew(exClass, message.c_str());
}

JNIEXPORT void JNICALL Java_org_omnetpp_scave_engine_ScaveEngineJNI_createSharedMemory(JNIEnv *env, jobject clazz, jstring name, jlong size)
{
    const char *nameChars = env->GetStringUTFChars(name, nullptr);
    char nameStr[2048];
    strncpy(nameStr, nameChars, 2048);
    env->ReleaseStringUTFChars(name, nameChars);

    try {
        omnetpp::createSharedMemory(nameStr, size);
    }
    catch (std::runtime_error& e) {
        throwJavaRuntimeException(env, e.what());
    }
}

JNIEXPORT jobject JNICALL Java_org_omnetpp_scave_engine_ScaveEngineJNI_mapSharedMemory(JNIEnv *env, jobject clazz, jstring name, jlong size)
{
    const char *nameChars = env->GetStringUTFChars(name, nullptr);
    char nameStr[2048];
    strncpy(nameStr, nameChars, 2048);
    env->ReleaseStringUTFChars(name, nameChars);

    void *buffer;
    try {
        buffer = omnetpp::mapSharedMemory(nameStr, size);
    }
    catch (std::runtime_error& e) {
        throwJavaRuntimeException(env, e.what());
        return NULL;
    }

    jobject directBuffer = env->NewDirectByteBuffer(buffer, size);
    return directBuffer;
}

JNIEXPORT void JNICALL Java_org_omnetpp_scave_engine_ScaveEngineJNI_unmapSharedMemory(JNIEnv *env, jobject clazz, jobject directBuffer)
{
    void *buffer = env->GetDirectBufferAddress(directBuffer);
    jlong capacity = env->GetDirectBufferCapacity(directBuffer);

    try {
        omnetpp::unmapSharedMemory(buffer, capacity);
     }
     catch (std::runtime_error& e) {
        throwJavaRuntimeException(env, e.what());
    }
}

JNIEXPORT void JNICALL Java_org_omnetpp_scave_engine_ScaveEngineJNI_removeSharedMemory(JNIEnv *env, jobject clazz, jstring name)
{
    const char *nameChars = env->GetStringUTFChars(name, nullptr);
    char nameStr[2048];
    strncpy(nameStr, nameChars, 2048);
    env->ReleaseStringUTFChars(name, nameChars);

    try {
        omnetpp::removeSharedMemory(nameStr);
    }
    catch (std::runtime_error& e) {
        throwJavaRuntimeException(env, e.what());
    }
}

} // extern "C"
