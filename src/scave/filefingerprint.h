//=========================================================================
//  FILEFINGERPRINT.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_SCAVE_FILEFINGERPRINT_H
#define __OMNETPP_SCAVE_FILEFINGERPRINT_H

#include <cstdint>
#include "scavedefs.h"

namespace omnetpp {
namespace scave {

/**
 * Stores a file's size and last modification date, for checking if it's up to date.
 */
struct FileFingerprint
{
    int64_t lastModified = 0;
    int64_t fileSize = 0;
    bool isEmpty() { return lastModified == 0 && fileSize == 0; }
    bool operator==(const FileFingerprint& other) const { return other.lastModified == lastModified && other.fileSize == fileSize; }
    bool operator!=(const FileFingerprint& other) const { return !operator==(other); }
};

/**
 * Computes and returns a file's fingerprint. Throws an exception if stat() fails.
 */
SCAVE_API FileFingerprint readFileFingerprint(const char *fileName);

}  // namespace scave
}  // namespace omnetpp


#endif

