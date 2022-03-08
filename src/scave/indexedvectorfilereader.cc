//=========================================================================
//  INDEXEDVECTORFILE.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
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
#include "common/stlutil.h"
#include "omnetpp/platdep/platmisc.h"
#include "indexedvectorfilereader.h"
#include "indexfilereader.h"
#include "indexfileutils.h"
#include "scaveutils.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace scave {

IndexedVectorFileReader::IndexedVectorFileReader(const char *filename, bool includeEventNumbers, AdapterLambdaType adapterLambda, const FileFingerprint& fingerprint)
    : adapterLambda(adapterLambda), fname(filename), includeEventNumbers(includeEventNumbers), expectedFingerprint(fingerprint)
{
    std::string ifname = IndexFileUtils::getIndexFileName(filename);
    IndexFileReader indexReader(ifname.c_str());
    index = indexReader.readAll();
}

IndexedVectorFileReader::~IndexedVectorFileReader()
{
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

Entries IndexedVectorFileReader::loadBlock(const Block& block, std::function<bool(const VectorDatum&)> filter)
{
    FileFingerprint actualFingerprint = readFileFingerprint(fname.c_str());
    if (!expectedFingerprint.isEmpty() && actualFingerprint != expectedFingerprint)
        throw opp_runtime_error("Vector file \"%s\" changed on disk", fname.c_str());
    if (actualFingerprint != index->fingerprint)
        throw opp_runtime_error("Index file (.vci) for \"%s\" is out of date", fname.c_str());

    std::vector<VectorDatum> result;

    VectorInfo *vector = index->getVectorById(block.vectorId);

    size_t bufferSize = vector->blockSize;
    if (bufferSize < MIN_BUFFER_SIZE)
        bufferSize = MIN_BUFFER_SIZE;
    FileReader reader(fname.c_str(), bufferSize);

    long count = block.getCount();
    reader.seekTo(block.startOffset);

    result.reserve(count);

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

        VectorDatum entry;
        entry.serial = block.startSerial+i;
        for (int j = 0; j < columnsNo; ++j) {
            switch (columns[j]) {
                case 'E': if (includeEventNumbers) { CHECK(parseInt64(tokens[j+1], entry.eventNumber), "Malformed event number", block, i); } break;
                case 'T': CHECK(parseSimtime(tokens[j+1], entry.simtime), "Malformed simulation time", block, i); break;
                case 'V': CHECK(parseDouble(tokens[j+1], entry.value), "Malformed vector value", block, i); break;
                default: CHECK(false, "Unknown column", block, i); break;
            }
        }

        if (!filter || filter(entry))
            result.push_back(entry);
    }
    return result;
}

VectorDatum *IndexedVectorFileReader::getEntryBySerial(int vectorId, int64_t serial)
{
    VectorInfo *vector = index->getVectorById(vectorId);
    if (!vector)
        return nullptr;

    const Block *block = vector->getBlockBySerial(serial);
    if (!block)
        return nullptr;

    Entries data = loadBlock(*block);

    return new VectorDatum(data.at(serial - block->startSerial));
}

VectorDatum *IndexedVectorFileReader::getEntryBySimtime(int vectorId, simultime_t simtime, bool after)
{
    VectorInfo *vector = index->getVectorById(vectorId);
    if (!vector)
        return nullptr;

    const Block *block = vector->getBlockBySimtime(simtime, after);
    if (!block)
        return nullptr;

    Entries data = loadBlock(*block);

    VectorDatum datumToFind;
    datumToFind.simtime = simtime;

    if (after) {
        auto first = std::lower_bound(data.begin(), data.end(), datumToFind, [](const VectorDatum &a, const VectorDatum &b) { return a.simtime < b.simtime; } );
        return first != data.end() ? new VectorDatum(*first) : nullptr;
    }
    else {
        auto last = std::lower_bound(data.rbegin(), data.rend(), datumToFind, [](const VectorDatum &a, const VectorDatum &b) { return a.simtime > b.simtime; });
        return last != data.rend() ? new VectorDatum(*last) : nullptr;
    }
}

VectorDatum *IndexedVectorFileReader::getEntryByEventnum(int vectorId, eventnumber_t eventNum, bool after)
{
    VectorInfo *vector = index->getVectorById(vectorId);
    if (!vector)
        return nullptr;

    const Block *block = vector->getBlockByEventnum(eventNum, after);
    if (!block)
        return nullptr;

    Entries data = loadBlock(*block);

    VectorDatum datumToFind;
    datumToFind.eventNumber = eventNum;

    if (after) {
        auto first = std::lower_bound(data.begin(), data.end(), datumToFind, [](const VectorDatum &a, const VectorDatum &b) { return a.eventNumber < b.eventNumber; } );
        return first != data.end() ? new VectorDatum(*first) : nullptr;
    }
    else {
        auto last = std::lower_bound(data.rbegin(), data.rend(), datumToFind, [](const VectorDatum &a, const VectorDatum &b) { return a.eventNumber > b.eventNumber; });
        return last != data.rend() ? new VectorDatum(*last) : nullptr;
    }
}

void IndexedVectorFileReader::collectEntries(const std::set<int>& vectorIds)
{
    for (auto block : index->getBlocks()) {
        if (contains(vectorIds, block->vectorId)) {
            std::vector<VectorDatum> data = loadBlock(*block);
            adapterLambda(block->vectorId, data);
        }
    }
}

void IndexedVectorFileReader::collectEntriesInSimtimeInterval(const std::set<int>& vectorIds, simultime_t startTime, simultime_t endTime)
{
    for (const auto &block : index->getBlocks()) {
        if (contains(vectorIds, block->vectorId)) {
            if (block->endTime < startTime || block->startTime >= endTime) {
                // no-op, block is completely out of filtered range
            }
            else if (block->startTime >= startTime && block->endTime < endTime) {
                // no need for filter, completely in range
                std::vector<VectorDatum> data = loadBlock(*block);
                adapterLambda(block->vectorId, data);
            }
            else {
                // block is partially in range
                auto filter = [startTime, endTime](const VectorDatum& datum) -> bool {
                    return datum.simtime >= startTime && datum.simtime < endTime;
                };

                std::vector<VectorDatum> data = loadBlock(*block, filter);
                adapterLambda(block->vectorId, data);
            }
        }
    }
}

void IndexedVectorFileReader::collectEntriesInEventnumInterval(const std::set<int>& vectorIds, eventnumber_t startEventNum, eventnumber_t endEventNum)
{
    for (auto block : index->getBlocks()) {
        if (contains(vectorIds, block->vectorId)) {
            if (block->endEventNum < startEventNum || block->startEventNum >= endEventNum) {
                // no-op, block is completely out of filtered range
            }
            else if (block->startEventNum >= startEventNum && block->endEventNum < endEventNum) {
                // no need for filter, completely in range
                std::vector<VectorDatum> data = loadBlock(*block);
                adapterLambda(block->vectorId, data);
            }
            else {
                // block is partially in range
                auto filter = [startEventNum, endEventNum](const VectorDatum& datum) -> bool {
                    return datum.eventNumber >= startEventNum && datum.eventNumber < endEventNum;
                };

                std::vector<VectorDatum> data = loadBlock(*block, filter);
                adapterLambda(block->vectorId, data);
            }
        }
    }
}


}  // namespace scave
}  // namespace omnetpp

