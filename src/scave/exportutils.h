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

#ifndef __OMNETPP_SCAVE_EXPORTUTILS_H
#define __OMNETPP_SCAVE_EXPORTUTILS_H

#include "scavedefs.h"
#include "common/stlutil.h"
#include "common/stringutil.h"
#include "common/csvwriter.h"

namespace omnetpp {
namespace scave {

class XYArray;
class DataTable;
class ResultFileManager;
class IDList;

template<typename T>
T translateOptionValue(const std::map<std::string,T>& mapping, const std::string& value) {
    using namespace omnetpp::common;
    auto it = mapping.find(value);
    if (it == mapping.end())
        throw opp_runtime_error("bad option value \"%s\", allowed ones are %s", value.c_str(), opp_join(keys(mapping), ", ", '"').c_str());
    return it->second;
}

/**
 * Read the VectorResult items in the IDList into the XYArrays.
 */
SCAVE_API std::vector<XYArray*> readVectorsIntoArrays(ResultFileManager *manager, const IDList& idlist, const std::vector<std::string>& filters);

/**
 * Save the given table into CSV.
 */
SCAVE_API void saveTable(common::CsvWriter& csv, const DataTable& table, bool columnNames);

/**
 * Save the given table cell into CSV.
 */
SCAVE_API void writeTableCell(common::CsvWriter& csv, const DataTable& table, int row, int col);

} // namespace scave
}  // namespace omnetpp

#endif


