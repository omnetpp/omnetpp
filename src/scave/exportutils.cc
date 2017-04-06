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

#include "common/opp_ctype.h"
#include "common/commonutil.h"
#include "common/stringutil.h"
#include "common/stlutil.h"
#include "scaveutils.h"
#include "xyarray.h"
#include "resultfilemanager.h"
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
namespace scave {

typedef DataTable::Column Column;  // shorthand

vector<XYArray *> readVectorsIntoArrays(ResultFileManager *manager, const IDList& idlist, const vector<string>& filters)
{
    DataflowManager dataflowManager;
    string opt_readerNodeType = "vectorreaderbyfiletype";
    NodeTypeRegistry *registry = NodeTypeRegistry::getInstance();
    NodeType *readerNodeType = registry->getNodeType(opt_readerNodeType.c_str());
    if (!readerNodeType)
        throw opp_runtime_error("Unknown node type '%s'", opt_readerNodeType.c_str());
    ResultFileList *filteredVectorFileList = manager->getUniqueFiles(idlist);
    map<ResultFile *, Node *> vectorFileReaders;
    StringMap attrs;
    for (int i = 0; i < (int)filteredVectorFileList->size(); i++) {
        ResultFile *resultFile = filteredVectorFileList->at(i);
        attrs["filename"] = resultFile->getFileSystemFilePath();
        attrs["allowindexing"] = "true";
        Node *readerNode = readerNodeType->create(&dataflowManager, attrs);
        vectorFileReaders[resultFile] = readerNode;
    }
    delete filteredVectorFileList;

    vector<ArrayBuilderNode *> arrayBuilders;  // for exporting

    for (int i = 0; i < idlist.size(); i++) {
        // create a port for each vector on its reader node
        char portName[30];
        const VectorResult& vector = manager->getVector(idlist.get(i));
        Assert(vectorFileReaders.find(vector.getFile()) != vectorFileReaders.end());
        sprintf(portName, "%d", vector.getVectorId());
        Node *readerNode = vectorFileReaders[vector.getFile()];
        Port *outPort = readerNodeType->getPort(readerNode, portName);

        // add filters
        for (int k = 0; k < (int)filters.size(); k++) {
            // TODO support filter to merge all into a single vector
            Node *node = registry->createNode(filters[k].c_str(), &dataflowManager);
            FilterNode *filterNode = dynamic_cast<FilterNode *>(node);
            if (!filterNode)
                throw opp_runtime_error("'%s' is not a filter node", filters[k].c_str());
            dataflowManager.connect(outPort, &(filterNode->in));
            outPort = &(filterNode->out);
        }

        // create writer node(s) and connect them
        ArrayBuilderNode *arrayBuilderNode = new ArrayBuilderNode();
        dataflowManager.addNode(arrayBuilderNode);
        dataflowManager.connect(outPort, &(arrayBuilderNode->in));
        arrayBuilders.push_back(arrayBuilderNode);
    }

    // run!
    dataflowManager.execute();

    vector<XYArray *> xyArrays;
    for (int i = 0; i < idlist.size(); i++)
        xyArrays.push_back(arrayBuilders[i]->getArray());

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


