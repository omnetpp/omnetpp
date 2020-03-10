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

/**
 * This class can be used to pickle filtered simulation results in various formats from the given ResultFileManager.
 * The return value of each member function is a space-separated pair of the name and the size of a newly
 * allocated SHM object. The caller is responsible for removing the SHM object once no longer needed.
 */
class ResultsPickler
{
protected:
    scave::ResultFileManager *rfm;

    size_t getSizeLimit();

    std::pair<std::string, std::string> readVectorIntoShm(const scave::ID& id, double simTimeStart = -INFINITY, double simTimeEnd = INFINITY, const scave::InterruptedFlag& interrupted = scave::InterruptedFlag());

    void pickleResultAttrs(Pickler& p, const scave::IDList& resultIDs, const scave::InterruptedFlag& interrupted = scave::InterruptedFlag());

public:

    ResultsPickler(scave::ResultFileManager *rfm) : rfm(rfm) { }

    std::string getCsvResultsPickle(std::string filterExpression, std::vector<std::string> rowTypes, bool omitUnusedColumns, double simTimeStart, double simTimeEnd, const scave::InterruptedFlag& interrupted = scave::InterruptedFlag());

    std::string getScalarsPickle(const char *filterExpression, bool includeAttrs, const scave::InterruptedFlag& interrupted = scave::InterruptedFlag());
    std::string getVectorsPickle(const char *filterExpression, bool includeAttrs, double simTimeStart, double simTimeEnd, const scave::InterruptedFlag& interrupted = scave::InterruptedFlag());
    std::string getStatisticsPickle(const char *filterExpression, bool includeAttrs, const scave::InterruptedFlag& interrupted = scave::InterruptedFlag());
    std::string getHistogramsPickle(const char *filterExpression, bool includeAttrs, const scave::InterruptedFlag& interrupted = scave::InterruptedFlag());
    std::string getParamValuesPickle(const char *filterExpression, bool includeAttrs, const scave::InterruptedFlag& interrupted = scave::InterruptedFlag());

    std::string getRunsPickle(const char *filterExpression, const scave::InterruptedFlag& interrupted = scave::InterruptedFlag());
    std::string getRunattrsPickle(const char *filterExpression, const scave::InterruptedFlag& interrupted = scave::InterruptedFlag());
    std::string getItervarsPickle(const char *filterExpression, const scave::InterruptedFlag& interrupted = scave::InterruptedFlag());
    std::string getConfigEntriesPickle(const char *filterExpression, const scave::InterruptedFlag& interrupted = scave::InterruptedFlag());
    std::string getParamAssignmentsPickle(const char *filterExpression, const scave::InterruptedFlag& interrupted = scave::InterruptedFlag());

    std::string getRunattrsForRunsPickle(const std::vector<std::string>& runIds, const scave::InterruptedFlag& interrupted = scave::InterruptedFlag());
    std::string getItervarsForRunsPickle(const std::vector<std::string>& runIds, const scave::InterruptedFlag& interrupted = scave::InterruptedFlag());
    std::string getConfigEntriesForRunsPickle(const std::vector<std::string>& runIds, const scave::InterruptedFlag& interrupted = scave::InterruptedFlag());
    std::string getParamAssignmentsForRunsPickle(const std::vector<std::string>& runIds, const scave::InterruptedFlag& interrupted = scave::InterruptedFlag());
};

} // namespace omnetpp

#endif
