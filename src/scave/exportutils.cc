//=========================================================================
//  EXPORTUTILS.H - part of
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

#include <set>
#include "common/opp_ctype.h"
#include "common/commonutil.h"
#include "common/stringutil.h"
#include "common/stlutil.h"
#include "scaveutils.h"
#include "xyarray.h"
#include "resultfilemanager.h"
#include "indexedvectorfilereader.h"
#include "datasorter.h"
#include "datatable.h"
#include "dataflowmanager.h"
#include "nodetype.h"
#include "nodetyperegistry.h"
#include "arraybuilder.h"
#include "vectorfilereader.h"
#include "exportutils.h"

#include "exportutils.h"

#include "csvrecexporter.h"

using namespace std;

namespace omnetpp {
using namespace common;
namespace scave {

typedef DataTable::Column Column;  // shorthand

XYArray *convertVectorData(const std::vector<VectorDatum>& data)
{
    int l = data.size();

    double* xv = new double[l];
    double* yv = new double[l];
    BigDecimal* xpv = new BigDecimal[l];
    eventnumber_t* ev = new eventnumber_t[l];

    for (int i = 0; i < data.size(); ++i) {
        const VectorDatum &vd = data[i];
        xv[i] = vd.simtime.dbl();
        yv[i] = vd.value;
        xpv[i] = vd.simtime;
        ev[i] = vd.eventNumber;
    }

    return new XYArray(l, xv, yv, xpv, ev);
}

vector<XYArray *> readVectorsIntoArrays(ResultFileManager *manager, const IDList& idlist, const std::vector<std::string>& filters)
{
    if (!filters.empty())
        throw opp_runtime_error("Vector filters not supported.");

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

        auto adapter = [&result, &vectorIdToIndex](int vectorId, const std::vector<VectorDatum>& data) {
            addAll(result[vectorIdToIndex.at(vectorId)], data);
        };

        IndexedVectorFileReader reader(resultFile->getFileSystemFilePath().c_str(), adapter);
        reader.collectEntries(vectorIdsInFile);
    }

    vector<XYArray *> xyArrays;
    for (int i = 0; i < idlist.size(); i++) {
        xyArrays.push_back(convertVectorData(result[i]));
        result[i].clear();
    }

    return xyArrays;
}

void saveTable(common::CsvWriter& csv, const DataTable& table, bool columnNames)
{
    // write header
    if (columnNames) {
        for (int col = 0; col < table.getNumColumns(); ++col)
            csv.writeString(table.getColumn(col).name);
        csv.writeNewLine();
    }

    // write data
    for (int row = 0; row < table.getNumRows(); ++row) {
        for (int col = 0; col < table.getNumColumns(); ++col)
            writeTableCell(csv, table, row, col);
        csv.writeNewLine();
    }
}

void writeTableCell(CsvWriter& csv, const DataTable& table, int row, int col)
{
    Column column = table.getColumn(col);
    if (table.isNull(row, col))
        csv.writeBlank();
    else {
        switch (column.type) {
            case DataTable::DOUBLE: csv.writeDouble(table.getDoubleValue(row, col)); break;
            case DataTable::BIGDECIMAL: csv.writeBigDecimal(table.getBigDecimalValue(row, col)); break;
            case DataTable::STRING: csv.writeString(table.getStringValue(row, col)); break;
            default: Assert(false);
        }
    }
}

} // namespace scave
}  // namespace omnetpp


