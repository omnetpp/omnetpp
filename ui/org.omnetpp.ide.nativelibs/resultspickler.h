//=========================================================================
//  RESULTSPICKLER.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
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

    std::pair<ShmSendBufferPtr, ShmSendBufferPtr> readVectorIntoShm(const ID& id, double simTimeStart = -INFINITY, double simTimeEnd = INFINITY);

    void pickleResultAttrs(Pickler& p, const IDList& resultIDs);

public:
    ResultsPickler(ResultFileManager *rfm, ShmSendBufferManager *shmManager, InterruptedFlag *interrupted=nullptr) : rfm(rfm), shmManager(shmManager), interrupted(interrupted?interrupted:&dummy) {}

    ShmSendBufferPtrVector getCsvResultsPickle(const char *filterExpression, std::vector<std::string> rowTypes, bool omitUnusedColumns, bool includeFieldsAsScalars, double simTimeStart, double simTimeEnd);
    ShmSendBufferPtrVector getCsvResultsPickle(const IDList& results, std::vector<std::string> rowTypes, bool omitUnusedColumns, double simTimeStart, double simTimeEnd);

    ShmSendBufferPtr getScalarsPickle(const char *filterExpression, bool includeAttrs, bool includeFields);
    ShmSendBufferPtrVector getVectorsPickle(const char *filterExpression, bool includeAttrs, double simTimeStart, double simTimeEnd);
    ShmSendBufferPtr getStatisticsPickle(const char *filterExpression, bool includeAttrs);
    ShmSendBufferPtr getHistogramsPickle(const char *filterExpression, bool includeAttrs);
    ShmSendBufferPtr getParamValuesPickle(const char *filterExpression, bool includeAttrs);

    ShmSendBufferPtr getScalarsPickle(const IDList& scalars, bool includeAttrs);
    ShmSendBufferPtrVector getVectorsPickle(const IDList& vectors, bool includeAttrs, double simTimeStart, double simTimeEnd);
    ShmSendBufferPtr getStatisticsPickle(const IDList& statistics, bool includeAttrs);
    ShmSendBufferPtr getHistogramsPickle(const IDList& histograms, bool includeAttrs);
    ShmSendBufferPtr getParamValuesPickle(const IDList& params, bool includeAttrs);


    ShmSendBufferPtr getRunsPickle(const char *filterExpression);
    ShmSendBufferPtr getRunattrsPickle(const char *filterExpression);
    ShmSendBufferPtr getItervarsPickle(const char *filterExpression);
    ShmSendBufferPtr getConfigEntriesPickle(const char *filterExpression);
    ShmSendBufferPtr getParamAssignmentsPickle(const char *filterExpression); //TODO eliminate? same as previous

    ShmSendBufferPtr getRunsPickle(const RunList& runs);
    ShmSendBufferPtr getRunattrsPickle(const RunAndValueList& runAttrs);
    ShmSendBufferPtr getItervarsPickle(const RunAndValueList& itervars);
    ShmSendBufferPtr getConfigEntriesPickle(const RunAndValueList& configEntries);
    ShmSendBufferPtr getParamAssignmentsPickle(const RunAndValueList& paramAssignments); //TODO eliminate? same as previous

    ShmSendBufferPtr getRunattrsForRunsPickle(const std::vector<std::string>& runIds);
    ShmSendBufferPtr getItervarsForRunsPickle(const std::vector<std::string>& runIds);
    ShmSendBufferPtr getConfigEntriesForRunsPickle(const std::vector<std::string>& runIds);
    ShmSendBufferPtr getParamAssignmentsForRunsPickle(const std::vector<std::string>& runIds);
};

}  // namespace scave
}  // namespace omnetpp

#endif
