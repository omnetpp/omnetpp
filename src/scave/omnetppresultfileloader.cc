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
#include "common/stlutil.h"
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


void OmnetppResultFileLoader::processLine(char **vec, int numTokens, ParseContext& ctx)
{
    ++ctx.lineNo;

    // ignore empty lines
    if (numTokens == 0 || vec[0][0] == '#')
        return;

    if (vec[0][0] == 'v' && strcmp(vec[0], "version") == 0) {
        int version;
        CHECK(numTokens == 2, "incorrect 'version' line -- version <number> expected");
        CHECK(parseInt(vec[1], version), "version is not a number");
        CHECK(version == 2, "expects version 2");
        return;
    }

    // process "run" lines
    if (vec[0][0] == 'r' && !strcmp(vec[0], "run")) {
        flush(ctx); // last result item in previous run

        // "run" line, format: run <runName>
        CHECK(numTokens == 2, "incorrect 'run' line -- run <runID> expected");

        ctx.currentItemType = ParseContext::RUN;
        ctx.runName = vec[1];
        return;
    }

    if (vec[0][0] == 's' && !strcmp(vec[0], "scalar")) {
        flush(ctx);

        // syntax: "scalar <module> <scalarname> <value>"
        CHECK(ctx.currentItemType != ParseContext::NONE, "stray 'scalar' line, must be under a 'run'");
        CHECK(numTokens == 4, "incorrect 'scalar' line -- scalar <module> <scalarname> <value> expected");

        double value;
        CHECK(parseDouble(vec[3], value), "invalid value column");

        ctx.currentItemType = ParseContext::SCALAR;
        ctx.moduleName = vec[1];
        ctx.resultName = vec[2];
        ctx.scalarValue = value;
    }
    else if (vec[0][0] == 'v' && !strcmp(vec[0], "vector")) {
        flush(ctx);

        // syntax: "vector <id> <module> <vectorname> [<columns>]"
        CHECK(ctx.currentItemType != ParseContext::NONE, "stray 'vector' line, must be under a 'run'");
        CHECK(numTokens == 4 || numTokens == 5, "incorrect 'vector' line -- vector <id> <module> <vectorname> [<columns>] expected");

        int vectorId;
        CHECK(parseInt(vec[1], vectorId), "invalid vector id in vector definition");
        const char *columns = (numTokens < 5 || opp_isdigit(vec[4][0]) ? "TV" : vec[4]);

        ctx.currentItemType = ParseContext::VECTOR;
        ctx.moduleName = vec[2];
        ctx.resultName = vec[3];
        ctx.vectorColumns = columns;
    }
    else if (vec[0][0] == 's' && !strcmp(vec[0], "statistic")) {
        flush(ctx);

        // syntax: "statistic <module> <statisticname>"
        CHECK(ctx.currentItemType != ParseContext::NONE, "stray 'statistic' line, must be under a 'run'");
        CHECK(numTokens == 3, "incorrect 'statistic' line -- statistic <module> <statisticname> expected");

        ctx.currentItemType = ParseContext::STATISTICS;
        ctx.moduleName = vec[1];
        ctx.resultName = vec[2];
    }
    else if (vec[0][0] == 'f' && !strcmp(vec[0], "field")) {
        // syntax: "field <name> <value>"
        CHECK(ctx.currentItemType == ParseContext::STATISTICS, "stray 'field' line, must be under a 'statistic'");
        CHECK(numTokens == 3, "incorrect 'field' line -- field <name> <value> expected");

        std::string fieldName = vec[1];
        double value;
        CHECK(parseDouble(vec[2], value), "invalid scalar file: invalid field value");

        if (fieldName == "count")
            ctx.fields.count = value;
        else if (fieldName == "min")
            ctx.fields.minValue = value;
        else if (fieldName == "max")
            ctx.fields.maxValue = value;
        else if (fieldName == "sum")
            ctx.fields.sum = value;
        else if (fieldName == "sqrsum")
            ctx.fields.sumSquares = value;
        else if (fieldName == "weights")
            ctx.fields.sumWeights = value;
        else if (fieldName == "weightedSum")
            ctx.fields.sumWeightedValues = value;
        else if (fieldName == "sqrSumWeights")
            ctx.fields.sumSquaredWeights = value;
        else if (fieldName == "weightedSqrSum")
            ctx.fields.sumWeightedSquaredValues = value;
    }
    else if (vec[0][0] == 'b' && !strcmp(vec[0], "bin")) {
        // syntax: "bin <lower_bound> <value>"
        if (ctx.currentItemType == ParseContext::STATISTICS)
            ctx.currentItemType = ParseContext::HISTOGRAM;
        CHECK(ctx.currentItemType == ParseContext::HISTOGRAM, "stray 'bin' line, must be under a 'statistic'");
        CHECK(numTokens == 3, "incorrect 'bin' line -- bin <lowerBound> <value> expected");

        double lowerBound, value;
        CHECK(parseDouble(vec[1], lowerBound), "invalid lower bound");
        CHECK(parseDouble(vec[2], value), "invalid bin value");

        try {
            ctx.bins.addBin(lowerBound, value);
        }
        catch (std::exception& e) {
            throw ResultFileFormatException(e.what(), ctx.fileName, ctx.lineNo);
        }
    }
    else if (vec[0][0] == 'a' && !strcmp(vec[0], "attr")) {
        // syntax: "attr <name> <value>"
        CHECK(ctx.currentItemType != ParseContext::NONE, "stray 'attr' line");
        CHECK(numTokens == 3, "incorrect 'attr' line -- attr <name> <value> expected");

        std::string attrName = vec[1];
        std::string attrValue = vec[2];
        ctx.attrs[attrName] = attrValue;
    }
    else if (vec[0][0] == 'i' && !strcmp(vec[0], "itervar")) {
        // syntax: "itervar <name> <value>"
        CHECK(ctx.currentItemType == ParseContext::RUN, "stray 'itervar' line, must be under a 'run' line");
        CHECK(numTokens == 3, "incorrect 'itervar' line -- itervar <name> <value> expected");

        std::string name = vec[1];
        std::string value = vec[2];
        ctx.itervars[name] = value;
    }
    else if (vec[0][0] == 'p' && !strcmp(vec[0], "param")) {
        // syntax: "param <namePattern> <value>"
        CHECK(ctx.currentItemType == ParseContext::RUN, "stray 'param' line, must be under a 'run' line");
        CHECK(numTokens == 3, "incorrect 'param' line -- param <namePattern> <value> expected");

        std::string paramName = vec[1];
        std::string paramValue = vec[2];
        ctx.moduleParams.push_back(std::make_pair(paramName, paramValue));
    }
    else if (opp_isdigit(vec[0][0]) && numTokens >= 3) {
        // this looks like a vector data line, skip it this time
        // syntax: "<vectorID> <2-or-3-columns>"
        CHECK(numTokens == 3 || numTokens == 4, "incorrect vector data line -- 3 or 4 items expected");
    }
    else {
        // ignore unknown lines and vector data lines
        CHECK(false, "unrecognized line");
    }
}

