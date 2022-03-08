//=========================================================================
//  FILEFINGERPRINT.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cstdlib>
#include "omnetpp/platdep/platmisc.h"
#include "filefingerprint.h"

namespace omnetpp {
namespace scave {

FileFingerprint readFileFingerprint(const char *fileName)
{
    struct opp_stat_t s;
    if (opp_stat(fileName, &s) != 0)
        throw opp_runtime_error("File '%s' not found", fileName); // "not found" == may be missing, or may be inaccessible

    FileFingerprint fingerprint;
    fingerprint.lastModified = (int64_t)s.st_mtime;
    fingerprint.fileSize = (int64_t)s.st_size;
    return fingerprint;
}

}  // namespace scave
}  // namespace omnetpp

