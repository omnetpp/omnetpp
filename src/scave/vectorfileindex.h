//=========================================================================
//  VECTORFILEINDEX.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_SCAVE_VECTORFILEINDEX_H
#define __OMNETPP_SCAVE_VECTORFILEINDEX_H

#include <cfloat>
#include <map>
#include <vector>
#include "common/commonutil.h"
#include "common/statistics.h"
#include "omnetpp/platdep/platmisc.h"
#include "scavedefs.h"
#include "scaveutils.h"
#include "filefingerprint.h"

namespace omnetpp {
namespace scave {

using omnetpp::common::Statistics;

/**
 * Data of all vectors stored in the index file.
 */
struct VectorFileIndex {

    /**
     * Run attributes written into the index file.
     */
    struct RunData {
        std::string runName;
        int runNumber = 0;
        StringMap attributes;
        StringMap itervars;
        OrderedKeyValueList configEntries;

        RunData() {}

        bool parseLine(char **tokens, int numTokens, const char *filename, int64_t lineNo);
        void writeToFile(FILE *file, const char *filename) const;
    };

    /**
     * Data of one block stored in the index file.
     */
    struct Block {
        int vectorId = -1;
        file_offset_t startOffset = -1;
        int64_t size = 0;
        long startSerial = -1;
        eventnumber_t startEventNum = -1;
        eventnumber_t endEventNum = -1;
        simultime_t startTime = 0.0;
        simultime_t endTime = 0.0;
        Statistics stat;

        long getCount() const { return stat.getCount(); }

        long endSerial() const { return startSerial + getCount(); }

        bool contains(long serial) const { return startSerial <= serial && serial < endSerial(); }

        void collect(eventnumber_t eventNum, simultime_t simtime, double value)
        {
            if (getCount() == 0) {
                startEventNum = eventNum;
                startTime = simtime;
            }
            endEventNum = eventNum;
            endTime = simtime;
            stat.collect(value);
        }
    };

    typedef std::map<std::string, std::string> StringMap;

    /**
     * Entry for one vector in the index.
     */
    struct VectorInfo {
        int vectorId = -1;
        std::string moduleName;
        std::string name;
        std::string columns;
        StringMap attributes;
        int64_t blockSize = 0;
        eventnumber_t startEventNum;
        eventnumber_t endEventNum;
        simultime_t startTime;
        simultime_t endTime;
        Statistics stat;
        std::vector<Block *> blocks; // points into the block list of the index

        /**
         * Creates an index entry for a vector.
         */
        VectorInfo() {}
        VectorInfo(int vectorId, std::string moduleName, std::string name, std::string columns, int64_t blockSize)
            : vectorId(vectorId), moduleName(moduleName), name(name), columns(columns), blockSize(blockSize),
              startEventNum(-1), endEventNum(-1), startTime(0.0), endTime(0.0) {}

        long getCount() const { return stat.getCount(); }
        double getMin() const { return stat.getMin(); }
        double getMax() const { return stat.getMax(); }
        //double getSum() const { return stat.getSum(); }
        //double getSumSqr() const { return stat.getSumSqr(); }

        /**
         * Adds the block statistics to the vector statistics.
         */
        void collect(const Block *block)
        {
            if (getCount() == 0)
            {
                startEventNum = block->startEventNum;
                startTime = block->startTime;
            }
            endEventNum = block->endEventNum;
            endTime = block->endTime;
            stat.adjoin(block->stat);
            if (block->size > blockSize)
                blockSize = block->size;
        }

        void addBlock(Block *block) { blocks.push_back(block); collect(block); }

        /**
         * Returns true if the vector contains the specified column.
         */
        bool hasColumn(char column) const { return columns.find(column) != std::string::npos; }

        /**
         * Returns a pointer to the block containing the entry with the given serial,
         * or nullptr if no such entry.
         */
        const Block *getBlockBySerial(long serial) const;

        /**
         * Returns the first block which endTime >= simtime (when after == true)
         * or the last block whose startTime <= simtime (when after == false).
         * Returns nullptr if no such block.
         */
        const Block *getBlockBySimtime(simultime_t simtime, bool after) const;

        /**
         * Returns the first block which endEventNum >= eventNum (when after == true)
         * or the last block whose startEventNum <= eventNum (when after == false).
         * Returns nullptr if no such block.
         */
        const Block *getBlockByEventnum(eventnumber_t eventNum, bool after) const;

        /**
         * Finds the start (inclusive) and end (exclusive) indeces of the range of blocks,
         * containing entries in the [startTime,endTime] interval (both inclusive).
         * Returns the number of blocks found.
         */
        std::vector<Block>::size_type getBlocksInSimtimeInterval(simultime_t startTime, simultime_t endTime, std::vector<Block>::size_type& startIndex, std::vector<Block>::size_type& endIndex) const;

        /**
         * Finds the start (inclusive) and end (exclusive) indeces of the range of blocks,
         * containing entries in the [startEventNum,endEventNum] interval (both inclusive).
         * Returns the number of blocks found.
         */
        std::vector<Block>::size_type getBlocksInEventnumInterval(eventnumber_t startEventNum, eventnumber_t endEventNum, std::vector<Block>::size_type& startIndex, std::vector<Block>::size_type& endIndex) const;
    };

    std::string vectorFileName;
    FileFingerprint fingerprint;
    RunData run;
private:
    std::vector<VectorInfo> vectors;
    typedef std::map<int,int> VectorIdToIndexMap;
    VectorIdToIndexMap map; // maps vectorId to index in the vectors array
    std::vector<Block *> blocks; // all the blocks in the vector file

public:
    VectorFileIndex() {}

    ~VectorFileIndex();

    void addBlock(Block *block) { blocks.push_back(block); }

    const std::vector<Block *>& getBlocks() const { return blocks; }

    int getNumberOfVectors() const { return vectors.size(); }

    void addVector(const VectorInfo& vector) {
        map[vector.vectorId] = vectors.size();
        vectors.push_back(vector);
    }

    const VectorInfo *getVectorAt(int index) const {
        Assert(0 <= index && index < (int)vectors.size());
        return& vectors[index];
    }

    VectorInfo *getVectorAt(int index) {
        Assert(0 <= index && index < (int)vectors.size());
        return& vectors[index];
    }

    VectorInfo *getVectorById(int vectorId) {
        VectorIdToIndexMap::const_iterator entry = map.find(vectorId);
        return entry!=map.end() ? getVectorAt(entry->second) : nullptr;
    }
};

}  // namespace scave
}  // namespace omnetpp


#endif
