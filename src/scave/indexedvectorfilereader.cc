//=========================================================================
//  INDEXEDVECTORFILE.CC - part of
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

#include <clocale>
#include <cstdlib>
#include "common/exception.h"
#include "common/linetokenizer.h"
#include "common/stringutil.h"
#include "omnetpp/platdep/platmisc.h"
#include "channel.h"
#include "indexedvectorfilereader.h"
#include "indexfilereader.h"
#include "scaveutils.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace scave {

#define VECTOR_FILE_VERSION 2

//=========================================================================

IndexedVectorFileReader::IndexedVectorFileReader(const char *filename, int vectorId)
    : fname(filename), index(nullptr), vector(nullptr), currentBlock(nullptr)
{
    std::string ifname = IndexFileUtils::getIndexFileName(filename);
    IndexFileReader indexReader(ifname.c_str());
    index = indexReader.readAll();  // XXX do not read whole index
    vector = index->getVectorById(vectorId);

    if (!vector)
        throw opp_runtime_error("Vector with vectorId %d not found in file '%s'",
                vectorId, filename);
}

IndexedVectorFileReader::~IndexedVectorFileReader()
{
    if (index != nullptr)
        delete index;
}

// see filemgrs.h
#define MIN_BUFFER_SIZE    512

#ifdef CHECK
#undef CHECK
#endif
#define CHECK(cond, msg, block, line) \
            if (!(cond))\
            {\
                throw opp_runtime_error("Invalid vector file syntax: %s, file %s, block offset %" PRId64 ", line in block %d", \
                                        msg, fname.c_str(), (int64_t)block.startOffset, line);\
            }

void IndexedVectorFileReader::loadBlock(const Block& block)
{
    if (currentBlock == &block)
        return;

    if (currentBlock != nullptr) {
        currentBlock = nullptr;
        currentEntries.clear();
    }

    size_t bufferSize = vector->blockSize;
    if (bufferSize < MIN_BUFFER_SIZE)
        bufferSize = MIN_BUFFER_SIZE;
    FileReader reader(fname.c_str(), bufferSize);

    long count = block.getCount();
    reader.seekTo(block.startOffset);
    currentEntries.resize(count);

    char *line, **tokens;
    int numTokens;
    LineTokenizer tokenizer;
    int id;

    std::string columns = vector->columns;
    int columnsNo = columns.size();

    for (int i = 0; i < count; ++i) {
        CHECK(line = reader.getNextLineBufferPointer(), "Unexpected end of file", block, i);
        int len = reader.getCurrentLineLength();

        tokenizer.tokenize(line, len);
        tokens = tokenizer.tokens();
        numTokens = tokenizer.numTokens();

        CHECK(numTokens >= (int)columns.size() + 1, "Line is too short", block, i);
        CHECK(parseInt(tokens[0], id) && id == vector->vectorId, "Missing or unexpected vector id", block, i);

        OutputVectorEntry& entry = currentEntries[i];
        entry.serial = block.startSerial+i;
        for (int j = 0; j < columnsNo; ++j) {
            switch (columns[j]) {
                case 'E': CHECK(parseInt64(tokens[j+1], entry.eventNumber), "Malformed event number", block, i); break;
                case 'T': CHECK(parseSimtime(tokens[j+1], entry.simtime), "Malformed simulation time", block, i); break;
                case 'V': CHECK(parseDouble(tokens[j+1], entry.value), "Malformed vector value", block, i); break;
                default: CHECK(false, "Unknown column", block, i); break;
            }
        }
    }

    currentBlock = &block;
}

OutputVectorEntry *IndexedVectorFileReader::getEntryBySerial(long serial)
{
    if (serial < 0 || serial >= vector->getCount())
        return nullptr;

    if (currentBlock == nullptr || !currentBlock->contains(serial)) {
        loadBlock(*(vector->getBlockBySerial(serial)));
    }

    return &currentEntries.at(serial - currentBlock->startSerial);
}

OutputVectorEntry *IndexedVectorFileReader::getEntryBySimtime(simultime_t simtime, bool after)
{
    const Block *block = vector->getBlockBySimtime(simtime, after);
    if (block) {
        loadBlock(*block);
        if (after) {
            for (Entries::iterator it = currentEntries.begin(); it != currentEntries.end(); ++it)  // FIXME: binary search
                if (it->simtime >= simtime)
                    return &(*it);

        }
        else {
            for (Entries::reverse_iterator it = currentEntries.rbegin(); it != currentEntries.rend(); ++it)  // FIXME: binary search
                if (it->simtime <= simtime)
                    return &(*it);

        }
    }
    return nullptr;
}

OutputVectorEntry *IndexedVectorFileReader::getEntryByEventnum(eventnumber_t eventNum, bool after)
{
    const Block *block = vector->getBlockByEventnum(eventNum, after);
    if (block) {
        loadBlock(*block);
        if (after) {
            for (Entries::iterator it = currentEntries.begin(); it != currentEntries.end(); ++it)  // FIXME: binary search
                if (it->eventNumber >= eventNum)
                    return &(*it);

        }
        else {
            for (Entries::reverse_iterator it = currentEntries.rbegin(); it != currentEntries.rend(); ++it)  // FIXME: binary search
                if (it->eventNumber <= eventNum)
                    return &(*it);

        }
    }
    return nullptr;
}

long IndexedVectorFileReader::collectEntriesInSimtimeInterval(simultime_t startTime, simultime_t endTime, Entries& out)
{
    std::vector<Block>::size_type startIndex;
    std::vector<Block>::size_type endIndex;
    vector->getBlocksInSimtimeInterval(startTime, endTime,  /*out*/ startIndex,  /*out*/ endIndex);

    Entries::size_type count = 0;
    for (std::vector<Block>::size_type i = startIndex; i < endIndex; i++) {
        const Block& block = vector->blocks[i];
        loadBlock(block);
        for (long j = 0; j < block.getCount(); ++j) {
            OutputVectorEntry& entry = currentEntries[j];
            if (startTime <= entry.simtime && entry.simtime <= endTime) {
                out.push_back(entry);
                count++;
            }
            else if (entry.simtime > endTime)
                break;
        }
    }
    return count;
}

long IndexedVectorFileReader::collectEntriesInEventnumInterval(eventnumber_t startEventNum, eventnumber_t endEventNum, Entries& out)
{
    std::vector<Block>::size_type startIndex;
    std::vector<Block>::size_type endIndex;
    vector->getBlocksInEventnumInterval(startEventNum, endEventNum,  /*out*/ startIndex,  /*out*/ endIndex);

    Entries::size_type count = 0;
    for (std::vector<Block>::size_type i = startIndex; i < endIndex; i++) {
        const Block& block = vector->blocks[i];
        loadBlock(block);

        for (long j = 0; j < block.getCount(); ++j) {
            OutputVectorEntry& entry = currentEntries[j];
            if (startEventNum <= entry.eventNumber && entry.eventNumber <= endEventNum) {
                out.push_back(entry);
                count++;
            }
            else if (entry.eventNumber > endEventNum)
                break;
        }
    }
    return count;
}


}  // namespace scave
}  // namespace omnetpp

