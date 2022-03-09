//=========================================================================
//  VECTORUTILS.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "vectorutils.h"

#include <set>
#include "common/opp_ctype.h"
#include "common/commonutil.h"
#include "common/stringutil.h"
#include "common/stlutil.h"
#include "scaveutils.h"
#include "memoryutils.h"
#include "xyarray.h"
#include "resultfilemanager.h"
#include "indexedvectorfilereader.h"
#include "sqliteresultfileutils.h"
#include "sqlitevectordatareader.h"
#include "interruptedflag.h"

using namespace std;

namespace omnetpp {
using namespace common;
namespace scave {

vector<XYArray *> readVectorsIntoArrays(ResultFileManager *manager, const IDList& idlist, bool includePreciseX, bool includeEventNumbers, size_t memoryLimitBytes, double simTimeStart, double simTimeEnd, InterruptedFlag *interrupted)
{
    std::vector<XYArray *> result;
    result.resize(idlist.size());

    for (int i = 0; i < result.size(); ++i) {
        result[i] = new XYArray();
        // TODO: reserve vectors, only those that are needed, taking time limit into account
    }

    size_t memoryUsedBytes = 0;

    ResultFileList filteredVectorFileList = manager->getUniqueFiles(idlist);

    StringMap attrs;
    for (ResultFile *resultFile : filteredVectorFileList) {
        std::vector<int> vectorIds; // local to each file
        RunList runs = manager->getRunsInFile(resultFile);

        if (runs.size() > 1)
            throw opp_runtime_error("More than one run in vector file.");

        assert(runs.size() == 1);

        IDList idsInFile = manager->filterIDList(idlist, runs[0], nullptr, nullptr);

        std::set<int> vectorIdsInFile;
        std::map<int, int> vectorIdToIndex;

        for (ID id : idsInFile) {
            int vectorID = manager->getVector(id)->getVectorId();
            vectorIdsInFile.insert(vectorID);
            vectorIdToIndex[vectorID] = idlist.indexOf(id);
        }

        const int elementSize = sizeof(double) + sizeof(double) + (includePreciseX ? sizeof(BigDecimal) : 0) + (includeEventNumbers ? sizeof(eventnumber_t) : 0);

        auto adapter = [&](int vectorId, const std::vector<VectorDatum>& data) {
            memoryUsedBytes += data.size() * elementSize;
            if (memoryUsedBytes > memoryLimitBytes)
                throw opp_runtime_error("Memory limit exceeded during vector data loading");

            XYArray *array = result[vectorIdToIndex.at(vectorId)];
            for (const VectorDatum &vd : data) {
                array->xs.push_back(vd.simtime.dbl());
                array->ys.push_back(vd.value);
                if (includePreciseX)
                    array->xps.push_back(vd.simtime);
                if (includeEventNumbers)
                    array->ens.push_back(vd.eventNumber);
            }

            if (interrupted != nullptr && interrupted->flag)
                throw InterruptedException("Vector loading interrupted");
        };

        IVectorDataReader *reader;
        if (SqliteResultFileUtils::isSqliteFile(resultFile->getFileSystemFilePath().c_str()))
            reader = new SqliteVectorDataReader(resultFile->getFileSystemFilePath().c_str(), includeEventNumbers, adapter, resultFile->getFingerprint());
        else
            reader = new IndexedVectorFileReader(resultFile->getFileSystemFilePath().c_str(), includeEventNumbers, adapter, resultFile->getFingerprint());

        try {
            if (simTimeStart == -INFINITY && simTimeEnd == INFINITY)
                reader->collectEntries(vectorIdsInFile);
            else
                reader->collectEntriesInSimtimeInterval(vectorIdsInFile, simTimeStart, simTimeEnd);
            delete reader;
        }
        catch (std::exception &e) {
            delete reader;

            for (XYArray *a : result)
                delete a;
            result.clear();
            result.shrink_to_fit();
            malloc_trim(); // TODO needed? effective?

            throw;
        }
    }

    return result;
}

XYArrayVector *readVectorsIntoArrays2(ResultFileManager *manager, const IDList& idlist, bool includePreciseX, bool includeEventNumbers, size_t memoryLimitBytes, double simTimeStart, double simTimeEnd, InterruptedFlag *interrupted) {
    return new XYArrayVector(readVectorsIntoArrays(manager, idlist, includePreciseX, includeEventNumbers, memoryLimitBytes, simTimeStart, simTimeEnd, interrupted));
}

}  // namespace scave
}  // namespace omnetpp
