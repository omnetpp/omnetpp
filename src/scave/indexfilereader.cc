//=========================================================================
//  INDEXFILEREADER.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
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
#include "indexfilereader.h"
#include "vectorfileindex.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace scave {

using VectorInfo = VectorFileIndex::VectorInfo;
using Block = VectorFileIndex::Block;

#ifdef CHECK
#undef CHECK
#endif
#define CHECK(cond,msg,line) if (!(cond)) throw ResultFileFormatException(msg, filename.c_str(), line);

// see ifilemgr.h
#define MIN_BUFFER_SIZE           512

IndexFileReader::IndexFileReader(const char *filename)
    : filename(filename)
{
}

VectorFileIndex *IndexFileReader::readAll()
{
    // read vector and blocks
    FileReader reader(filename.c_str());
    LineTokenizer tokenizer(1024);
    int numTokens;
    char *line, **tokens;

    VectorFileIndex *index = new VectorFileIndex();
    reader.setCheckFileForChanges(false);
    while ((line = reader.getNextLineBufferPointer()) != nullptr) {
        int64_t lineNum = reader.getNumReadLines();
        int len = reader.getCurrentLineLength();
        numTokens = tokenizer.tokenize(line, len);
        tokens = tokenizer.tokens();
        parseLine(tokens, numTokens, index, lineNum);
    }
    return index;
}

FileFingerprint IndexFileReader::readRecordedFingerprint()
{
    FileReader reader(filename.c_str());
    LineTokenizer tokenizer(1024);
    int numTokens;
    char *line, **tokens;

    FileFingerprint result;

    reader.setCheckFileForChanges(false);
    while ((line = reader.getNextLineBufferPointer()) != nullptr) {
        int64_t lineNum = reader.getNumReadLines();
        int len = reader.getCurrentLineLength();
        numTokens = tokenizer.tokenize(line, len);
        tokens = tokenizer.tokens();

        if (numTokens == 0 || tokens[0][0] == '#')
            continue;
        else if (strcmp(tokens[0], "file") == 0) {
            int64_t fileSize;
            int64_t lastModified;
            CHECK(numTokens >= 3, "missing file attributes", lineNum);
            CHECK(parseInt64(tokens[1], fileSize), "file size is not a number", lineNum);
            CHECK(parseInt64(tokens[2], lastModified), "modification date is not a number", lineNum);
            result.fileSize = fileSize;
            result.lastModified = lastModified;
        }
        else
            break;
    }

    return result;
}

void IndexFileReader::parseLine(char **tokens, int numTokens, VectorFileIndex *index, int64_t lineNum)
{
    if (numTokens == 0 || tokens[0][0] == '#')
        return;

    long count;
    double min;
    double max;
    double sum;
    double sumSqr;

    if (tokens[0][0] == 'v' && strcmp(tokens[0], "vector") == 0) {
        CHECK(numTokens >= 5, "invalid vector declaration", lineNum);

        VectorInfo vector;
        CHECK(parseInt(tokens[1], vector.vectorId), "invalid vector id", lineNum);
        vector.moduleName = tokens[2];
        vector.name = tokens[3];
        vector.columns = tokens[4];

        index->addVector(vector);
    }
    else if (tokens[0][0] == 'a' && strcmp(tokens[0], "attr") == 0 && index->getNumberOfVectors() > 0) {  // vector attr
        CHECK(numTokens == 3, "malformed vector attribute", lineNum);
        VectorInfo *lastVector = index->getVectorAt(index->getNumberOfVectors()-1);
        lastVector->attributes[tokens[1]] = tokens[2];
    }
    else if (tokens[0][0] == 'f' && strcmp(tokens[0], "file") == 0) {
        int64_t fileSize;
        int64_t lastModified;
        CHECK(numTokens >= 3, "missing file attributes", lineNum);
        CHECK(parseInt64(tokens[1], fileSize), "file size is not a number", lineNum);
        CHECK(parseInt64(tokens[2], lastModified), "modification date is not a number", lineNum);
        index->fingerprint.fileSize = fileSize;
        index->fingerprint.lastModified = lastModified;
    }
    else if (tokens[0][0] == 'v' && strcmp(tokens[0], "version") == 0) {
        int version;
        CHECK(numTokens >= 2, "missing version number", lineNum);
        CHECK(parseInt(tokens[1], version), "version is not a number", lineNum);
        CHECK(version == 2 || version == 3, "unsupported file version (version 2 or version 3 expected)", lineNum);
    }
    else if (index->run.parseLine(tokens, numTokens, filename.c_str(), lineNum)) {
        return;
    }
    else {  // blocks
        CHECK(numTokens >= 10, "missing fields from block", lineNum);

        int id;
        CHECK(parseInt(tokens[0], id), "malformed vector id", lineNum);
        VectorInfo *vector = index->getVectorById(id);
        CHECK(vector, "missing vector definition", lineNum);

        Block *block = new Block();
        block->vectorId = id;
        block->startSerial = !vector->blocks.empty() ? vector->blocks.back()->endSerial() : 0;
        int i = 1;  // column index
        CHECK(parseInt64(tokens[i++], block->startOffset), "invalid file offset", lineNum);
        CHECK(parseInt64(tokens[i++], block->size), "invalid block size", lineNum);
        if (vector->hasColumn('E')) {
            CHECK(parseInt64(tokens[i++], block->startEventNum) && parseInt64(tokens[i++], block->endEventNum),
                    "invalid event numbers", lineNum);
        }
        if (vector->hasColumn('T')) {
            CHECK(parseSimtime(tokens[i++], block->startTime) && parseSimtime(tokens[i++], block->endTime),
                    "invalid simulation time", lineNum);
        }
        if (vector->hasColumn('V')) {
            CHECK(parseLong(tokens[i++], count) && parseDouble(tokens[i++], min) && parseDouble(tokens[i++], max) &&
                    parseDouble(tokens[i++], sum) && parseDouble(tokens[i++], sumSqr), "invalid statistics data", lineNum);
            block->stat = Statistics::makeUnweighted(count, min, max, sum, sumSqr);
        }
        index->addBlock(block);
        vector->addBlock(block);
    }
}


}  // namespace scave
}  // namespace omnetpp

