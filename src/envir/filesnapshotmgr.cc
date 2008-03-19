//==========================================================================
//  FILESNAPSHOTMGR.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "simkerneldefs.h"

#include <assert.h>
#include <string.h>
#include <fstream>
#include "cconfigkey.h"
#include "fileutil.h"
#include "cenvir.h"
#include "omnetapp.h"
#include "filesnapshotmgr.h"

USING_NAMESPACE

using std::ostream;
using std::ofstream;
using std::ios;


Register_PerRunConfigEntry(CFGID_SNAPSHOT_FILE, "snapshot-file", CFG_FILENAME, "${configname}-${runnumber}.sna", "Name of the snapshot file.");


Register_Class(cFileSnapshotManager);


cFileSnapshotManager::cFileSnapshotManager()
{
}

cFileSnapshotManager::~cFileSnapshotManager()
{
}

void cFileSnapshotManager::startRun()
{
    // clean up file from previous runs
    fname = ev.config()->getAsFilename(CFGID_SNAPSHOT_FILE).c_str();
    ev.app->processFileName(fname);
    removeFile(fname.c_str(), "old snapshot file");
}

void cFileSnapshotManager::endRun()
{
}

ostream *cFileSnapshotManager::getStreamForSnapshot()
{
    ostream *os = new ofstream(fname.c_str(), ios::out|ios::app);
    return os;
}

void cFileSnapshotManager::releaseStreamForSnapshot(std::ostream *os)
{
    delete os;
}

const char *cFileSnapshotManager::fileName() const
{
    return fname.c_str();
}

