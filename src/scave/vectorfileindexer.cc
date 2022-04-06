//=========================================================================
//  VECTORFILEINDEXER.CC - part of
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
#include <cerrno>
#include <sstream>
#include <ostream>
#include <cstdlib>
#include "common/opp_ctype.h"
#include "common/stringutil.h"
#include "common/filereader.h"
#include "common/linetokenizer.h"
#include "omnetpp/platdep/platmisc.h"
#include "scaveutils.h"
#include "scaveexception.h"
#include "indexfileutils.h"
#include "indexfilewriter.h"
#include "vectorfileindexer.h"
#include "indexedvectorfilereader.h"
#include "vectorfileindex.h"

using namespace std;
using namespace omnetpp::common;

namespace omnetpp {
namespace scave {

static inline bool existsFile(const string& fileName)
{
    struct stat s;
    return stat(fileName.c_str(), &s) == 0;
}

static string createTempFileName(const string& baseFileName)
{
    string prefix = baseFileName;
    prefix.append(".temp");
    string tmpFileName = prefix;
    int serial = 0;

    while (existsFile(tmpFileName))
        tmpFileName = opp_stringf("%s%d", prefix.c_str(), serial++);
    return tmpFileName;
}

// TODO: adjacent blocks are merged
void VectorFileIndexer::generateIndex(const char *vectorFileName, IProgressMonitor *monitor)
{
    FileReader reader(vectorFileName);
    LineTokenizer tokenizer(1024);
    VectorFileIndex index;
    index.vectorFileName = vectorFileName;

    char *line;
    char **tokens;
    int64_t lineNo;
    int numTokens, numOfUnrecognizedLines = 0;
    VectorInfo *currentVectorRef = nullptr;
    VectorInfo *lastVectorDecl = nullptr;
    Block *currentBlock = new Block();

    int64_t onePercentFileSize = reader.getFileSize() / 100;
    int readPercentage = 0;

    if (monitor)
        monitor->beginTask(string("Indexing ")+vectorFileName, 110);

    try {
        while ((line = reader.getNextLineBufferPointer()) != nullptr) {
            if (monitor) {
                if (monitor->isCanceled()) {
                    monitor->done();
                    return;
                }
                if (onePercentFileSize > 0) {
                    int64_t readBytes = reader.getNumReadBytes();
                    int currentPercentage = readBytes / onePercentFileSize;
                    if (currentPercentage > readPercentage) {
                        monitor->worked(currentPercentage - readPercentage);
                        readPercentage = currentPercentage;
                    }
                }
            }

            tokenizer.tokenize(line, reader.getCurrentLineLength());
            numTokens = tokenizer.numTokens();
            tokens = tokenizer.tokens();
            lineNo = reader.getNumReadLines();

            if (numTokens == 0 || tokens[0][0] == '#')
                continue;
            else if ((tokens[0][0] == 'r' && strcmp(tokens[0], "run") == 0) ||
                     (tokens[0][0] == 'c' && strcmp(tokens[0], "config") == 0) ||
                     (tokens[0][0] == 'p' && strcmp(tokens[0], "param") == 0) ||
                     (tokens[0][0] == 'i' && strcmp(tokens[0], "itervar") == 0))
            {
                index.run.parseLine(tokens, numTokens, vectorFileName, lineNo);
            }
            else if (tokens[0][0] == 'a' && strcmp(tokens[0], "attr") == 0) {
                if (lastVectorDecl == nullptr) {  // run attribute
                    index.run.parseLine(tokens, numTokens, vectorFileName, lineNo);
                }
                else {  // vector attribute
                    if (numTokens < 3)
                        throw ResultFileFormatException("Vector file indexer: Missing attribute name or value", vectorFileName, lineNo);
                    lastVectorDecl->attributes[tokens[1]] = tokens[2];
                }
            }
            else if (tokens[0][0] == 'v' && strcmp(tokens[0], "vector") == 0) {
                if (numTokens < 4)
                    throw ResultFileFormatException("Vector file indexer: Broken vector declaration", vectorFileName, lineNo);

                VectorInfo vector;
                if (!parseInt(tokens[1], vector.vectorId))
                    throw ResultFileFormatException("Vector file indexer: Malformed vector in vector declaration", vectorFileName, lineNo);
                vector.moduleName = tokens[2];
                vector.name = tokens[3];
                vector.columns = (numTokens < 5 || opp_isdigit(tokens[4][0]) ? "TV" : tokens[4]);
                vector.blockSize = 0;

                int currentVectorId = currentVectorRef != nullptr ? currentVectorRef->vectorId : -1; // remember id

                index.addVector(vector);
                lastVectorDecl = index.getVectorAt(index.getNumberOfVectors() - 1);

                if (currentVectorRef != nullptr)
                    currentVectorRef = index.getVectorById(currentVectorId); // refresh currentVectorRef, as index.addVector() might have invalidated it due to std::vector reallocation
            }
            else if (tokens[0][0] == 'v' && strcmp(tokens[0], "version") == 0) {
                int version;
                if (numTokens < 2)
                    throw ResultFileFormatException("Vector file indexer: Missing version number", vectorFileName, lineNo);
                if (!parseInt(tokens[1], version))
                    throw ResultFileFormatException("Vector file indexer: Version is not a number", vectorFileName, lineNo);
                if (version != 2 && version != 3)
                    throw ResultFileFormatException("Vector file indexer: Expects version 2 or version 3", vectorFileName, lineNo);
            }
            else {  // data line
                int vectorId;
                simultime_t simTime;
                double value;
                eventnumber_t eventNum = -1;

                if (!parseInt(tokens[0], vectorId)) {
                    numOfUnrecognizedLines++;
                    continue;
                }

                if (currentVectorRef == nullptr || vectorId != currentVectorRef->vectorId) {
                    if (currentVectorRef != nullptr) {
                        currentBlock->size = (int64_t)(reader.getCurrentLineStartOffset() - currentBlock->startOffset);
                        if (currentBlock->size > currentVectorRef->blockSize)
                            currentVectorRef->blockSize = currentBlock->size;
                        currentVectorRef->addBlock(currentBlock);
                        index.addBlock(currentBlock);
                    }
                    else {
                        Assert(currentBlock->getCount() == 0);
                        delete currentBlock;
                    }

                    currentBlock = new Block();
                    currentBlock->startOffset = reader.getCurrentLineStartOffset();
                    currentVectorRef = index.getVectorById(vectorId);
                    if (currentVectorRef == nullptr)
                        throw ResultFileFormatException("Vector file indexer: Missing vector declaration", vectorFileName, lineNo);
                }

                for (int i = 0; i < (int)currentVectorRef->columns.size(); ++i) {
                    char column = currentVectorRef->columns[i];
                    if (i+1 >= numTokens)
                        throw ResultFileFormatException("Vector file indexer: Data line too short", vectorFileName, lineNo);

                    char *token = tokens[i+1];
                    switch (column) {
                        case 'T':
                            if (!parseSimtime(token, simTime))
                                throw ResultFileFormatException("Vector file indexer: Malformed simulation time", vectorFileName, lineNo);
                            break;

                        case 'V':
                            if (!parseDouble(token, value))
                                throw ResultFileFormatException("Vector file indexer: Malformed data value", vectorFileName, lineNo);
                            break;

                        case 'E':
                            if (!parseInt64(token, eventNum))
                                throw ResultFileFormatException("Vector file indexer: Malformed event number", vectorFileName, lineNo);
                            break;
                    }
                }

                currentBlock->collect(eventNum, simTime, value);
            }
        }

        // finish last block
        if (currentBlock->getCount() == 0) {
            delete currentBlock;
            currentBlock = nullptr;
        }
        else {
            Assert(currentVectorRef != nullptr);
            currentBlock->size = (int64_t)(reader.getFileSize() - currentBlock->startOffset);
            if (currentBlock->size > currentVectorRef->blockSize)
                currentVectorRef->blockSize = currentBlock->size;
            currentVectorRef->addBlock(currentBlock);
            index.addBlock(currentBlock);
            currentBlock = nullptr;
        }

        if (numOfUnrecognizedLines > 0) {
            fprintf(stderr, "Found %d unrecognized lines in %s.\n", numOfUnrecognizedLines, vectorFileName);
        }
    }
    catch (exception&) {
        delete currentBlock;
        if (monitor)
            monitor->done();
        throw;
    }
    if (monitor) {
        if (monitor->isCanceled()) {
            monitor->done();
            return;
        }
        if (readPercentage < 100)
            monitor->worked(100 - readPercentage);
    }

    // generate index file: first write it to a temp file then rename it to .vci;
    // we do this in order to prevent race conditions from other processes/threads
    // reading an incomplete .vci file
    string indexFileName = IndexFileUtils::getIndexFileName(vectorFileName);
    string tempIndexFileName = createTempFileName(indexFileName);

    try {
        IndexFileWriter writer(tempIndexFileName.c_str());
        writer.writeAll(index);

        if (monitor)
            monitor->worked(10);

        // rename generated index file
        if (unlink(indexFileName.c_str()) != 0 && errno != ENOENT)
            throw opp_runtime_error("Cannot remove original index file '%s': %s", indexFileName.c_str(), strerror(errno));
        if (rename(tempIndexFileName.c_str(), indexFileName.c_str()) != 0)
            throw opp_runtime_error("Cannot rename index file from '%s' to '%s': %s", tempIndexFileName.c_str(), indexFileName.c_str(), strerror(errno));
    }
    catch (exception&) {
        if (monitor)
            monitor->done();

        // if something wrong happened, we remove the temp files
        unlink(indexFileName.c_str());
        unlink(tempIndexFileName.c_str());
        throw;
    }
    if (monitor)
        monitor->done();
}

}  // namespace scave
}  // namespace omnetpp

