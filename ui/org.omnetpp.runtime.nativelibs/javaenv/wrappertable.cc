//==========================================================================
//  WRAPPERTABLE.CC - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <map>

#include "wrappertable.h"
#include "javaenv.h"

WrapperTable::WrapperTable(JNIEnv *je)
{
    jenv = je;
    ASSERT(jenv!=NULL);
    jclass clazz = jenv->FindClass("org/omnetpp/runtime/nativelibs/simkernel/cObject");
    ASSERT(clazz!=NULL);
    zapMethodID = jenv->GetMethodID(clazz, "zap", "()V");
    ASSERT(zapMethodID!=NULL);
}

void WrapperTable::wrapperCreated(cObject *p, jobject wrapper)
{
    jweak ref = jenv->NewWeakGlobalRef(wrapper);
    wrappers.insert( std::pair<cObject*,jweak>(p,ref) );
}

void WrapperTable::wrapperFinalized(cObject *p, jobject wrapper)
{
    std::pair<RefMap::iterator,RefMap::iterator> range = wrappers.equal_range(p);
    for (RefMap::iterator it = range.first; it != range.second; /*nop*/) {
        ASSERT(it->first == p);
        if (jenv->IsSameObject(it->second,wrapper)) {
            jenv->DeleteWeakGlobalRef(it->second);
            wrappers.erase(it);
            break; // one Java object may only be once in the table
        } else {
            ++it;
        }
    }
}

void WrapperTable::objectDeleted(cObject *p)
{
    std::pair<RefMap::iterator,RefMap::iterator> range = wrappers.equal_range(p);
    for (RefMap::iterator it = range.first; it != range.second; /*nop*/) {
        ASSERT(it->first == p);
        jobject ref = jenv->NewLocalRef(it->second);
        if (!jenv->IsSameObject(it->second,NULL))
            jenv->CallVoidMethod(ref, zapMethodID);
        jenv->DeleteLocalRef(ref);
        jenv->DeleteWeakGlobalRef(it->second);
        RefMap::iterator tmp = it++;
        wrappers.erase(tmp);
    }
}

void WrapperTable::purge()
{
    for (RefMap::iterator it = wrappers.begin(); it != wrappers.end(); /*nop*/) {
        if (jenv->IsSameObject(it->second,NULL)) {
            jenv->DeleteWeakGlobalRef(it->second);
            RefMap::iterator tmp = it++;
            wrappers.erase(tmp);
        } else {
            ++it;
        }
    }
}