void OmnetppResultFileLoader::flush(ParseContext& ctx)
{
    if (ctx.fileRunRef == nullptr && ctx.currentItemType != ParseContext::NONE && ctx.currentItemType != ParseContext::RUN)
        CHECK(false, "line must be preceded by a 'run' line");

    // add item to results
    switch (ctx.currentItemType) {
    case ParseContext::NONE:
        break;
    case ParseContext::RUN: {
        Run *runRef = resultFileManager->getOrAddRun(ctx.runName);
        ctx.fileRunRef = resultFileManager->getOrAddFileRun(ctx.fileRef, runRef);
        separateItervarsFromAttrs(ctx.attrs, ctx.itervars);
        addAll(runRef->attributes, ctx.attrs);  // note: merge/overwrite attributes if run already existed
        addAll(runRef->itervars, ctx.itervars);
        addAll(runRef->paramAssignments, ctx.moduleParams);
        break;
    }
    case ParseContext::SCALAR: {
        resultFileManager->addScalar(ctx.fileRunRef, ctx.moduleName.c_str(), ctx.resultName.c_str(), ctx.attrs, ctx.scalarValue, false, false);
        break;
    }
    case ParseContext::VECTOR: {
        resultFileManager->addVector(ctx.fileRunRef, ctx.vectorId,ctx.moduleName.c_str(), ctx.resultName.c_str(), ctx.attrs, ctx.vectorColumns.c_str());
        break;
    }
    case ParseContext::STATISTICS: {
        Statistics stats = makeStatsFromFields(ctx);
        resultFileManager->addStatistics(ctx.fileRunRef, ctx.moduleName.c_str(), ctx.resultName.c_str(), stats, ctx.attrs);
        break;
    }
    case ParseContext::HISTOGRAM: {
        Statistics stats = makeStatsFromFields(ctx);
        resultFileManager->addHistogram(ctx.fileRunRef, ctx.moduleName.c_str(), ctx.resultName.c_str(), stats, ctx.bins, ctx.attrs);
        break;
    }
    default:
        throw opp_runtime_error("invalid result type");
    }

    // reset
    if (ctx.currentItemType != ParseContext::NONE)
        ctx.currentItemType = ParseContext::RUN;
    ctx.moduleName.clear();
    ctx.resultName.clear();
    ctx.vectorId = -1;
    ctx.scalarValue = NaN;
    ctx.attrs.clear();
    resetFields(ctx);
    ctx.bins.clear();
}

