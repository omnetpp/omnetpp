//==========================================================================
//   AKOUTVECTORMGR.H - header for
//                             OMNeT++
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//     AkOutputVectorManager
//     AkOutputScalarManager
//     AkSnapshotManager
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2002 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

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
class AkOutputVectorManager : public cFileOutputVectorManager
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
    explicit AkOutputVectorManager();

    /**
     * Destructor.
     */
    virtual ~AkOutputVectorManager();
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

