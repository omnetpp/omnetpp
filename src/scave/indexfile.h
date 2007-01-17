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

struct Block {
    long startOffset;
    long startSerial; // inclusive
    long endSerial;   // exclusive

    Block(long startSerial, long endSerial, long startOffset)
        : startSerial(startSerial), endSerial(endSerial), startOffset(startOffset) {}
    long numOfEntries() { return endSerial-startSerial; }
    bool contains(long serial) { return startSerial <= serial && serial < endSerial; }
};

typedef std::vector<Block> Blocks;

struct VectorData {
    int vectorId;
    std::string moduleName;
    std::string name;
    long blockSize;
    long count;
    double min;
    double max;
    double sum;
    double sumSqr;
    Blocks blocks;

    VectorData() {}
    VectorData(int vectorId, std::string moduleName, std::string name, long blockSize)
        : vectorId(vectorId), moduleName(moduleName), name(name), blockSize(blockSize),
        count(0), min(DBL_MAX), max(DBL_MIN), sum(0.0), sumSqr(0.0) {}

    void collect(double value)
    {
        count++;
        min = (min < value ? min : value);
        max = (max > value ? max : value);
        sum += value;
        sumSqr += value*value;
    }

    void addBlock(long offset, long count)
    {
        long start = blocks.size() == 0 ? 0 : blocks.back().endSerial;
        blocks.push_back(Block(start, start+count, offset));
    }

    Block* VectorData::getBlockForEntry(long serial);
};

typedef std::vector<VectorData> Vectors;

struct VectorFileIndex {
    long vectorFileLastModified;
    long vectorFileSize;
    Vectors vectors;

    VectorFileIndex() : vectorFileLastModified(0), vectorFileSize(0), vectors() {}
    int getNumberOfVectors() { return vectors.size(); };
    VectorData *getVectorAt(int index) { return &vectors[index]; };
    VectorData *getVector(int vectorId);
};

class IndexFile
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

class IndexFileReader
{
   private:
        std::string filename;
    public:
        IndexFileReader(const char *filename);
        VectorFileIndex *readAll();
        VectorFileIndex *readHeader();
    protected:
        void parseLine(char **tokens, int numTokens, VectorFileIndex *index, long &numOfEntries, int lineNum);
};

class IndexFileWriter
{
    private:
        std::string filename;
        int precision;
        FILE *file;
    public:
        IndexFileWriter(const char *filename, int precision=14);
        ~IndexFileWriter();
        void writeAll(VectorFileIndex &index);
        void writeVector(VectorData &vector);
        void writeVectorDeclaration(VectorData &vector);
        void writeBlock(Block &block);
    protected:
        void openFile();
        void closeFile();
};

#endif