void OmnetppResultFileLoader::resetFields(ParseContext& ctx)
{
    ctx.fields.count = -1;
    ctx.fields.minValue = ctx.fields.maxValue = ctx.fields.sum = ctx.fields.sumSquares = NaN;
    ctx.fields.sumWeights = ctx.fields.sumWeightedValues = ctx.fields.sumSquaredWeights = ctx.fields.sumWeightedSquaredValues = NaN;
}

Statistics OmnetppResultFileLoader::makeStatsFromFields(ParseContext& ctx)
{
    bool weighted = !std::isnan(ctx.fields.sumWeights);
    if (!weighted)
        return Statistics::makeUnweighted(ctx.fields.count, ctx.fields.minValue, ctx.fields.maxValue, ctx.fields.sum, ctx.fields.sumSquares);
    else
        return Statistics::makeWeighted(ctx.fields.count, ctx.fields.minValue, ctx.fields.maxValue, ctx.fields.sumWeights, ctx.fields.sumWeightedValues, ctx.fields.sumSquaredWeights, ctx.fields.sumWeightedSquaredValues);
}

void OmnetppResultFileLoader::separateItervarsFromAttrs(StringMap& attrs, StringMap& itervars)
{
    // Before version 5.1, files didn't have 'itervar' lines, iteration variables were saved as run attributes.
    // This method identifies iteration variables, and moves them from the attrs stringmap into itervars.
    if (itervars.empty()) {
        std::string itervarsAttr = attrs.find("iterationvars") != attrs.end() ? attrs["iterationvars"] : "";
        if (!itervarsAttr.empty()) {
            for (auto part : opp_split(itervarsAttr, ", ")) {
                std::string varName = opp_substringafter(opp_substringbefore(part, "="), "$");
                if (attrs.find(varName) != attrs.end()) {
                    itervars[varName] = attrs[varName];
                    attrs.erase(varName);
                }
            }
        }
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
            ParseContext ctx;
            ctx.fileRef = fileRef;
            ctx.fileName = fileRef->getFilePath().c_str();
            resetFields(ctx);
            while ((line = freader.getNextLineBufferPointer()) != nullptr) {
                int len = freader.getCurrentLineLength();
                int numTokens = tokenizer.tokenize(line, len);
                char **tokens = tokenizer.tokens();
                processLine(tokens, numTokens, ctx);
            }
            flush(ctx); // last result item
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
    if (!runRef)
        runRef = resultFileManager->addRun(index->run.runName);

    separateItervarsFromAttrs(index->run.attributes, index->run.itervars);

    runRef->attributes = index->run.attributes;
    runRef->itervars = index->run.itervars;
    runRef->paramAssignments = index->run.paramAssignments;
    FileRun *fileRunRef = resultFileManager->addFileRun(fileRef, runRef);

    const StringMap emptyAttrs;
    for (int i = 0; i < numOfVectors; ++i) {
        const VectorData *vectorRef = index->getVectorAt(i);
        assert(vectorRef);

        VectorResult vectorResult(fileRunRef, vectorRef->moduleName, vectorRef->name, emptyAttrs, vectorRef->vectorId, vectorRef->columns);
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
