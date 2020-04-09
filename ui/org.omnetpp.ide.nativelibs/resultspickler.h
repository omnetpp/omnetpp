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

    std::pair<ShmSendBuffer*, ShmSendBuffer*> readVectorIntoShm(const ID& id, double simTimeStart = -INFINITY, double simTimeEnd = INFINITY);

    void pickleResultAttrs(Pickler& p, const IDList& resultIDs);

public:
    ResultsPickler(ResultFileManager *rfm, ShmSendBufferManager *shmManager, InterruptedFlag *interrupted=nullptr) : rfm(rfm), shmManager(shmManager), interrupted(interrupted?interrupted:&dummy) {}

    ShmSendBuffer *getCsvResultsPickle(std::string filterExpression, std::vector<std::string> rowTypes, bool omitUnusedColumns, double simTimeStart, double simTimeEnd);

    ShmSendBuffer *getScalarsPickle(const char *filterExpression, bool includeAttrs);
    ShmSendBuffer *getVectorsPickle(const char *filterExpression, bool includeAttrs, double simTimeStart, double simTimeEnd);
    ShmSendBuffer *getStatisticsPickle(const char *filterExpression, bool includeAttrs);
    ShmSendBuffer *getHistogramsPickle(const char *filterExpression, bool includeAttrs);
    ShmSendBuffer *getParamValuesPickle(const char *filterExpression, bool includeAttrs);

    ShmSendBuffer *getRunsPickle(const char *filterExpression);
    ShmSendBuffer *getRunattrsPickle(const char *filterExpression);
    ShmSendBuffer *getItervarsPickle(const char *filterExpression);
    ShmSendBuffer *getConfigEntriesPickle(const char *filterExpression);
    ShmSendBuffer *getParamAssignmentsPickle(const char *filterExpression);

    ShmSendBuffer *getRunattrsForRunsPickle(const std::vector<std::string>& runIds);
    ShmSendBuffer *getItervarsForRunsPickle(const std::vector<std::string>& runIds);
    ShmSendBuffer *getConfigEntriesForRunsPickle(const std::vector<std::string>& runIds);
    ShmSendBuffer *getParamAssignmentsForRunsPickle(const std::vector<std::string>& runIds);
};

} // namespace scave
} // namespace omnetpp

#endif
