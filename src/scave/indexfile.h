//=========================================================================
//  INDEXFILE.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef _INDEXFILE_H_
#define _INDEXFILE_H_

#include <float.h>
#include <map>
#include "scavedefs.h"

/**
 * Statistical data collected per block and per vector.
 */
class SCAVE_API Statistics 
{
    private:
        long _count;
        double _min;
        double _max;
        double _sum;
        double _sumSqr;

    public:
        Statistics() : _count(0), _min(DBL_MAX), _max(DBL_MIN), _sum(0.0), _sumSqr(0.0) {}
        Statistics(long count, double min, double max, double sum, double sumSqr)
            :_count(count), _min(min), _max(max), _sum(sum), _sumSqr(sumSqr) {}

        Statistics &operator=(const Statistics &other)
        {
            _count = other._count;
            _min = other._min;
            _max = other._max;
            _sum = other._sum;
            _sumSqr = other._sumSqr;
            return *this;
        }

        long count() const { return _count; }
        double min() const { return _min; }
        double max() const { return _max; }
        double sum() const { return _sum; }
        double sumSqr() const { return _sumSqr; }

        void collect(double value)
        {
            _count++;
            _min = (_min < value ? _min : value);
            _max = (_max > value ? _max : value);
            _sum += value;
            _sumSqr += value * value;
        }

        void adjoin(const Statistics &other)
        {
            _count += other._count;
            _min = (_min < other._min ? _min : other._min);
            _max = (_max > other._max ? _max : other._max);
            _sum += other._sum;
            _sumSqr += other._sumSqr;
        }
};

/**
 * Data of one block stored in the index file.
 */
struct Block {
    long startOffset;
    long startSerial;
    long startEventNum;
    long endEventNum;
    simultime_t startTime;
    simultime_t endTime;
    Statistics stat;

    Block() : startOffset(-1), startSerial(-1), startEventNum(-1), endEventNum(-1),
        startTime(0.0), endTime(0.0), stat() {}

    long count() const { return stat.count(); }
    double min() const { return stat.min(); }
    double max() const { return stat.max(); }
    double sum() const { return stat.sum(); }
    double sumSqr() const { return stat.sumSqr(); }

    long endSerial() const { return startSerial + count(); }

    bool contains(long serial) const { return startSerial <= serial && serial < endSerial(); }

    void collect(long eventNum, simultime_t simtime, double value)
    {
        if (count() == 0)
        {
            startEventNum = eventNum;
            startTime = simtime;
        }
        endTime = simtime;
        endEventNum = eventNum;
        stat.collect(value);
    }
};

typedef std::vector<Block> Blocks;

/**
 * Entry for one vector in the index.
 */
struct VectorData {
    int vectorId;
    std::string moduleName;
    std::string name;
    std::string columns;
    long blockSize;
    Statistics stat;
    Blocks blocks;

    /**
     * Creates an index entry for a vector.
     */
    VectorData() {}
    VectorData(int vectorId, std::string moduleName, std::string name, std::string columns, long blockSize)
        : vectorId(vectorId), moduleName(moduleName), name(name), columns(columns), blockSize(blockSize), stat() {}

    long count() const { return stat.count(); }
    double min() const { return stat.min(); }
    double max() const { return stat.max(); }
    double sum() const { return stat.sum(); }
    double sumSqr() const { return stat.sumSqr(); }

    /**
     * Adds the block statistics to the vector statistics.
     */
    void collect(const Block &block) { stat.adjoin(block.stat); }

    void addBlock(const Block &block) { blocks.push_back(block); collect(block); }

    /**
     * Returns true if the vector contains the specified column.
     */
    bool hasColumn(char column) const { return columns.find(column) != std::string::npos; }

    /**
     * Returns a pointer to the block containing the entry with the given serial,
     * or NULL if no such entry.
     */
    const Block *getBlockBySerial(long serial) const;

    /**
     * Returns the first block which endTime >= simtime (when after == true)
     * or the last block whose startTime <= simtime (when after == false).
     * Returns NULL if no such block.
     */
    const Block *getBlockBySimtime(simultime_t simtime, bool after) const;

    /**
     * Returns the first block which endEventNum >= eventNum (when after == true)
     * or the last block whose startEventNum <= eventNum (when after == false).
     * Returns NULL if no such block.
     */
    const Block *getBlockByEventnum(long eventNum, bool after) const;

    /**
     * Finds the start (inclusive) and end (exclusive) indeces of the range of blocks,
     * containing entries in the [startTime,endTime] interval (both inclusive).
     * Returns the number of blocks found.
     */
    Blocks::size_type getBlocksInSimtimeInterval(simultime_t startTime, simultime_t endTime, Blocks::size_type &startIndex, Blocks::size_type &endIndex) const;

    /**
     * Finds the start (inclusive) and end (exclusive) indeces of the range of blocks,
     * containing entries in the [startEventNum,endEventNum] interval (both inclusive).
     * Returns the number of blocks found.
     */
    Blocks::size_type getBlocksInEventnumInterval(long startEventNum, long endEventNum, Blocks::size_type &startIndex, Blocks::size_type &endIndex) const;
};

typedef std::vector<VectorData> Vectors;

typedef std::map<std::string, std::string> StringMap;

/**
 * Run attributes written into the index file.
 */
struct RunData {
    std::string runName;
    int runNumber;
    StringMap attributes;
    StringMap moduleParams;

    RunData() : runNumber(0) {}

    bool parseLine(char **tokens, int numTokens, const char *filename, int lineNo);
    void writeToFile(FILE *file, const char *filename) const;
};

/**
 * Attributes of the vector files that are stored in the index file to
 * check if it is up-to-date.
 */
struct FingerPrint {
    long lastModified;
    long fileSize;

    FingerPrint() : lastModified(0), fileSize(0) {}
    FingerPrint(const char *vectorFileName);
};

/**
 * Data of all vectors stored in the index file.
 */
struct VectorFileIndex {
    std::string vectorFileName;
    FingerPrint fingerprint;
    RunData run;
    Vectors vectors;

    int getNumberOfVectors() const { return vectors.size(); };
    void addVector(const VectorData &vector) { vectors.push_back(vector); }
    const VectorData *getVectorAt(int index) const { return &vectors[index]; };
    VectorData *getVector(int vectorId);
};

/**
 * FIXME comment
 */
class SCAVE_API IndexFile
{
    public:
        static bool isIndexFile(const char *indexFileName);
        static bool isVectorFile(const char *vectorFileName);
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
class SCAVE_API IndexFileReader
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
        void parseLine(char **tokens, int numTokens, VectorFileIndex *index, long &numOfEntries, int lineNum);
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
        void writeRun(const RunData &run);
        /**
         * Writes out the index of one vector (declaration+blocks).
         */
        void writeVector(const VectorData& vector);
        /**
         * Writes out the declaration of a vector.
         */
        void writeVectorDeclaration(const VectorData& vector);
        /**
         * Writes out a block of the specified vector.
         */
        void writeBlock(const VectorData &vector, const Block& block);
    protected:
        /** Opens the index file. */
        void openFile();
        /** Closes the index file. */
        void closeFile();
};

#endif
