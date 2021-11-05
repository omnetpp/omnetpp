//=========================================================================
//  RESULTSPICKLER.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Attila Torok
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2020 Andras Varga
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_RESULTSPICKLER_H
#define __OMNETPP_RESULTSPICKLER_H

#include "scave/resultfilemanager.h"
#include "scave/interruptedflag.h"
#include "pickler.h"

namespace omnetpp {
namespace scave {

class ShmSendBuffer;
class ShmSendBufferManager;

/**
 * This class can be used to pickle filtered simulation results in various formats from the given ResultFileManager.
 * The return value of each member function is a space-separated pair of the name and the size of a newly
 * allocated SHM object. The caller is responsible for removing the SHM object once no longer needed.
 */
class ResultsPickler
{
protected:
    ResultFileManager *rfm;
    ShmSendBufferManager *shmManager;
    InterruptedFlag *interrupted;
    InterruptedFlag dummy;
    size_t reserveSize = (1ull<<31)-1-8;

    size_t getSizeLimit(); // available space, based on free physical memory

    std::pair<std::shared_ptr<ShmSendBuffer>, std::shared_ptr<ShmSendBuffer>> readVectorIntoShm(const ID& id, double simTimeStart = -INFINITY, double simTimeEnd = INFINITY);

    void pickleResultAttrs(Pickler& p, const IDList& resultIDs);

public:
    ResultsPickler(ResultFileManager *rfm, ShmSendBufferManager *shmManager, InterruptedFlag *interrupted=nullptr) : rfm(rfm), shmManager(shmManager), interrupted(interrupted?interrupted:&dummy) {}

    std::vector<std::shared_ptr<ShmSendBuffer>> getCsvResultsPickle(const char *filterExpression, std::vector<std::string> rowTypes, bool omitUnusedColumns, bool includeFieldsAsScalars, double simTimeStart, double simTimeEnd);
    std::vector<std::shared_ptr<ShmSendBuffer>> getCsvResultsPickle(const IDList& results, std::vector<std::string> rowTypes, bool omitUnusedColumns, double simTimeStart, double simTimeEnd);

    std::shared_ptr<ShmSendBuffer> getScalarsPickle(const char *filterExpression, bool includeAttrs, bool includeFields);
    std::vector<std::shared_ptr<ShmSendBuffer>> getVectorsPickle(const char *filterExpression, bool includeAttrs, double simTimeStart, double simTimeEnd);
    std::shared_ptr<ShmSendBuffer> getStatisticsPickle(const char *filterExpression, bool includeAttrs);
    std::shared_ptr<ShmSendBuffer> getHistogramsPickle(const char *filterExpression, bool includeAttrs);
    std::shared_ptr<ShmSendBuffer> getParamValuesPickle(const char *filterExpression, bool includeAttrs);

    std::shared_ptr<ShmSendBuffer> getScalarsPickle(const IDList& scalars, bool includeAttrs);
    std::vector<std::shared_ptr<ShmSendBuffer>> getVectorsPickle(const IDList& vectors, bool includeAttrs, double simTimeStart, double simTimeEnd);
    std::shared_ptr<ShmSendBuffer> getStatisticsPickle(const IDList& statistics, bool includeAttrs);
    std::shared_ptr<ShmSendBuffer> getHistogramsPickle(const IDList& histograms, bool includeAttrs);
    std::shared_ptr<ShmSendBuffer> getParamValuesPickle(const IDList& params, bool includeAttrs);


    std::shared_ptr<ShmSendBuffer> getRunsPickle(const char *filterExpression);
    std::shared_ptr<ShmSendBuffer> getRunattrsPickle(const char *filterExpression);
    std::shared_ptr<ShmSendBuffer> getItervarsPickle(const char *filterExpression);
    std::shared_ptr<ShmSendBuffer> getConfigEntriesPickle(const char *filterExpression);
    std::shared_ptr<ShmSendBuffer> getParamAssignmentsPickle(const char *filterExpression); //TODO eliminate? same as previous

    std::shared_ptr<ShmSendBuffer> getRunsPickle(const RunList& runs);
    std::shared_ptr<ShmSendBuffer> getRunattrsPickle(const RunAndValueList& runAttrs);
    std::shared_ptr<ShmSendBuffer> getItervarsPickle(const RunAndValueList& itervars);
    std::shared_ptr<ShmSendBuffer> getConfigEntriesPickle(const RunAndValueList& configEntries);
    std::shared_ptr<ShmSendBuffer> getParamAssignmentsPickle(const RunAndValueList& paramAssignments); //TODO eliminate? same as previous

    std::shared_ptr<ShmSendBuffer> getRunattrsForRunsPickle(const std::vector<std::string>& runIds);
    std::shared_ptr<ShmSendBuffer> getItervarsForRunsPickle(const std::vector<std::string>& runIds);
    std::shared_ptr<ShmSendBuffer> getConfigEntriesForRunsPickle(const std::vector<std::string>& runIds);
    std::shared_ptr<ShmSendBuffer> getParamAssignmentsForRunsPickle(const std::vector<std::string>& runIds);
};

}  // namespace scave
}  // namespace omnetpp

#endif
