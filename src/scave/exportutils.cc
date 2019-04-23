//=========================================================================
//  EXPORTUTILS.CC - part of
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

#include "exportutils.h"

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
#include "csvrecexporter.h"

using namespace std;

namespace omnetpp {
using namespace common;
namespace scave {

typedef DataTable::Column Column;  // shorthand

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


