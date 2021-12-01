//==========================================================================
//  FILESNAPSHOTMGR.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "omnetpp/simkerneldefs.h"

#include <cassert>
#include <cstring>
#include <fstream>
#include "common/fileutil.h"
#include "omnetpp/cconfigoption.h"
#include "envirbase.h"
#include "filesnapshotmgr.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace envir {

using std::ostream;
using std::ofstream;
using std::ios;

Register_PerRunConfigOption(CFGID_SNAPSHOT_FILE, "snapshot-file", CFG_FILENAME, "${resultdir}/${configname}-${iterationvarsf}#${repetition}.sna", "Name of the snapshot file.");

Register_Class(FileSnapshotManager);

void FileSnapshotManager::startRun()
{
    // clean up file from previous runs
    fname = getEnvir()->getConfig()->getAsFilename(CFGID_SNAPSHOT_FILE);
    dynamic_cast<EnvirBase *>(getEnvir())->processFileName(fname);
    removeFile(fname.c_str(), "old snapshot file");
}

void FileSnapshotManager::endRun()
{
}

ostream *FileSnapshotManager::getStreamForSnapshot()
{
    mkPath(directoryOf(fname.c_str()).c_str());
    ostream *os = new ofstream(fname.c_str(), ios::out|ios::app);
    return os;
}

void FileSnapshotManager::releaseStreamForSnapshot(std::ostream *os)
{
    delete os;
}

const char *FileSnapshotManager::getFileName() const
{
    return fname.c_str();
}

}  // namespace envir
}  // namespace omnetpp

