//==========================================================================
//  FILESNAPSHOTMGR.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_ENVIR_FILESNAPSHOTMGR_H
#define __OMNETPP_ENVIR_FILESNAPSHOTMGR_H

#include <cstdio>
#include "omnetpp/envirext.h"
#include "omnetpp/simutil.h"
#include "envirdefs.h"

namespace omnetpp {
namespace envir {

/**
 * The default cISnapshotManager.
 *
 * @ingroup Envir
 */
class ENVIR_API cFileSnapshotManager : public cISnapshotManager
{
  protected:
    std::string fname;  // output file name

  public:
    /** @name Constructor, destructor */
    //@{

    /**
     * Constructor.
     */
    explicit cFileSnapshotManager();

    /**
     * Destructor.
     */
    virtual ~cFileSnapshotManager();
    //@}

    /** @name Controlling the beginning and end of collecting data. */
    //@{

    /**
     * Called at the beginning of a simulation run.
     */
    virtual void startRun() override;

    /**
     * Called at the end of a simulation run.
     */
    virtual void endRun() override;
    //@}

    /** @name Snapshot management */
    //@{
    /**
     * Returns a stream where a snapshot can be written.
     */
    virtual std::ostream *getStreamForSnapshot() override;

    /**
     * Releases a stream after a snapshot was written.
     */
    virtual void releaseStreamForSnapshot(std::ostream *os) override;

    /**
     * Returns the file name.
     */
    const char *getFileName() const override;
    //@}
};

} // namespace envir
}  // namespace omnetpp


#endif

