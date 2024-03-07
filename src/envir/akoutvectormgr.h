//==========================================================================
//  AKOUTVECTORMGR.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Declaration of the following classes:
//     cAkOutputVectorManager
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifdef WITH_AKAROA

#ifndef __OMNETPP_ENVIR_AKOUTVECTORMGR_H
#define __OMNETPP_ENVIR_AKOUTVECTORMGR_H

#include <cstdio>
#include "omnetpp/simutil.h"
#include "omnetppoutvectormgr.h"

namespace omnetpp {
namespace envir {


/**
 * An output vector manager that supports multiple independent runs
 * via Akaroa; otherwise it behaves as the default output vector manager.
 * Akaroa will stop the simulation when the desired result accuracy
 * has been reached.
 *
 * @ingroup Envir
 */
class ENVIR_API AkOutputVectorManager : public OmnetppOutputVectorManager
{
    struct AkVectorData : VectorData {
       bool ak_controlled;  // under Akaroa control or not
       long ak_id;          // Akaroa vector ID
    };

  protected:
    bool ak_declared;  // is true when the number of records is already declared to Akaroa
    int ak_count;      // number of Akaroa vectors already registered

  protected:
    virtual VectorData *createVectorData();

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
    virtual void *registerVector(const char *modulename, const char *vectorname, opp_string_map *attributes=nullptr);

    /**
     * Passes a (time, value) pair to Akaroa and also writes it into the output file.
     */
    virtual bool record(void *vectorhandle, simtime_t t, double value);
    //@}
};

#endif
}  // namespace envir
}  // namespace omnetpp


#endif

