//=========================================================================
//  INDEXFILE.CC - part of
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

#include <sys/stat.h>
#include <cstdint>
#include <algorithm>
#include <clocale>
#include "common/exception.h"
#include "common/filereader.h"
#include "common/linetokenizer.h"
#include "common/stringutil.h"
#include "scaveutils.h"
#include "scaveexception.h"
#include "indexfile.h"
#include "indexfilereader.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace scave {

using VectorInfo = VectorFileIndex::VectorInfo;
using Block = VectorFileIndex::Block;

static bool serialLess(const Block& first, const Block& second)
{
    return first.endSerial() < second.endSerial();
}

const Block *VectorInfo::getBlockBySerial(long serial) const
{
    if (serial < 0 || serial >= getCount())
        return nullptr;

    Block blockToFind;
    blockToFind.startSerial = serial;
    std::vector<Block>::const_iterator first = std::upper_bound(blocks.begin(), blocks.end(), blockToFind, serialLess);
    assert(first == blocks.end() || first->endSerial() > serial);  // first block ending after serial

    if (first != blocks.end()) {
        assert(first->contains(serial));
        return &(*first);
    }
    else
        return nullptr;
}

// ordering of blocks
static bool simtimeLess(const Block& first, const Block& second)
{
    return first.endTime < second.startTime;
}

// ordering of reversed blocks
static bool simtimeGreater(const Block& first, const Block& second)
{
    return first.startTime > second.endTime;
}

const Block *VectorInfo::getBlockBySimtime(simultime_t simtime, bool after) const
{
    Block blockToFind;
    blockToFind.startTime = simtime;
    blockToFind.endTime = simtime;

    if (after) {
        std::vector<Block>::const_iterator first = std::lower_bound(blocks.begin(), blocks.end(), blockToFind, simtimeLess);
        return first != blocks.end() ? &(*first) : nullptr;
    }
    else {
        std::vector<Block>::const_reverse_iterator last = std::lower_bound(blocks.rbegin(), blocks.rend(), blockToFind, simtimeGreater);
        return last != blocks.rend() ? &(*last) : nullptr;
    }
}

std::vector<Block>::size_type VectorInfo::getBlocksInSimtimeInterval(simultime_t startTime, simultime_t endTime, std::vector<Block>::size_type& startIndex, std::vector<Block>::size_type& endIndex) const
{
    Block blockToFind;
    blockToFind.startTime = startTime;
    blockToFind.endTime = endTime;

    std::vector<Block>::const_iterator first = std::lower_bound(blocks.begin(), blocks.end(), blockToFind, simtimeLess);
    std::vector<Block>::const_iterator last = std::upper_bound(blocks.begin(), blocks.end(), blockToFind, simtimeLess);

    assert(first == blocks.end() || first->endTime >= startTime);
    assert(last == blocks.end() || last->startTime > endTime);
    assert(first <= last);

    startIndex = first - blocks.begin();
    endIndex = last - blocks.begin();
    return endIndex - startIndex;
}

// ordering of blocks
static bool eventnumLess(const Block& first, const Block& second)
{
    return first.endEventNum < second.startEventNum;
}

// ordering of reversed blocks
static bool eventnumGreater(const Block& first, const Block& second)
{
    return first.startEventNum > second.endEventNum;
}

const Block *VectorInfo::getBlockByEventnum(eventnumber_t eventNum, bool after) const
{
    Block blockToFind;
    blockToFind.startEventNum = eventNum;
    blockToFind.endEventNum = eventNum;

    if (after) {
        std::vector<Block>::const_iterator first = std::lower_bound(blocks.begin(), blocks.end(), blockToFind, eventnumLess);
        return first != blocks.end() ? &(*first) : nullptr;
    }
    else {
        std::vector<Block>::const_reverse_iterator last = std::lower_bound(blocks.rbegin(), blocks.rend(), blockToFind, eventnumGreater);
        return last != blocks.rend() ? &(*last) : nullptr;
    }
}

std::vector<Block>::size_type VectorInfo::getBlocksInEventnumInterval(eventnumber_t startEventNum, eventnumber_t endEventNum, std::vector<Block>::size_type& startIndex, std::vector<Block>::size_type& endIndex) const
{
    Block blockToFind;
    blockToFind.startEventNum = startEventNum;
    blockToFind.endEventNum = endEventNum;

    std::vector<Block>::const_iterator first = std::lower_bound(blocks.begin(), blocks.end(), blockToFind, eventnumLess);
    std::vector<Block>::const_iterator last = std::upper_bound(blocks.begin(), blocks.end(), blockToFind, eventnumLess);

    assert(first == blocks.end() || first->endEventNum >= startEventNum);
    assert(last == blocks.end() || last->startEventNum > endEventNum);
    assert(first <= last);

    startIndex = first - blocks.begin();
    endIndex = last - blocks.begin();
    return endIndex - startIndex;
}

//=========================================================================

#ifdef CHECK
#undef CHECK
#endif
#define CHECK(cond,msg) if (!(cond)) throw ResultFileFormatException(msg, filename, lineNo);

