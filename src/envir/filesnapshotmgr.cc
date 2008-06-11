//==========================================================================
//  FILESNAPSHOTMGR.CC - part of
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

#include "simkerneldefs.h"

#include <assert.h>
#include <string.h>
#include <fstream>
#include "cconfigkey.h"
#include "fileutil.h"
#include "bootenv.h"
#include "envirbase.h"
#include "filesnapshotmgr.h"

USING_NAMESPACE

using std::ostream;
using std::ofstream;
using std::ios;


Register_PerRunConfigEntry(CFGID_SNAPSHOT_FILE, "snapshot-file", CFG_FILENAME, "${resultdir}/${configname}-${runnumber}.sna", "Name of the snapshot file.");


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
    fname = ev.getConfig()->getAsFilename(CFGID_SNAPSHOT_FILE).c_str();
    dynamic_cast<EnvirBase *>(&ev)->processFileName(fname);
    removeFile(fname.c_str(), "old snapshot file");
}

void cFileSnapshotManager::endRun()
{
}

ostream *cFileSnapshotManager::getStreamForSnapshot()
{
    mkPath(directoryOf(fname.c_str()).c_str());
    ostream *os = new ofstream(fname.c_str(), ios::out|ios::app);
    return os;
}

void cFileSnapshotManager::releaseStreamForSnapshot(std::ostream *os)
{
    delete os;
}

const char *cFileSnapshotManager::getFileName() const
{
    return fname.c_str();
}

