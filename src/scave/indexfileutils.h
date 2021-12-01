//=========================================================================
//  INDEXFILEUTILS.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_SCAVE_INDEXFILEUTILS_H
#define __OMNETPP_SCAVE_INDEXFILEUTILS_H

#include <cfloat>
#include <map>
#include <vector>
#include "common/commonutil.h"
#include "common/statistics.h"
#include "omnetpp/platdep/platmisc.h"
#include "scavedefs.h"
#include "vectorfileindex.h"

namespace omnetpp {
namespace scave {

/**
 * FIXME comment
 */
class SCAVE_API IndexFileUtils
{
    public:
        static bool isIndexFile(const char *indexFileName);
        static bool isExistingVectorFile(const char *vectorFileName);
        static std::string getIndexFileName(const char *vectorFileName);
        static std::string getVectorFileName(const char *indexFileName);
        /**
         * Checks if the index file is up-to-date.
         * The fileName is either the name of the index file or the vector file.
         * The index file is up-to-date if the size and modification date stored in the index file
         * is equal to the size and date of the vector file.
         */
        static bool isIndexFileUpToDate(const char *fileName);
};


}  // namespace scave
}  // namespace omnetpp


#endif