bool RunData::parseLine(char **tokens, int numTokens, const char *filename, int64_t lineNo)
{
    if (tokens[0][0] == 'a' && strcmp(tokens[0], "attr") == 0) {
        CHECK(numTokens >= 3, "'attr <name> <value>' expected");
        attributes[tokens[1]] = tokens[2];
        // the "runNumber" attribute is also stored separately
        if (strcmp(tokens[1], "runNumber") == 0) {
            CHECK(parseInt(tokens[2], this->runNumber), "runNumber: an integer expected");
        }
        return true;
    }
    else if (tokens[0][0] == 'i' && strcmp(tokens[0], "itervar") == 0) {
        CHECK(numTokens >= 3, "'itervar <name> <value>' expected");
        itervars[tokens[1]] = tokens[2];
        return true;
    }
    else if (tokens[0][0] == 'p' && strcmp(tokens[0], "param") == 0) {
        CHECK(numTokens >= 3, "'param <namePattern> <value>' expected");
        paramAssignments.push_back(std::make_pair(tokens[1], tokens[2]));
        return true;
    }
    else if (tokens[0][0] == 'r' && strcmp(tokens[0], "run") == 0) {
        CHECK(numTokens >= 2, "missing run name");
        runName = tokens[1];
        return true;
    }

    return false;
}

#undef CHECK
#define CHECK(fprintf)    if ((fprintf)<0) throw opp_runtime_error("Cannot write output file '%s'", filename)

void RunData::writeToFile(FILE *file, const char *filename) const
{
    if (runName.size() > 0)
        CHECK(fprintf(file, "run %s\n", runName.c_str()));
    for (auto it = attributes.begin(); it != attributes.end(); ++it)
        CHECK(fprintf(file, "attr %s %s\n", it->first.c_str(), QUOTE(it->second.c_str())));
    for (auto it = itervars.begin(); it != itervars.end(); ++it)
        CHECK(fprintf(file, "itervar %s %s\n", it->first.c_str(), QUOTE(it->second.c_str())));
    for (auto it = paramAssignments.begin(); it != paramAssignments.end(); ++it)
        CHECK(fprintf(file, "param %s %s\n", it->first.c_str(), QUOTE(it->second.c_str())));
    CHECK(fprintf(file, "\n"));
}

//=========================================================================
static bool isFileReadable(const char *filename)
{
    FILE *f = fopen(filename, "r");
    if (f != nullptr)
        fclose(f);
    return f != nullptr;
}

bool IndexFileUtils::isIndexFile(const char *filename)
{
    return opp_stringendswith(filename, ".vci");
}

bool IndexFileUtils::isExistingVectorFile(const char *filename)
{
    if (!opp_stringendswith(filename, ".vec"))
        return false;

    FILE *f = fopen(filename, "r");
    if (!f)
        return false;

    const char *signature = "version 2";
    char buf[20];
    memset(buf, 0, 20);
    fread(buf, strlen(signature), 1, f);
    fclose(f);
    bool matches = memcmp(buf, signature, strlen(signature)) == 0;
    return matches;
}

std::string IndexFileUtils::getVectorFileName(const char *filename)
{
    std::string vectorFileName(filename);
    std::string::size_type pos = vectorFileName.rfind('.');
    if (pos != std::string::npos)
        vectorFileName.replace(vectorFileName.begin()+pos, vectorFileName.end(), ".vec");
    else
        vectorFileName.append(".vec");
    return vectorFileName;
}

std::string IndexFileUtils::getIndexFileName(const char *filename)
{
    std::string indexFileName(filename);
    std::string::size_type pos = indexFileName.rfind('.');
    if (pos != std::string::npos)
        indexFileName.replace(indexFileName.begin()+pos, indexFileName.end(), ".vci");
    else
        indexFileName.append(".vci");
    return indexFileName;
}

bool IndexFileUtils::isIndexFileUpToDate(const char *filename)
{
    std::string indexFileName, vectorFileName;

    if (isIndexFile(filename)) {
        indexFileName = std::string(filename);
        vectorFileName = getVectorFileName(filename);
    }
    else {
        indexFileName = getIndexFileName(filename);
        vectorFileName = std::string(filename);
    }

    if (!isFileReadable(indexFileName.c_str()))
        return false;

    IndexFileReader reader(indexFileName.c_str());
    VectorFileIndex *index = reader.readFingerprint();

    // when the fingerprint not found assume the index file is being written therefore it is up to date
    if (!index)
        return true;

    bool uptodate = index->fingerprint.check(vectorFileName.c_str());
    delete index;
    return uptodate;
}

//=========================================================================
FingerPrint::FingerPrint(const char *vectorFileName)
{
    struct opp_stat_t s;
    if (opp_stat(vectorFileName, &s) != 0)
        throw opp_runtime_error("Vector file '%s' does not exist", vectorFileName);

    this->lastModified = (int64_t)s.st_mtime;
    this->fileSize = (int64_t)s.st_size;
}

bool FingerPrint::check(const char *vectorFileName)
{
    struct opp_stat_t s;
    if (opp_stat(vectorFileName, &s) == 0) {
        return (this->lastModified == (int64_t)s.st_mtime) && (this->fileSize == (int64_t)s.st_size);
    }
    return false;
}


}  // namespace scave
}  // namespace omnetpp

