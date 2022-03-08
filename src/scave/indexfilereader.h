//=========================================================================
//  INDEXFILEREADER.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_SCAVE_INDEXFILEREADER_H
#define __OMNETPP_SCAVE_INDEXFILEREADER_H

#include <cfloat>
#include <map>
#include <vector>
#include "common/commonutil.h"
#include "common/statistics.h"
#include "omnetpp/platdep/platmisc.h"
#include "scavedefs.h"
#include "vectorfileindex.h"
#include "filefingerprint.h"

namespace omnetpp {
namespace scave {

using omnetpp::common::Statistics;

/**
 * Reader for an index file.
 */
class SCAVE_API IndexFileReader  //TODO this class should share the reader/parser with OmnetppResultFileLoader!!!
{
   private:
        /** The name of the index file. */
        std::string filename;
    public:
        /**
         * Creates a reader for the specified index file.
         */
        IndexFileReader(const char *filename);

        /**
         * Reads the index fully into the memory.
         */
        VectorFileIndex *readAll();

        /**
         * Reads the fingerprint of the vector file this index file belongs to.
         */
        FileFingerprint readRecordedFingerprint();
    protected:
        /**
         * Parse one line of the index file.
         */
        void parseLine(char **tokens, int numTokens, VectorFileIndex *index, int64_t lineNum);
};


}  // namespace scave
}  // namespace omnetpp


#endif
