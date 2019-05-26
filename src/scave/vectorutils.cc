//=========================================================================
//  VECTORUTILS.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga
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
#include "xyarray.h"
#include "resultfilemanager.h"
#include "indexedvectorfilereader.h"

using namespace std;

namespace omnetpp {
using namespace common;
namespace scave {

XYArray *convertVectorData(const std::vector<VectorDatum>& data, bool includeEventNumbers)
{
    int l = data.size();

    double* xv = new double[l];
    double* yv = new double[l];
    BigDecimal* xpv = new BigDecimal[l];
    eventnumber_t* ev = includeEventNumbers ? new eventnumber_t[l] : nullptr;

    for (int i = 0; i < data.size(); ++i) {
        const VectorDatum &vd = data[i];
        xv[i] = vd.simtime.dbl();
        yv[i] = vd.value;
        xpv[i] = vd.simtime;
        if (includeEventNumbers)
            ev[i] = vd.eventNumber;
    }

    return new XYArray(l, xv, yv, xpv, ev);
}

vector<XYArray *> readVectorsIntoArrays(ResultFileManager *manager, const IDList& idlist, bool includeEventNumbers, const InterruptedFlag& interrupted)
{
    std::vector<std::vector<VectorDatum>> result;
    result.resize(idlist.size());

    ResultFileList *filteredVectorFileList = manager->getUniqueFiles(idlist);

    StringMap attrs;
    for (int i = 0; i < (int)filteredVectorFileList->size(); i++) {
        ResultFile *resultFile = filteredVectorFileList->at(i);

        std::vector<int> vectorIds; // local to each file
        RunList runs = manager->getRunsInFile(resultFile);

        if (runs.size() > 1)
            throw opp_runtime_error("More than one run in vector file.");

        assert(runs.size() == 1);

        IDList idsInFile = manager->filterIDList(idlist, runs[0], nullptr, nullptr);

        std::set<int> vectorIdsInFile;
        std::map<int, int> vectorIdToIndex;

        for (long id : idsInFile) {
            int vectorID = manager->getVector(id).getVectorId();
            vectorIdsInFile.insert(vectorID);
            vectorIdToIndex[vectorID] = idlist.indexOf(id);
        }

        auto adapter = [&result, &vectorIdToIndex, &interrupted](int vectorId, const std::vector<VectorDatum>& data) {
            addAll(result[vectorIdToIndex.at(vectorId)], data);
            if (interrupted.flag)
                throw opp_runtime_error("Vector loading interrupted");
        };

        IndexedVectorFileReader reader(resultFile->getFileSystemFilePath().c_str(), includeEventNumbers, adapter);
        reader.collectEntries(vectorIdsInFile);
    }

    vector<XYArray *> xyArrays;
    for (int i = 0; i < idlist.size(); i++) {
        xyArrays.push_back(convertVectorData(result[i], includeEventNumbers));
        result[i].clear();
    }

    return xyArrays;
}

} // namespace scave
}  // namespace omnetpp


