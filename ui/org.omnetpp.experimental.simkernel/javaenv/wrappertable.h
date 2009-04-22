//==========================================================================
//  WRAPPERTABLE.H - part of
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

#ifndef __WRAPPERTABLE_H
#define __WRAPPERTABLE_H

#include "javaenvdefs.h"
#include "omnetpp.h"
#include "jni.h"

/*------------------
 into cObject:
  protected cObject(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;

    //System.out.println("registering cobject " + swigCPtr);
    Javaenv env = Javaenv.cast(cSimulation.getActiveEnvir());
    env.swigWrapperCreated(this, this);
  }

  protected static long getCPtr(cObject obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  public void zap() {
    //System.out.println("zapping " + swigCPtr);
    swigCPtr = 0;
  }

  protected void finalize() {
    if (swigCPtr != 0) {
      Javaenv env = Javaenv.cast(cSimulation.getActiveEnvir());
      env.swigWrapperFinalized(this, this);
    }
    delete();
  }

 plus: check every swigCPtr access!!! (throw NPE if it's 0)
---------------------
*/

/**
 * Utility class. Purpose: zero out the cPtr in all SWIG Java proxy
 * objects whose C++ peer gets deleted.
 */
//XXX es: javabol minden cPtr accesst lecsekkelni!
class WrapperTable
{
  private:
    JNIEnv *jenv;
    typedef std::multimap<cObject*,jweak> RefMap;
    RefMap wrappers;
    jmethodID zapMethodID;

  public:
    WrapperTable(JNIEnv *je);
    // add weak reference to table (called from Java cObject ctor)
    void wrapperCreated(cObject *p, jobject wrapper);
    // should be called from finalize(); removes object from the table. Alleviates need for purge().
    void wrapperFinalized(cObject *p, jobject wrapper);
    // if there's a wrapper for it: clear cPtr in it, and remove from table
    void objectDeleted(cObject *p);
    // remove table entries where the Java object was already garbage collected (not very efficient!)
    void purge();
    // table size (useful for debugging)
    size_t getTableSize() {return wrappers.size();}
};

#endif


