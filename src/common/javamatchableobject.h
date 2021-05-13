//==========================================================================
//  JAVAMATCHABLEOBJECT.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2021 Andras Varga
  Copyright (C) 2006-2021 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_COMMON_JAVAMATCHABLEOBJECT_H
#define __OMNETPP_COMMON_JAVAMATCHABLEOBJECT_H

#include <jni.h>
#include "common/matchexpression.h"

namespace omnetpp {

namespace common {

class COMMON_API JavaMatchableObject : public omnetpp::common::MatchExpression::Matchable
{
  protected:
    JNIEnv *env = nullptr;
    jobject object = nullptr;
    jmethodID getDefaultAttributeMethod;
    jmethodID getAttributeByNameMethod;
    mutable jstring jresult;
    mutable const char *result = nullptr;

  public:
    JavaMatchableObject() {}

    virtual ~JavaMatchableObject() {
        if (result)
            env->ReleaseStringUTFChars(jresult, result);
        if (object)
            env->DeleteGlobalRef(object);
    }

    void setJavaEnv(JNIEnv *env) {
        this->env = env;
        auto jclass = env->FindClass("org/omnetpp/common/engineext/IMatchableObject");
        getDefaultAttributeMethod = env->GetMethodID(jclass, "getAsString", "()Ljava/lang/String;");
        getAttributeByNameMethod = env->GetMethodID(jclass, "getAsString", "(Ljava/lang/String;)Ljava/lang/String;");
    }

    void setJavaObject(jobject object) {
        this->object = env->NewGlobalRef(object);
    }

    virtual const char *getAsString() const override {
        if (result)
            env->ReleaseStringUTFChars(jresult, result);
        jresult = (jstring)env->CallObjectMethod(object, getDefaultAttributeMethod);
        result = jresult ? env->GetStringUTFChars(jresult, 0) : nullptr;
        return result;
    }

    virtual const char *getAsString(const char *attribute) const override {
        if (result)
            env->ReleaseStringUTFChars(jresult, result);
        jstring jattribute = env->NewStringUTF(attribute);
        jresult = (jstring)env->CallObjectMethod(object, getAttributeByNameMethod, jattribute);
        env->DeleteLocalRef(jattribute);
        result = jresult ? env->GetStringUTFChars(jresult, 0) : nullptr;
        return result;
    }
};

}  // namespace common
}  // namespace omnetpp

#endif

