//=========================================================================
//  RESULTSPICKLER.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2020 Andras Varga
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "resultspickler.h"

#include "common/stringutil.h"
#include "common/stlutil.h"
#include "sharedmemory.h"
#include "scave/xyarray.h"
#include "scave/vectorutils.h"
#include "scave/memoryutils.h"

namespace omnetpp {

using namespace common;
using namespace scave;


std::pair<std::string, std::string> ResultsPickler::readVectorIntoShm(const ID& id, double simTimeStart, double simTimeEnd, const InterruptedFlag& interrupted)
{
    size_t memoryLimitBytes = getSizeLimit();

    static int counter = 0;
    counter++;

    IDList vectorList(id);

    std::vector<XYArray *> vectorData = readVectorsIntoArrays(rfm, vectorList, false, false, memoryLimitBytes, simTimeStart, simTimeEnd, interrupted);
    // ASSERT(vectorData.size() == 1);
    auto array = std::unique_ptr<XYArray>(vectorData[0]);

    std::string nameBase = "/vectordata_" + std::to_string(counter);
    std::string nameX = nameBase + "_x";
    std::string nameY = nameBase + "_y";
    size_t size = array->length() * 8;

    createSharedMemory(nameX.c_str(), size);
    createSharedMemory(nameY.c_str(), size);

    void *shmX = mapSharedMemory(nameX.c_str(), size);
    void *shmY = mapSharedMemory(nameY.c_str(), size);

    memcpy(shmX, array->xs.data(), size);
    memcpy(shmY, array->ys.data(), size);

    unmapSharedMemory(shmX, size);
    unmapSharedMemory(shmY, size);

    array.release();
    malloc_trim(); // so the std::vector buffers (in array) are released to the operating system

    return {nameX + " " + std::to_string(size), nameY + " " + std::to_string(size)};
}

size_t ResultsPickler::getSizeLimit()
{
    // we'll need about 1-2x the size of the pickle to unpack it in addition...
    return getAvailableMemoryBytes() / 3;
}

void ResultsPickler::pickleResultAttrs(Pickler& p, const IDList& resultIDs, const InterruptedFlag& interrupted)
{
    p.startList();

    for (int i = 0; i < resultIDs.size(); ++i) {
        const ResultItem &result = rfm->getItem(resultIDs.get(i));
        const StringMap &attrs = result.getAttributes();
        for (const auto & a : attrs) {
            p.startTuple();

            p.pushString(result.getRun()->getRunName());
            p.pushString(result.getModuleName());
            p.pushString(result.getName());
            p.pushString(a.first); // attrname
            p.pushString(a.second); // attrvalue

            p.endTuple();
        }

        if ((i & 0xFF) == 0 && interrupted.flag)
            throw std::runtime_error("Result attribute pickling interrupted");
    }

    p.endList();
}


std::string ResultsPickler::getCsvResultsPickle(std::string filterExpression, std::vector<std::string> rowTypes, bool omitUnusedColumns, double simTimeStart, double simTimeEnd, const InterruptedFlag& interrupted)
{
    bool addRunAttrs, addIterVars, addConfigEntries, addScalars, addVectors, addStatistics, addHistograms, addParams, addAttrs;

    addRunAttrs = contains(rowTypes, std::string("runattr"));
    addIterVars = contains(rowTypes, std::string("itervar"));
    addConfigEntries = contains(rowTypes, std::string("config"));

    addScalars = contains(rowTypes, std::string("scalar"));
    addVectors = contains(rowTypes, std::string("vector"));
    addStatistics = contains(rowTypes, std::string("statistic"));
    addHistograms = contains(rowTypes, std::string("histogram"));
    addParams = contains(rowTypes, std::string("param"));

    addAttrs = contains(rowTypes, std::string("attr"));

    int resultTypesToAdd =
        (addScalars    ? ResultFileManager::SCALAR     : 0) |
        (addVectors    ? ResultFileManager::VECTOR     : 0) |
        (addStatistics ? ResultFileManager::STATISTICS : 0) |
        (addHistograms ? ResultFileManager::HISTOGRAM  : 0) |
        (addParams     ? ResultFileManager::PARAMETER  : 0);

    ShmPickler p("results", 1024*1024*1000);

    p.protocol();

    // the columns are always:
    // runID, type, module, name, attrname, attrvalue, value, count, sumweights, mean, stddev, min, max, underflows, overflows, binedges, binvalues, vectime, vecvalue

    // not all tuples in the list will have all the elements though. we still have to pad them "in the middle",
    // so all data falls into the right column, but the rest will be filled in by numpy after unpickling
    p.startList();

    if (!opp_trim(filterExpression).empty()) {
        IDList results = rfm->getAllItems(false);
        results = rfm->filterIDList(results, filterExpression.c_str());

        // pickle runs of results

        RunList runs = rfm->getUniqueRuns(results);

        // is the order of this right, or should we sort the rows by type?
        for (Run *r : runs) {
            if (addRunAttrs) {
                // Run attributes
                const StringMap& attrs = r->getAttributes();

                for (auto a : attrs) {
                    p.startTuple();

                    p.pushString(r->getRunName());
                    p.pushString("runattr");
                    p.pushNone(); // module
                    p.pushNone(); // name
                    p.pushString(a.first); // attrname
                    p.pushString(a.second); // attrvalue

                    p.endTuple();
                }
            }

            if (addIterVars) {
                // Iteration variables
                const StringMap &itervars = r->getIterationVariables();

                for (auto iv : itervars) {
                    p.startTuple();

                    p.pushString(r->getRunName());
                    p.pushString("itervar");
                    p.pushNone(); // module
                    p.pushNone(); // name
                    p.pushString(iv.first); // attrname
                    p.pushString(iv.second); // attrvalue

                    //for (int j = 0; j < 13; ++j)
                    //    p.pushNone();

                    p.endTuple();
                }
            }

            if (addConfigEntries) {
                // Config entries
                const OrderedKeyValueList &entries = r->getConfigEntries();
                for (auto e : entries) {

                    p.startTuple();

                    p.pushString(r->getRunName());
                    p.pushString("config");
                    p.pushNone(); // module
                    p.pushNone(); // name
                    p.pushString(e.first); // attrname
                    p.pushString(e.second); // attrvalue

                    //for (int j = 0; j < 13; ++j)
                    //    p.pushNone();

                    p.endTuple();
                }
            }

            if (interrupted.flag)
                throw std::runtime_error("Result pickling interrupted");
        }

        // end of run data pickling

        for (const ID& id : results) {
            const ResultItem &result = rfm->getItem(id);
            auto type = result.getItemType();

            if (resultTypesToAdd & type) {

                p.startTuple();

                p.pushString(result.getRun()->getRunName());
                switch (type) {
                    case ResultFileManager::PARAMETER:  p.pushString("param");     break; // parameter?
                    case ResultFileManager::SCALAR:     p.pushString("scalar");    break;
                    case ResultFileManager::VECTOR:     p.pushString("vector");    break;
                    case ResultFileManager::STATISTICS: p.pushString("statistic"); break; // statistics?
                    case ResultFileManager::HISTOGRAM:  p.pushString("histogram"); break;
                }

                p.pushString(result.getModuleName());
                p.pushString(result.getName());
                p.pushNone(); // attrname
                p.pushNone(); // attrvalue

                switch (type) {
                    case ResultFileManager::PARAMETER: {
                        const ParameterResult& parameter = rfm->getParameter(id);
                        p.pushString(parameter.getValue());
                        break;
                    }
                    case ResultFileManager::SCALAR: {
                        const ScalarResult& scalar = rfm->getScalar(id);
                        p.pushDouble(scalar.getValue());
                        break;
                    }
                    case ResultFileManager::VECTOR: {
                        // value, count, sumweights, mean, stddev, min, max, underflows, overflows, binedges, binvalues,
                        for (int j = 0; j < 11; ++j)
                            p.pushNone();

                        auto shmNames = readVectorIntoShm(id, simTimeStart, simTimeEnd, interrupted);

                        p.pushString(shmNames.first); // vectime
                        p.pushString(shmNames.second); // vecvalue

                        break;
                    }
                    case ResultFileManager::STATISTICS: {
                        const Statistics& statistics = rfm->getStatistics(id).getStatistics();

                        p.pushNone(); // value column (for scalars/parameters)

                        p.pushDouble(statistics.getCount());
                        p.pushDouble(statistics.getSumWeights());
                        p.pushDouble(statistics.getMean());
                        p.pushDouble(statistics.getStddev());
                        p.pushDouble(statistics.getMin());
                        p.pushDouble(statistics.getMax());

                        break;
                    }
                    case ResultFileManager::HISTOGRAM: {
                        const HistogramResult& histogramResult = rfm->getHistogram(id);

                        const Statistics& statistics = histogramResult.getStatistics();
                        const Histogram& histogram = histogramResult.getHistogram();

                        p.pushNone(); // value column (for scalars/parameters)

                        p.pushDouble(statistics.getCount());
                        p.pushDouble(statistics.getSumWeights());
                        p.pushDouble(statistics.getMean());
                        p.pushDouble(statistics.getStddev());
                        p.pushDouble(statistics.getMin());
                        p.pushDouble(statistics.getMax());

                        p.pushDouble(histogram.getUnderflows());
                        p.pushDouble(histogram.getOverflows());

                        p.pushDoublesAsRawBytes(histogram.getBinEdges());
                        p.pushDoublesAsRawBytes(histogram.getBinValues());

                        break;
                    }
                }

                p.endTuple();
            }


            if (addAttrs) {
                const StringMap &attrs = result.getAttributes();

                for (const auto& a : attrs) {
                    p.startTuple();

                    p.pushString(result.getRun()->getRunName());
                    p.pushString("attr");
                    p.pushString(result.getModuleName());
                    p.pushString(result.getName());
                    p.pushString(a.first); // attrname
                    p.pushString(a.second); // attrvalue

                    p.endTuple();
                }
            }

            if (interrupted.flag)
                throw std::runtime_error("Result pickling interrupted");
        }
    }

    p.endList();

    p.stop();

    return p.getShmNameAndSize();
}


std::string ResultsPickler::getScalarsPickle(const char *filterExpression, bool includeAttrs, const InterruptedFlag& interrupted)
{
    size_t sizeLimit = getSizeLimit();
    ShmPickler p("scalars", sizeLimit);

    p.protocol();

    p.startList();

    IDList scalars;
    if (!opp_isempty(filterExpression)) {
        IDList allScalars = rfm->getAllScalars(false);
        scalars = rfm->filterIDList(allScalars, filterExpression, -1, interrupted);

        for (int i = 0; i < scalars.size(); ++i) {
            const ScalarResult &result = rfm->getScalar(scalars.get(i));
            p.startTuple();

            p.pushString(result.getRun()->getRunName());
            p.pushString(result.getModuleName());
            p.pushString(result.getName());
            p.pushDouble(result.getValue());

            p.endTuple();

            if ((i & 0xFF) == 0 && interrupted.flag)
                throw std::runtime_error("Result pickling interrupted");
        }
    }

    p.endList();

    if (!scalars.isEmpty() && includeAttrs)
        pickleResultAttrs(p, scalars, interrupted);
    else
        p.push<PickleOpCode>(PickleOpCode::NONE);


    p.push<PickleOpCode>(PickleOpCode::TUPLE2);

    p.stop();

    return p.getShmNameAndSize();
}


std::string ResultsPickler::getVectorsPickle(const char *filterExpression, bool includeAttrs, double simTimeStart, double simTimeEnd, const InterruptedFlag& interrupted)
{
    size_t sizeLimit = getSizeLimit();
    ShmPickler p("vectors", sizeLimit);

    p.protocol();

    p.startList();

    IDList vectors;
    if (!opp_isempty(filterExpression)) {
        IDList allVectors = rfm->getAllVectors();
        vectors = rfm->filterIDList(allVectors, filterExpression, -1, interrupted);

        for (int i = 0; i < vectors.size(); ++i) {
            const VectorResult &result = rfm->getVector(vectors.get(i));
            p.startTuple();

            p.pushString(result.getRun()->getRunName());
            p.pushString(result.getModuleName());
            p.pushString(result.getName());

            auto shmNames = readVectorIntoShm(vectors.get(i), simTimeStart, simTimeEnd, interrupted);

            p.pushString(shmNames.first); // vectime
            p.pushString(shmNames.second); // vecvalue

            p.endTuple();

            if ((i & 0xFF) == 0 && interrupted.flag)
                throw std::runtime_error("Result pickling interrupted");
        }
    }

    p.endList();

    if (!vectors.isEmpty() && includeAttrs)
        pickleResultAttrs(p, vectors, interrupted);
    else
        p.push<PickleOpCode>(PickleOpCode::NONE);


    p.push<PickleOpCode>(PickleOpCode::TUPLE2);

    p.stop();

    return p.getShmNameAndSize();
}


std::string ResultsPickler::getParamValuesPickle(const char *filterExpression, bool includeAttrs, const InterruptedFlag& interrupted)
{
    size_t sizeLimit = getSizeLimit();
    ShmPickler p("paramvalues", sizeLimit);

    p.protocol();

    p.startList();

    IDList params;
    if (!opp_isempty(filterExpression)) {
        IDList allParams = rfm->getAllParameters();
        params = rfm->filterIDList(allParams, filterExpression, -1, interrupted);

        for (int i = 0; i < params.size(); ++i) {
            const ParameterResult &result = rfm->getParameter(params.get(i));
            p.startTuple();

            p.pushString(result.getRun()->getRunName());
            p.pushString(result.getModuleName());
            p.pushString(result.getName());
            p.pushString(result.getValue());

            p.endTuple();

            if ((i & 0xFF) == 0 && interrupted.flag)
                throw std::runtime_error("Result pickling interrupted");
        }
    }

    p.endList();

    if (!params.isEmpty() && includeAttrs)
        pickleResultAttrs(p, params, interrupted);
    else
        p.push<PickleOpCode>(PickleOpCode::NONE);


    p.push<PickleOpCode>(PickleOpCode::TUPLE2);

    p.stop();

    return p.getShmNameAndSize();
}


std::string ResultsPickler::getStatisticsPickle(const char *filterExpression, bool includeAttrs, const InterruptedFlag& interrupted)
{
    size_t sizeLimit = getSizeLimit();

    ShmPickler p("statistics", sizeLimit);

    p.protocol();

    p.startList();

    IDList statistics;
    if (!opp_isempty(filterExpression)) {
        IDList allStatistics = rfm->getAllStatistics();
        statistics = rfm->filterIDList(allStatistics, filterExpression, -1, interrupted);

        for (int i = 0; i < statistics.size(); ++i) {
            const StatisticsResult &result = rfm->getStatistics(statistics.get(i));

            const Statistics& statistics = result.getStatistics();

            p.startTuple();

            p.pushString(result.getRun()->getRunName());
            p.pushString(result.getModuleName());
            p.pushString(result.getName());

            p.pushDouble(statistics.getCount());
            p.pushDouble(statistics.getSumWeights());
            p.pushDouble(statistics.getMean());
            p.pushDouble(statistics.getStddev());
            p.pushDouble(statistics.getMin());
            p.pushDouble(statistics.getMax());

            p.endTuple();

            if ((i & 0xFF) == 0 && interrupted.flag)
                throw std::runtime_error("Result pickling interrupted");
        }
    }

    p.endList();

    if (!statistics.isEmpty() && includeAttrs)
        pickleResultAttrs(p, statistics, interrupted);
    else
        p.push<PickleOpCode>(PickleOpCode::NONE);


    p.push<PickleOpCode>(PickleOpCode::TUPLE2);

    p.stop();

    return p.getShmNameAndSize();
}


std::string ResultsPickler::getHistogramsPickle(const char *filterExpression, bool includeAttrs, const InterruptedFlag& interrupted)
{
    size_t sizeLimit = getSizeLimit();
    ShmPickler p("histograms", sizeLimit);

    p.protocol();

    p.startList();

    IDList histograms;
    if (!opp_isempty(filterExpression)) {
        IDList allHistograms = rfm->getAllHistograms();
        histograms = rfm->filterIDList(allHistograms, filterExpression, -1, interrupted);

        for (int i = 0; i < histograms.size(); ++i) {
            const HistogramResult &result = rfm->getHistogram(histograms.get(i));

            const Statistics& statistics = result.getStatistics();
            const Histogram& histogram = result.getHistogram();

            p.startTuple();

            p.pushString(result.getRun()->getRunName());
            p.pushString(result.getModuleName());
            p.pushString(result.getName());

            p.pushDouble(statistics.getCount());
            p.pushDouble(statistics.getSumWeights());
            p.pushDouble(statistics.getMean());
            p.pushDouble(statistics.getStddev());
            p.pushDouble(statistics.getMin());
            p.pushDouble(statistics.getMax());

            p.pushDouble(histogram.getUnderflows());
            p.pushDouble(histogram.getOverflows());

            p.pushDoublesAsRawBytes(histogram.getBinEdges());
            p.pushDoublesAsRawBytes(histogram.getBinValues());

            p.endTuple();

            if ((i & 0xFF) == 0 && interrupted.flag)
                throw std::runtime_error("Result pickling interrupted");
        }
    }

    p.endList();

    if (!histograms.isEmpty() && includeAttrs)
        pickleResultAttrs(p, histograms, interrupted);
    else
        p.push<PickleOpCode>(PickleOpCode::NONE);


    p.push<PickleOpCode>(PickleOpCode::TUPLE2);

    p.stop();

    return p.getShmNameAndSize();
}


std::string ResultsPickler::getRunsPickle(const char *filterExpression, const InterruptedFlag& interrupted)
{
    size_t sizeLimit = getSizeLimit();
    ShmPickler p("runs", sizeLimit);

    p.protocol();

    p.startList();

    if (!opp_isempty(filterExpression)) {
        RunList runs = rfm->getRuns();
        runs = rfm->filterRunList(runs, filterExpression);

        for (const auto &r : runs)
            p.pushString(r->getRunName());
    }

    p.endList();

    p.stop();

    return p.getShmNameAndSize();
}


std::string ResultsPickler::getRunattrsPickle(const char *filterExpression, const InterruptedFlag& interrupted)
{
    size_t sizeLimit = getSizeLimit();
    ShmPickler p("runattrs", sizeLimit);

    p.protocol();

    p.startList();

    if (!opp_isempty(filterExpression)) {
        auto runattrs = rfm->getMatchingRunattrsPtr(filterExpression);

        for (const auto &ra : runattrs) {
            Run *run = ra.first;
            const std::string &raName = ra.second;

            p.startTuple();

            p.pushString(run->getRunName());
            p.pushString(raName);
            p.pushString(run->getAttribute(raName));

            p.endTuple();
        }
    }

    p.endList();

    p.stop();

    return p.getShmNameAndSize();
}


std::string ResultsPickler::getItervarsPickle(const char *filterExpression, const InterruptedFlag& interrupted)
{
    size_t sizeLimit = getSizeLimit();
    ShmPickler p("itervars", sizeLimit);

    p.protocol();

    p.startList();

    if (!opp_isempty(filterExpression)) {
        auto itervars = rfm->getMatchingItervarsPtr(filterExpression);

        for (const auto &iv : itervars) {
            Run *run = iv.first;
            const std::string &ivName = iv.second;

            p.startTuple();

            p.pushString(run->getRunName());
            p.pushString(ivName);
            p.pushString(run->getIterationVariable(ivName));

            p.endTuple();
        }
    }

    p.endList();

    p.stop();

    return p.getShmNameAndSize();
}


std::string ResultsPickler::getConfigEntriesPickle(const char *filterExpression, const InterruptedFlag& interrupted)
{
    size_t sizeLimit = getSizeLimit();
    ShmPickler p("configentries", sizeLimit);

    p.protocol();

    p.startList();

    if (!opp_isempty(filterExpression)) {
        auto configentries = rfm->getMatchingConfigEntriesPtr(filterExpression);

        for (const auto &ce : configentries) {
            Run *run = ce.first;
            const std::string &ceName = ce.second;

            p.startTuple();

            p.pushString(run->getRunName());
            p.pushString(ceName);
            p.pushString(run->getConfigValue(ceName));

            p.endTuple();
        }
    }

    p.endList();

    p.stop();

    return p.getShmNameAndSize();
}


std::string ResultsPickler::getParamAssignmentsPickle(const char *filterExpression, const InterruptedFlag& interrupted)
{
    size_t sizeLimit = getSizeLimit();
    ShmPickler p("paramassignments", sizeLimit);

    p.protocol();

    p.startList();

    if (!opp_isempty(filterExpression)) {
        auto paramassignments = rfm->getMatchingParamAssignments(filterExpression); // TODO why is there no Ptr equivalent to this?

        for (const auto &pa : paramassignments) {
            Run *run = rfm->getRunByName(pa.first.c_str());
            const std::string &paName = pa.second;

            p.startTuple();

            p.pushString(run->getRunName());
            p.pushString(paName);
            p.pushString(run->getParamAssignment(paName));

            p.endTuple();
        }
    }

    p.endList();

    p.stop();

    return p.getShmNameAndSize();
}


std::string ResultsPickler::getRunattrsForRunsPickle(const std::vector<std::string>& runIds, const InterruptedFlag& interrupted)
{
    size_t sizeLimit = getSizeLimit();
    ShmPickler p("runattrs_for_runs", sizeLimit);

    p.protocol();

    p.startList();

    for (const std::string &runId : runIds) {
        Run *run = rfm->getRunByName(runId.c_str());
        const StringMap& runattrs = run->getAttributes();

        for (const auto &ra : runattrs) {
            p.startTuple();

            p.pushString(run->getRunName());
            p.pushString(ra.first);
            p.pushString(ra.second);

            p.endTuple();
        }
    }

    p.endList();

    p.stop();

    return p.getShmNameAndSize();
}


std::string ResultsPickler::getItervarsForRunsPickle(const std::vector<std::string>& runIds, const InterruptedFlag& interrupted)
{
    size_t sizeLimit = getSizeLimit();
    ShmPickler p("runattrs_for_runs", sizeLimit);

    p.protocol();

    p.startList();

    for (const std::string &runId : runIds) {
        Run *run = rfm->getRunByName(runId.c_str());
        const StringMap& itervars = run->getIterationVariables();

        for (const auto &iv : itervars) {
            p.startTuple();

            p.pushString(run->getRunName());
            p.pushString(iv.first);
            p.pushString(iv.second);

            p.endTuple();
        }
    }

    p.endList();

    p.stop();

    return p.getShmNameAndSize();
}


std::string ResultsPickler::getConfigEntriesForRunsPickle(const std::vector<std::string>& runIds, const InterruptedFlag& interrupted)
{
    size_t sizeLimit = getSizeLimit();
    ShmPickler p("configentries_for_runs", sizeLimit);

    p.protocol();

    p.startList();

    for (const std::string &runId : runIds) {
        Run *run = rfm->getRunByName(runId.c_str());
        const OrderedKeyValueList& configentries = run->getConfigEntries();

        for (const auto &ce : configentries) {
            p.startTuple();

            p.pushString(run->getRunName());
            p.pushString(ce.first);
            p.pushString(ce.second);

            p.endTuple();
        }
    }

    p.endList();

    p.stop();

    return p.getShmNameAndSize();
}


std::string ResultsPickler::getParamAssignmentsForRunsPickle(const std::vector<std::string>& runIds, const InterruptedFlag& interrupted)
{
    size_t sizeLimit = getSizeLimit();
    ShmPickler p("paramassignments_for_runs", sizeLimit);

    p.protocol();

    p.startList();

    for (const std::string &runId : runIds) {
        Run *run = rfm->getRunByName(runId.c_str());
        const OrderedKeyValueList& paramassignments = run->getParamAssignments();

        for (const auto &pa : paramassignments) {
            p.startTuple();

            p.pushString(run->getRunName());
            p.pushString(pa.first);
            p.pushString(pa.second);

            p.endTuple();
        }
    }

    p.endList();

    p.stop();

    return p.getShmNameAndSize();
}


} // namespace omnetpp
