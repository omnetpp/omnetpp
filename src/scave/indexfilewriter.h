//=========================================================================
//  INDEXFILEWRITER.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_SCAVE_INDEXFILEWRITER_H
#define __OMNETPP_SCAVE_INDEXFILEWRITER_H

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

using omnetpp::common::Statistics;


/**
 * Writer for an index file.
 */
class SCAVE_API IndexFileWriter
{
    using VectorInfo = VectorFileIndex::VectorInfo;
    using Block = VectorFileIndex::Block;
    using RunData = VectorFileIndex::RunData;

    private:
        /** Name of the index file. */
        std::string filename;
        /** Precision of double values stored in the index file. */
        int precision;
        /** Handle of the opened index file. */
        FILE *file;
    public:
        /**
         * Creates a writer for the specified index file.
         */
        IndexFileWriter(const char *filename, int precision=14);
        /**
         * Deletes the writer. (Closes the index file first.)
         */
        ~IndexFileWriter();
        /**
         * Writes out the index fully.
         */
        void writeAll(const VectorFileIndex& index);
        /**
         * Writes out the fingerprint of the vector file this index file belongs to.
         */
        void writeFingerprint(std::string vectorFileName);
        /**
         * Writes out the run attributes.
         */
        void writeRun(const RunData& run);
        /**
         * Writes out the index of one vector (declaration+blocks).
         */
        void writeVector(const VectorInfo& vector);
        /**
         * Writes out the declaration of a vector.
         */
        void writeVectorDeclaration(const VectorInfo& vector);
        /**
         * Writes out the attributes of a vector.
         */
        void writeVectorAttributes(const VectorInfo& vector);
        /**
         * Writes out a block of the specified vector.
         */
        void writeBlock(const VectorInfo& vector, const Block *block);
    protected:
        /** Opens the index file. */
        void openFile();
        /** Closes the index file. */
        void closeFile();
};

}  // namespace scave
}  // namespace omnetpp


#endif
