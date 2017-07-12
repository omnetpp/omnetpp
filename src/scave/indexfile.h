//=========================================================================
//  INDEXFILE.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Tamas Borbely
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_SCAVE_INDEXFILE_H
#define __OMNETPP_SCAVE_INDEXFILE_H

#include <cfloat>
#include <map>
#include <vector>
#include "common/commonutil.h"
#include "common/statistics.h"
#include "omnetpp/platdep/platmisc.h"
#include "scavedefs.h"

namespace omnetpp {
namespace scave {

using omnetpp::common::Statistics;

/**
 * Data of one block stored in the index file.
 */
struct Block {
    file_offset_t startOffset;
    int64_t size;
    long startSerial;
    eventnumber_t startEventNum;
    eventnumber_t endEventNum;
    simultime_t startTime;
    simultime_t endTime;
    Statistics stat;

    Block() : startOffset(-1), size(0), startSerial(-1), startEventNum(-1), endEventNum(-1),
        startTime(0.0), endTime(0.0) {}

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

typedef std::vector<Block> Blocks;

typedef std::map<std::string, std::string> StringMap;

/**
 * Entry for one vector in the index.
 */
struct VectorData {
    int vectorId;
    std::string moduleName;
    std::string name;
    std::string columns;
    StringMap attributes;
    int64_t blockSize;
    eventnumber_t startEventNum;
    eventnumber_t endEventNum;
    simultime_t startTime;
    simultime_t endTime;
    Statistics stat;
    Blocks blocks;

    /**
     * Creates an index entry for a vector.
     */
    VectorData() : vectorId(-1), blockSize(0) {}
    VectorData(int vectorId, std::string moduleName, std::string name, std::string columns, int64_t blockSize)
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
    void collect(const Block& block)
    {
        if (getCount() == 0)
        {
            startEventNum = block.startEventNum;
            startTime = block.startTime;
        }
        endEventNum = block.endEventNum;
        endTime = block.endTime;
        stat.adjoin(block.stat);
        if (block.size > blockSize)
            blockSize = block.size;
    }

    void addBlock(const Block& block) { blocks.push_back(block); collect(block); }

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
    Blocks::size_type getBlocksInSimtimeInterval(simultime_t startTime, simultime_t endTime, Blocks::size_type& startIndex, Blocks::size_type& endIndex) const;

    /**
     * Finds the start (inclusive) and end (exclusive) indeces of the range of blocks,
     * containing entries in the [startEventNum,endEventNum] interval (both inclusive).
     * Returns the number of blocks found.
     */
    Blocks::size_type getBlocksInEventnumInterval(eventnumber_t startEventNum, eventnumber_t endEventNum, Blocks::size_type& startIndex, Blocks::size_type& endIndex) const;
};

typedef std::vector<VectorData> Vectors;

/**
 * Run attributes written into the index file.
 */
struct RunData {
    std::string runName;
    int runNumber;
    StringMap attributes;
    StringMap itervars;
    OrderedKeyValueList paramAssignments;

    RunData() : runNumber(0) {}

    bool parseLine(char **tokens, int numTokens, const char *filename, int64_t lineNo);
    void writeToFile(FILE *file, const char *filename) const;
};

/**
 * Attributes of the vector files that are stored in the index file to
 * check if it is up-to-date.
 */
struct FingerPrint {
    int64_t lastModified;
    int64_t fileSize;

    FingerPrint() : lastModified(0), fileSize(0) {}
    FingerPrint(const char *vectorFileName);
    bool check(const char *vectorFileName);
};

/**
 * Data of all vectors stored in the index file.
 */
struct VectorFileIndex {
    std::string vectorFileName;
    FingerPrint fingerprint;
    RunData run;
private:
    Vectors vectors;
    typedef std::map<int,int> VectorIdToIndexMap;
    VectorIdToIndexMap map; // maps vectorId to index in the vectors array

public:

    int getNumberOfVectors() const
    {
        return vectors.size();
    }

    void addVector(const VectorData& vector)
    {
        map[vector.vectorId] = vectors.size();
        vectors.push_back(vector);
    }

    const VectorData *getVectorAt(int index) const
    {
        Assert(0 <= index && index < (int)vectors.size());
        return& vectors[index];
    }

    VectorData *getVectorAt(int index)
    {
        Assert(0 <= index && index < (int)vectors.size());
        return& vectors[index];
    }

    VectorData *getVectorById(int vectorId)
    {
        VectorIdToIndexMap::const_iterator entry = map.find(vectorId);
        return entry!=map.end() ? getVectorAt(entry->second) : nullptr;
    }
};

/**
 * FIXME comment
 */
class SCAVE_API IndexFile
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
        VectorFileIndex *readFingerprint();
    protected:
        /**
         * Parse one line of the index file.
         */
        void parseLine(char **tokens, int numTokens, VectorFileIndex *index, int64_t lineNum);
};

/**
 * Writer for an index file.
 */
class SCAVE_API IndexFileWriter
{
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
        void writeVector(const VectorData& vector);
        /**
         * Writes out the declaration of a vector.
         */
        void writeVectorDeclaration(const VectorData& vector);
        /**
         * Writes out the attributes of a vector.
         */
        void writeVectorAttributes(const VectorData& vector);
        /**
         * Writes out a block of the specified vector.
         */
        void writeBlock(const VectorData& vector, const Block& block);
    protected:
        /** Opens the index file. */
        void openFile();
        /** Closes the index file. */
        void closeFile();
};

} // namespace scave
}  // namespace omnetpp


#endif
