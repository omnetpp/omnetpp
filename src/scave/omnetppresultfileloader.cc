//=========================================================================
//  OMNETPPRESULTFILELOADER.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga, Tamas Borbely, Zoltan Bojthe
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
   `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cstdlib>
#include <fstream>
#include <sstream>
#include <iostream>
#include <memory>
#include <algorithm>
#include <utility>
#include <functional>

#include "omnetppresultfileloader.h"

#include "common/opp_ctype.h"
#include "common/matchexpression.h"
#include "common/patternmatcher.h"
#include "common/filereader.h"
#include "common/linetokenizer.h"
#include "common/stringtokenizer.h"
#include "common/filereader.h"
#include "common/fileutil.h"
#include "common/commonutil.h"
#include "common/stringutil.h"
#include "omnetpp/platdep/platmisc.h"
#include "indexfile.h"
#include "scaveutils.h"
#include "scaveexception.h"

#ifdef THREADED
#define READER_MUTEX    Mutex __reader_mutex_(getReadLock());
#define WRITER_MUTEX    Mutex __writer_mutex_(getWriteLock());
#else
#define READER_MUTEX
#define WRITER_MUTEX
#endif

using namespace omnetpp::common;

namespace omnetpp {
namespace scave {

#ifdef CHECK
#undef CHECK
#endif
#define CHECK(cond,msg) if (!(cond)) throw ResultFileFormatException(msg, ctx.fileName, ctx.lineNo);


OmnetppResultFileLoader::ParseContext::ParseContext(ResultFile* fileRef) :
        fileRef(fileRef), fileName(fileRef->getFilePath().c_str()), lineNo(0), fileRunRef(nullptr),
        lastResultItemType(0), lastResultItemIndex(-1), count(0),
        min(POSITIVE_INFINITY), max(NEGATIVE_INFINITY), sum(0.0), sumSqr(0.0)
{
}

void OmnetppResultFileLoader::ParseContext::clearHistogram()
{
    moduleName.clear();
    statisticName.clear();
    count = 0;
    min = POSITIVE_INFINITY;
    max = NEGATIVE_INFINITY;
    sum = 0.0;
    sumSqr = 0.0;
}

void OmnetppResultFileLoader::processLine(char **vec, int numTokens, ParseContext& ctx)
{
    ++ctx.lineNo;

    // ignore empty lines
    if (numTokens == 0 || vec[0][0] == '#')
        return;

    // process "run" lines
    if (vec[0][0] == 'r' && !strcmp(vec[0], "run")) {
        CHECK(numTokens == 2, "incorrect 'run' line -- run <runID> expected");

        // "run" line, format: run <runName>
        // find out if we have that run already
        Run *runRef = resultFileManager->getRunByName(vec[1]);
        if (!runRef) {
            // not yet: add it
            runRef = resultFileManager->addRun(vec[1]);
        }
        // associate Run with this file
        CHECK(resultFileManager->getFileRun(ctx.fileRef, runRef) == nullptr, "non-unique runId in the file");
        ctx.fileRunRef = resultFileManager->addFileRun(ctx.fileRef, runRef);

        ctx.lastResultItemType = 0;
        ctx.lastResultItemIndex = -1;
        ctx.clearHistogram();
        return;
    }

    if (vec[0][0] == 'v' && strcmp(vec[0], "version") == 0) {
        int version;
        CHECK(numTokens == 2, "incorrect 'version' line -- version <number> expected");
        CHECK(parseInt(vec[1], version), "version is not a number");
        CHECK(version == 2, "expects version 2");
        return;
    }

    // anything else is within runs
    CHECK(ctx.fileRunRef != nullptr, "line must be preceded by a 'run' line");

    if (vec[0][0] == 's' && !strcmp(vec[0], "scalar")) {
        // syntax: "scalar <module> <scalarname> <value>"
        CHECK(numTokens == 4, "incorrect 'scalar' line -- scalar <module> <scalarname> <value> expected");

        double value;
        CHECK(parseDouble(vec[3], value), "invalid value column");

        ctx.lastResultItemType = ResultFileManager::SCALAR;
        ctx.lastResultItemIndex = resultFileManager->addScalar(ctx.fileRunRef, vec[1], vec[2], value, false);
        ctx.clearHistogram();
    }
    else if (vec[0][0] == 'v' && !strcmp(vec[0], "vector")) {
        // syntax: "vector <id> <module> <vectorname> [<columns>]"
        CHECK(numTokens == 4 || numTokens == 5, "incorrect 'vector' line -- vector <id> <module> <vectorname> [<columns>] expected");
        int vectorId;
        CHECK(parseInt(vec[1], vectorId), "invalid vector id in vector definition");
        const char *columns = (numTokens < 5 || opp_isdigit(vec[4][0]) ? "TV" : vec[4]);

        ctx.lastResultItemType = ResultFileManager::VECTOR;
        ctx.lastResultItemIndex = resultFileManager->addVector(ctx.fileRunRef, vectorId, vec[2], vec[3], columns);
        ctx.clearHistogram();
    }
    else if (vec[0][0] == 's' && !strcmp(vec[0], "statistic")) {
        // syntax: "statistic <module> <statisticname>"
        CHECK(numTokens == 3, "incorrect 'statistic' line -- statistic <module> <statisticname> expected");

        ctx.clearHistogram();
        ctx.moduleName = vec[1];
        ctx.statisticName = vec[2];
        ctx.lastResultItemType = ResultFileManager::SCALAR;  // add scalars first
        ctx.lastResultItemIndex = ctx.fileRef->scalarResults.size();

        CHECK(!ctx.moduleName.empty(), "missing module name");
        CHECK(!ctx.statisticName.empty(), "missing statistics name");
    }
    else if (vec[0][0] == 'f' && !strcmp(vec[0], "field")) {
        // syntax: "field <name> <value>"
        CHECK(numTokens == 3, "incorrect 'field' line -- field <name> <value> expected");

        std::string fieldName = vec[1];
        double value;
        CHECK(parseDouble(vec[2], value), "invalid scalar file: invalid field value");

        CHECK(!ctx.moduleName.empty() && !ctx.statisticName.empty(),
                "invalid scalar file: missing statistics declaration");
        std::string scalarName = ctx.statisticName + ":" + fieldName;

        // set statistics field in the current histogram
        bool isField = true;
        if (fieldName == "count")
            ctx.count = (long)value;
        else if (fieldName == "min")
            ctx.min = value;
        else if (fieldName == "max")
            ctx.max = value;
        else if (fieldName == "sum")
            ctx.sum = value;
        else if (fieldName == "sqrsum")
            ctx.sumSqr = value;
        else if (fieldName != "mean" && fieldName != "stddev")
            isField = false;

        resultFileManager->addScalar(ctx.fileRunRef, ctx.moduleName.c_str(), scalarName.c_str(), value, isField);
    }
    else if (vec[0][0] == 'b' && !strcmp(vec[0], "bin")) {
        // syntax: "bin <lower_bound> <value>"
        CHECK(numTokens == 3, "incorrect 'bin' line -- bin <lowerBound> <value> expected");
        double lower_bound, value;
        CHECK(parseDouble(vec[1], lower_bound), "invalid lower bound");
        CHECK(parseDouble(vec[2], value), "invalid bin value");

        if (ctx.lastResultItemType != ResultFileManager::HISTOGRAM) {
            CHECK(ctx.lastResultItemType == ResultFileManager::SCALAR && !ctx.moduleName.empty() && !ctx.statisticName.empty(),
                    "stray 'bin' line (not preceded by a 'statistic' line)");
            Statistics stat(ctx.count, ctx.min, ctx.max, ctx.sum, ctx.sumSqr);
            const ScalarResults& scalars = ctx.fileRef->scalarResults;
            const StringMap& attrs = ctx.lastResultItemIndex < (int)scalars.size() ?
                scalars[ctx.lastResultItemIndex].getAttributes() : StringMap();
            ctx.lastResultItemType = ResultFileManager::HISTOGRAM;
            ctx.lastResultItemIndex = resultFileManager->addHistogram(ctx.fileRunRef, ctx.moduleName.c_str(), ctx.statisticName.c_str(), stat, attrs);
        }
        HistogramResult& histogram = ctx.fileRef->histogramResults[ctx.lastResultItemIndex];
        try {
            histogram.addBin(lower_bound, value);
        }
        catch (std::exception& e) {
            throw ResultFileFormatException(e.what(), ctx.fileName, ctx.lineNo);
        }
    }
    else if (vec[0][0] == 'a' && !strcmp(vec[0], "attr")) {
        // syntax: "attr <name> <value>"
        CHECK(numTokens == 3, "incorrect 'attr' line -- attr <name> <value> expected");

        std::string attrName = vec[1];
        std::string attrValue = vec[2];

        if (ctx.lastResultItemType == 0) {  // run attribute
            // store attribute
            StringMap& attributes = ctx.fileRunRef->runRef->attributes;
            StringMap::iterator oldPairRef = attributes.find(attrName);
            CHECK(oldPairRef == attributes.end() || oldPairRef->second == attrValue,
                    "Value of run attribute conflicts with previously loaded value");
            attributes[attrName] = attrValue;

            // the "runNumber" attribute is also stored separately
            if (attrName == "runNumber")
                CHECK(parseInt(vec[2], ctx.fileRunRef->runRef->runNumber), "invalid result file: int value expected as runNumber");
        }
        else if (ctx.lastResultItemIndex >= 0) {  // resultItem attribute
            if (ctx.lastResultItemType == ResultFileManager::SCALAR)
                for (int i = ctx.lastResultItemIndex; i < (int)ctx.fileRef->scalarResults.size(); ++i) {
                    ctx.fileRef->scalarResults[i].setAttribute(attrName, attrValue);
                }
            else if (ctx.lastResultItemType == ResultFileManager::VECTOR)
                for (int i = ctx.lastResultItemIndex; i < (int)ctx.fileRef->vectorResults.size(); ++i) {
                    ctx.fileRef->vectorResults[i].setAttribute(attrName, attrValue);
                }
            else if (ctx.lastResultItemType == ResultFileManager::HISTOGRAM)
                for (int i = ctx.lastResultItemIndex; i < (int)ctx.fileRef->histogramResults.size(); ++i) {
                    ctx.fileRef->histogramResults[i].setAttribute(attrName, attrValue);
                }
        }
    }
    else if (vec[0][0] == 'p' && !strcmp(vec[0], "param")) {
        // syntax: "param <namePattern> <value>"
        CHECK(numTokens == 3, "incorrect 'param' line -- param <namePattern> <value> expected");

        // store module param
        std::string paramName = vec[1];
        std::string paramValue = vec[2];
        StringMap& params = ctx.fileRunRef->runRef->moduleParams;
        StringMap::iterator oldPairRef = params.find(paramName);
        CHECK(oldPairRef == params.end() || oldPairRef->second == paramValue,
                "Value of module parameter conflicts with previously loaded value");
        params[paramName] = paramValue;
    }
    else if (opp_isdigit(vec[0][0]) && numTokens >= 3) {
        // this looks like a vector data line, skip it this time
        // syntax: "<vectorID> <2-or-3-columns>"
        CHECK(numTokens == 3 || numTokens == 4, "incorrect vector data line -- 3 or 4 items expected");
    }
    else {
        // ignore unknown lines and vector data lines
        // ctx.fileRef->numUnrecognizedLines++; -- counter not used any more
        CHECK(false, "unrecognized line");
    }
}

ResultFile *OmnetppResultFileLoader::loadFile(const char *fileName, const char *fileSystemFileName, bool reload)
{
    // add to fileList
    ResultFile *fileRef = nullptr;

    try {
        fileRef = resultFileManager->addFile(fileName, fileSystemFileName, ResultFile::FILETYPE_OMNETPP, false);

        // if vector file and has index, load vectors from the index file
        if (IndexFile::isExistingVectorFile(fileSystemFileName) && IndexFile::isIndexFileUpToDate(fileSystemFileName)) {
            std::string indexFileName = IndexFile::getIndexFileName(fileSystemFileName);
            loadVectorsFromIndex(indexFileName.c_str(), fileRef);
        }
        else {
            // process lines in file
            FileReader freader(fileSystemFileName);
            char *line;
            LineTokenizer tokenizer;
            ParseContext ctx(fileRef);
            while ((line = freader.getNextLineBufferPointer()) != nullptr) {
                int len = freader.getCurrentLineLength();
                int numTokens = tokenizer.tokenize(line, len);
                char **tokens = tokenizer.tokens();
                processLine(tokens, numTokens, ctx);
            }
        }
    }
    catch (std::exception&) {
        try {
            if (fileRef)
                resultFileManager->unloadFile(fileRef);
        }
        catch (...) {
        }
        throw;
    }
    return fileRef;
}

void OmnetppResultFileLoader::loadVectorsFromIndex(const char *filename, ResultFile *fileRef)
{
    VectorFileIndex *index = IndexFileReader(filename).readAll();
    int numOfVectors = index->getNumberOfVectors();

    if (numOfVectors == 0) {
        delete index;
        return;
    }

    Run *runRef = resultFileManager->getRunByName(index->run.runName.c_str());
    if (!runRef) {
        runRef = resultFileManager->addRun(index->run.runName);
    }
    runRef->runNumber = index->run.runNumber;
    runRef->attributes = index->run.attributes;
    runRef->moduleParams = index->run.moduleParams;
    FileRun *fileRunRef = resultFileManager->addFileRun(fileRef, runRef);

    for (int i = 0; i < numOfVectors; ++i) {
        const VectorData *vectorRef = index->getVectorAt(i);
        assert(vectorRef);

        VectorResult vectorResult(fileRunRef, vectorRef->moduleName, vectorRef->name, vectorRef->vectorId, vectorRef->columns);
        vectorResult.setAttributes(vectorRef->attributes);
        vectorResult.startEventNum = vectorRef->startEventNum;
        vectorResult.endEventNum = vectorRef->endEventNum;
        vectorResult.startTime = vectorRef->startTime;
        vectorResult.endTime = vectorRef->endTime;
        vectorResult.stat = vectorRef->stat;
        fileRef->vectorResults.push_back(vectorResult);
    }
    delete index;
}

}  // namespace scave
}  // namespace omnetpp
