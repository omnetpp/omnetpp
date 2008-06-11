//==========================================================================
//  FILESNAPSHOTMGR.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __FILESNAPSHOTMGR_H
#define __FILESNAPSHOTMGR_H

#include <stdio.h>
#include "envirdefs.h"
#include "envirext.h"
#include "util.h"

NAMESPACE_BEGIN

/**
 * The default cSnapshotManager.
 *
 * @ingroup Envir
 */
class ENVIR_API cFileSnapshotManager : public cSnapshotManager
{
  protected:
    opp_string fname;  // output file name

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
    virtual void startRun();

    /**
     * Called at the end of a simulation run.
     */
    virtual void endRun();
    //@}

    /** @name Snapshot management */
    //@{
    /**
     * Returns a stream where a snapshot can be written.
     */
    virtual std::ostream *getStreamForSnapshot();

    /**
     * Releases a stream after a snapshot was written.
     */
    virtual void releaseStreamForSnapshot(std::ostream *os);

    /**
     * Returns the file name.
     */
    const char *getFileName() const;
    //@}
};

NAMESPACE_END


#endif

