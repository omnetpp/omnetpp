//==========================================================================
//   AKOUTVECTORMGR.H - header for
//                             OMNeT++
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//     cAkOutputVectorManager
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2003 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifdef WITH_AKAROA

#ifndef __AKOUTVECTORMGR_H
#define __AKOUTVECTORMGR_H

#include <stdio.h>
#include "filemgrs.h"
#include "util.h"


/**
 * An output vector manager that supports multiple independent runs
 * via Akaroa; otherwise it behaves as the default output vector manager.
 * Akaroa will stop the simulation when the desired result accuracy
 * has been reached.
 *
 * @ingroup Envir
 */
class cAkOutputVectorManager : public cFileOutputVectorManager
{
    struct sAkVectorData : sVectorData {
       bool ak_controlled;  // under Akaroa control or not
       long ak_id;          // Akaroa vector ID
    };

  protected:
    bool ak_declared;  // is true when the number of records is already declared to Akaroa
    int ak_count;      // number of Akaroa vectors already registered

  protected:
    virtual sVectorData *createVectorData();

  public:
    /** @name Constructors, destructor */
    //@{

    /**
     * Constructor.
     */
    explicit cAkOutputVectorManager();

    /**
     * Destructor.
     */
    virtual ~cAkOutputVectorManager();
    //@}

    /** @name Redefined cFileOutputVectorManager member functions. */
    //@{

    /**
     * Registers a vector and returns a handle.
     */
    virtual void *registerVector(const char *modulename, const char *vectorname, int tuple);

    /**
     * Passes a (time, value) pair to Akaroa and also writes it into the output file.
     */
    virtual bool record(void *vectorhandle, simtime_t t, double value);

    /**
     * Recording value pairs is not supported with Akaroa.
     */
    virtual bool record(void *vectorhandle, simtime_t t, double value1, double value2);

    //@}
};

#endif
#endif

