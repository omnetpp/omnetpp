//=========================================================================
//  OMNETPPRESULTFILELOADER.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
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
#include "indexfileutils.h"
#include "indexfilereader.h"
#include "scaveutils.h"
#include "scaveexception.h"
#include "vectorfileindex.h"
#include "vectorfileindexer.h"
#include "interruptedflag.h"

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

#define LOG !verbose ? std::cout : std::cout

OmnetppResultFileLoader::OmnetppResultFileLoader(ResultFileManager *resultFileManagerPar, int flags, InterruptedFlag *interrupted) :
    IResultFileLoader(resultFileManagerPar),
    indexingOption(flags & (ResultFileManager::ALLOW_INDEXING|ResultFileManager::SKIP_IF_NO_INDEX|ResultFileManager::ALLOW_LOADING_WITHOUT_INDEX)),
    lockfileOption(flags & (ResultFileManager::SKIP_IF_LOCKED|ResultFileManager::IGNORE_LOCK_FILE)),
    verbose(flags & ResultFileManager::VERBOSE),
    interrupted(interrupted)
{
}


void OmnetppResultFileLoader::processLine(char **vec, int numTokens, ParseContext& ctx)
{
    ++ctx.lineNo;

    if (interrupted->flag)
        throw InterruptedException("Result file loading interrupted");

    // ignore empty lines
    if (numTokens == 0 || vec[0][0] == '#')
        return;

    if (vec[0][0] == 'v' && strcmp(vec[0], "version") == 0) {
        int version;
        CHECK(numTokens == 2, "incorrect 'version' line -- version <number> expected");
        CHECK(parseInt(vec[1], version), "version is not a number");
        CHECK(version == 2 || version == 3, "unsupported file version (version 2 or version 3 expected)");
        return;
    }

    // process "run" lines
    if (vec[0][0] == 'r' && !strcmp(vec[0], "run")) {
        flush(ctx); // last result item in previous run

        // "run" line, format: run <runName>
        CHECK(numTokens == 2, "incorrect 'run' line -- run <runID> expected");

        ctx.currentItemType = ParseContext::RUN;
        ctx.runName = vec[1];
        LOG << ctx.runName << " " << std::flush;
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
    else if (vec[0][0] == 'p' && !strcmp(vec[0], "par")) {
        flush(ctx);

        // syntax: "par <module> <paramname> <value>"
        CHECK(ctx.currentItemType != ParseContext::NONE, "stray 'par' line, must be under a 'run'");
        CHECK(numTokens == 4, "incorrect 'par' line -- par <module> <paramname> <value> expected");

        ctx.currentItemType = ParseContext::PARAMETER;
        ctx.moduleName = vec[1];
        ctx.resultName = vec[2];
        ctx.paramValue = vec[3];
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
        ctx.vectorId = vectorId;
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

        double binLowerEdge, value;
        CHECK(parseDouble(vec[1], binLowerEdge), "invalid bin edge");
        CHECK(parseDouble(vec[2], value), "invalid bin value");

        if (ctx.binEdges.empty() || binLowerEdge > ctx.binEdges.back()) {
            ctx.binEdges.push_back(binLowerEdge);
            ctx.binValues.push_back(value);
        }
        else if (binLowerEdge == ctx.binEdges.back())  // this can happen if nearly-equal bin edges are printed with insufficient precision, generating the same character sequence in the file
            ctx.binValues.back() += value; // solution: merge those bins
        else
            CHECK(false, "bin edges must be increasing");
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
    else if (vec[0][0] == 'c' && !strcmp(vec[0], "config")) {
        // syntax: "config <key> <value>"
        CHECK(ctx.currentItemType == ParseContext::RUN, "stray 'config' line, must be under a 'run' line");
        CHECK(numTokens == 3, "incorrect 'config' line -- config <key> <value> expected");

        std::string key = vec[1];
        std::string value = vec[2];
        ctx.configEntries.push_back(std::make_pair(key, value));
    }
    else if (vec[0][0] == 'p' && !strcmp(vec[0], "param")) {
        // "param" is an obsolete form of "config", just for parameter values; we treat it exactly like "config".
        // syntax: "param <key> <value>"
        CHECK(ctx.currentItemType == ParseContext::RUN, "stray 'param' line, must be under a 'run' line");
        CHECK(numTokens == 3, "incorrect 'param' line -- param <namePattern> <value> expected");

        std::string paramName = vec[1];
        std::string paramValue = vec[2];
        ctx.configEntries.push_back(std::make_pair(paramName, paramValue));
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
        Run *existingRun = resultFileManager->getRunByName(ctx.runName.c_str());
        if (existingRun) {
            ctx.fileRunRef = resultFileManager->getOrAddFileRun(ctx.fileRef, existingRun);
            // TODO check for consistency, or merge/overwrite attributes
        }
        else {
            Run *runRef = resultFileManager->getOrAddRun(ctx.runName);
            ctx.fileRunRef = resultFileManager->getOrAddFileRun(ctx.fileRef, runRef);
            separateItervarsFromAttrs(ctx.attrs, ctx.itervars);
            addAll(runRef->attributes, ctx.attrs);
            addAll(runRef->itervars, ctx.itervars);
            addAll(runRef->configEntries, ctx.configEntries);
        }
        break;
    }
    case ParseContext::SCALAR: {
        resultFileManager->addScalar(ctx.fileRunRef, ctx.moduleName.c_str(), ctx.resultName.c_str(), ctx.attrs, ctx.scalarValue, false);
        break;
    }
    case ParseContext::PARAMETER: {
        resultFileManager->addParameter(ctx.fileRunRef, ctx.moduleName.c_str(), ctx.resultName.c_str(), ctx.attrs, ctx.paramValue);
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
        Histogram bins;
        if (ctx.binEdges.size() == ctx.binValues.size()+1)
            bins.setBins(ctx.binEdges, ctx.binValues);
        else if (ctx.binEdges.size() == ctx.binValues.size()) {
            bins.setUnderflows(ctx.binValues.front());
            bins.setOverflows(ctx.binValues.back());
            ctx.binEdges.erase(ctx.binEdges.begin()); // "-inf"
            ctx.binValues.erase(ctx.binValues.begin()); // underflow
            ctx.binValues.erase(ctx.binValues.end()-1); // overflow
            bins.setBins(ctx.binEdges, ctx.binValues);
        }
        else {
            CHECK(false, "number of bin edges and bin values do not match");
        }
        resultFileManager->addHistogram(ctx.fileRunRef, ctx.moduleName.c_str(), ctx.resultName.c_str(), stats, bins, ctx.attrs);
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
    ctx.paramValue.clear();
    ctx.attrs.clear();
    ctx.itervars.clear();
    ctx.configEntries.clear();
    resetFields(ctx);
    ctx.binEdges.clear();
    ctx.binValues.clear();
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
            for (auto part : opp_splitandtrim(itervarsAttr, ",")) {
                std::string varName = opp_substringafter(opp_substringbefore(part, "="), "$");
                if (attrs.find(varName) != attrs.end()) {
                    itervars[varName] = attrs[varName];
                    attrs.erase(varName);
                }
            }
        }
    }
}

ResultFile *OmnetppResultFileLoader::loadFile(const char *displayName, const char *fileSystemFileName)
{
    // add to fileList
    ResultFile *fileRef = nullptr;

    try {
        //TODO handle lockfileOption

        bool isVecFile = IndexFileUtils::isExistingVectorFile(fileSystemFileName);
        bool hasUpToDateIndex = isVecFile && IndexFileUtils::isIndexFileUpToDate(fileSystemFileName);
        if (isVecFile && !hasUpToDateIndex) {
            // vector file with a missing or out-of-date index
            LOG << "file " << fileSystemFileName << " has no valid index, ";
            switch (indexingOption) {
            case ResultFileManager::SKIP_IF_NO_INDEX: LOG << "skipping\n"; return nullptr;
            case ResultFileManager::ALLOW_LOADING_WITHOUT_INDEX: LOG << "scanning vec file instead of vci\n"; break;
            case ResultFileManager::ALLOW_INDEXING: {
                LOG << "reindexing..." << std::flush;
                VectorFileIndexer().generateIndex(fileSystemFileName, nullptr);
                hasUpToDateIndex = true;
                LOG << "done\n";
                break;
            }
            }
        }

        fileRef = resultFileManager->addFile(displayName, fileSystemFileName, ResultFile::FILETYPE_OMNETPP);


        if (isVecFile && hasUpToDateIndex) {
            // load vectors from the index file
            std::string indexFileName = IndexFileUtils::getIndexFileName(fileSystemFileName);
            LOG << "reading " << indexFileName << "... " << std::flush;
            loadVectorsFromIndex(indexFileName.c_str(), fileRef);
            LOG << "done\n";
        }
        else {
            LOG << "reading " << fileSystemFileName << "... " << std::flush;
            doLoadFile(fileSystemFileName, fileRef);
            LOG << "done\n";
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

void OmnetppResultFileLoader::doLoadFile(const char *fileName, ResultFile *fileRef)
{
    // process lines in file
    FileReader freader(fileName);
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
    runRef->configEntries = index->run.configEntries;
    FileRun *fileRunRef = resultFileManager->addFileRun(fileRef, runRef);

    const StringMap emptyAttrs;
    for (int i = 0; i < numOfVectors; ++i) {
        const VectorInfo *vectorRef = index->getVectorAt(i);
        assert(vectorRef);

        VectorResult vectorResult(fileRunRef, vectorRef->moduleName, vectorRef->name, emptyAttrs, vectorRef->vectorId, vectorRef->columns);
        vectorResult.setAttributes(vectorRef->attributes);
        vectorResult.startEventNum = vectorRef->startEventNum;
        vectorResult.endEventNum = vectorRef->endEventNum;
        vectorResult.startTime = vectorRef->startTime;
        vectorResult.endTime = vectorRef->endTime;
        vectorResult.stat = vectorRef->stat;
        fileRunRef->vectorResults.push_back(vectorResult); //TODO use addVector()
    }
    delete index;
}

}  // namespace scave
}  // namespace omnetpp
