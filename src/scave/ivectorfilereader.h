//=========================================================================
//  IVECTORFILEREADER.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef _IVECTORFILEREADER_H_
#define _IVECTORFILEREADER_H_

#include <assert.h>
#include "filereader.h"

struct OutputVectorEntry {
    long serial;
    double simtime;
    double value;

    OutputVectorEntry() {}

    OutputVectorEntry(long serial, double simtime, double value)
        : serial(serial), simtime(simtime), value(value) {}
};

struct Block {
    long startSerial; // inclusive
    long endSerial;   // exclusive
    long startOffset;
    OutputVectorEntry *entries;

    Block(long startSerial, long endSerial, long startOffset)
        : startSerial(startSerial), endSerial(endSerial), startOffset(startOffset), entries(NULL) {}
    ~Block() { deleteEntries(); }
    long numOfEntries() { return endSerial-startSerial; }
    bool contains(long serial) { return startSerial <= serial && serial < endSerial; }
    OutputVectorEntry* getEntryBySerial(long serial) { assert(entries != NULL); return &entries[serial - startSerial]; }
    void deleteEntries()
    {
        if (entries)
        {
            delete[] entries;
            entries = NULL;
        }
    }
};

/**
 * Vector file reader with random access.
 */
class IndexedVectorFileReader
{
    char* fname;
    int vectorId;
    
    char* ifname;
    FileReader *reader;
    
    // data read from the index file
    int numOfEntries;
    std::vector<Block> blocks;
    Block* currentBlock;    

    public:
        explicit IndexedVectorFileReader(const char* filename, int vectorId);
        ~IndexedVectorFileReader();
    protected:
        void loadIndex();
        void loadBlock(Block &block);
        Block* getBlockForEntry(long serial);
    public:
        int getNumberOfEntries() { return numOfEntries; };
        OutputVectorEntry *getEntryBySerial(long serial);
};

#endif
