//==========================================================================
//  DATATABLE.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Tamas Borbely
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cstdio>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <functional>
#include "common/opp_ctype.h"
#include "common/commonutil.h"
#include "common/stringutil.h"
#include "scaveutils.h"
#include "datatable.h"

using namespace std;
using namespace omnetpp::common;

namespace omnetpp {
namespace scave {

bool DataTable::CellPtr::operator<(const DataTable::CellPtr& other) const
{
    if (this->isNull())
        return false;
    if (other.isNull())
        return true;

    ColumnType keyColumnType = this->table->getColumn(this->column).type;
    switch (keyColumnType) {
        case DOUBLE:
            return this->table->getDoubleValue(this->row, this->column) <
                   other.table->getDoubleValue(other.row, other.column);

        case BIGDECIMAL:
            return this->table->getBigDecimalValue(this->row, this->column) <
                   other.table->getBigDecimalValue(other.row, other.column);

        case STRING:
            return this->table->getStringValue(this->row, this->column) <
                   other.table->getStringValue(other.row, other.column);
    }
    Assert(false);
    return false;
}

/*=====================
 *       Vectors
 *=====================*/
XYDataTable::XYDataTable(const string& name, const string& description,
        const string& xColumnName, const string& yColumnName, const XYArray *vec)
    : DataTable(name, description), vec(vec)
{
    header.push_back(Column(xColumnName, BIGDECIMAL));
    header.push_back(Column(yColumnName, DOUBLE));
}

int XYDataTable::getNumRows() const
{
    return vec->length();
}

bool XYDataTable::isNull(int row, int col) const
{
    return false;
}

string XYDataTable::getStringValue(int row, int col) const
{
    // no string column
    return "";
}

BigDecimal XYDataTable::getBigDecimalValue(int row, int col) const
{
    if (col == 0) {
        BigDecimal xp = vec->getPreciseX(row);
        return xp.isNil() ? BigDecimal(vec->getX(row)) : xp;
    }
    else
        return BigDecimal::NaN;
}

double XYDataTable::getDoubleValue(int row, int col) const
{
    if (col == 1)
        return vec->getY(row);
    else
        return NaN;
}

static string createNameForXYDatasetRow(const XYDataset& data, int row, const string& separator = "/")
{
    string name;
    ResultItemFields rowFields = data.getRowFields();
    bool first = true;
    for (ResultItemFields::const_iterator rowField = rowFields.begin(); rowField != rowFields.end(); ++rowField) {
        if (!first)
            name += separator;
        name += data.getRowField(row, *rowField);
        first = false;
    }
    return name;
}

/*=====================
 *     Scatter plots
 *=====================*/
ScatterDataTable::ScatterDataTable(const string& name, const string& description, const XYDataset& data)
    : DataTable(name, description), dataset(data)
{
    for (int row = 0; row < data.getRowCount(); ++row) {
        string columnName = createNameForXYDatasetRow(data, row);
        header.push_back(Column(columnName, DOUBLE));
    }
}

int ScatterDataTable::getNumRows() const
{
    return dataset.getColumnCount();
}

bool ScatterDataTable::isNull(int row, int col) const
{
    return dataset.getValue(col, row).getCount() == 0;
}

string ScatterDataTable::getStringValue(int row, int col) const
{
    // no string column
    return "";
}

BigDecimal ScatterDataTable::getBigDecimalValue(int row, int col) const
{
    // no BigDecimal column
    return BigDecimal::Nil;
}

double ScatterDataTable::getDoubleValue(int row, int col) const
{
    return dataset.getValue(col, row).getMean();
}

/*================================
 *          Scalars
 *================================*/
ScalarDataTable::ScalarDataTable(const std::string& name, const std::string& description,
        const IDList& idlist, const ResultItemFields& groupBy, ResultFileManager& manager)
    : DataTable(name, description), manager(manager)
{
    DataSorter sorter(&manager);
    scalars = sorter.groupAndAlign(idlist, groupBy);
    // add a column for each grouping field
    if (groupBy.hasField(ResultItemField::FILE))
        header.push_back(Column("File", STRING));
    if (groupBy.hasField(ResultItemField::RUN))
        header.push_back(Column("Run", STRING));

    RunList *runList = manager.getUniqueRuns(idlist);
    StringSet *runAttrNames = manager.getUniqueRunAttributeNames(runList);
    for (auto name : *runAttrNames)
        header.push_back(Column((std::string)"runattr:"+name, STRING));
    delete runAttrNames;
    delete runList;

    if (groupBy.hasField(ResultItemField::MODULE))
        header.push_back(Column("Module", STRING));
    if (groupBy.hasField(ResultItemField::NAME))
        header.push_back(Column("Name", STRING));

    // add a column for each column in scalars headed by the non-grouping fields
    const IDVector firstRow = scalars[0];  // XXX empty idlist?
    for (int col = 0; col < (int)firstRow.size(); ++col) {
        ID id = -1;
        for (int row = 0; row < (int)scalars.size(); ++row)
            if ((id = scalars[row][col]) != -1)
                break;

        if (id != -1) {
            const ScalarResult& scalar = manager.getScalar(id);
            string name;
            if (!groupBy.hasField(ResultItemField::FILE))
                name += scalar.fileRunRef->fileRef->filePath+" ";
            if (!groupBy.hasField(ResultItemField::RUN))
                name += scalar.fileRunRef->runRef->runName+" ";
            if (!groupBy.hasField(ResultItemField::MODULE))
                name += *scalar.moduleNameRef+" ";
            if (!groupBy.hasField(ResultItemField::NAME))
                name += *scalar.nameRef+" ";
            if (!name.empty())
                name.erase(name.end()-1);  // remove last ' '
            header.push_back(Column(name, DOUBLE));
        }
        else {
            header.push_back(Column("?", DOUBLE));
        }
    }
}

int ScalarDataTable::getNumRows() const
{
    return scalars.size();
}

bool ScalarDataTable::isNull(int row, int col) const
{
    return false;
}

double ScalarDataTable::getDoubleValue(int row, int col) const
{
    if (row >= 0 && row < getNumRows() && col >= 0 && col < getNumColumns()) {
        IDVector r = scalars[row];
        ID id = r[col-(header.size() - r.size())];
        if (id != -1) {
            const ScalarResult& scalar = manager.getScalar(id);
            return scalar.value;
        }
    }
    return NaN;
}

BigDecimal ScalarDataTable::getBigDecimalValue(int row, int col) const
{
    // no BigDecimal scalars yet
    return BigDecimal::NaN;
}

std::string ScalarDataTable::getStringValue(int row, int col) const
{
    if (row >= 0 && row < getNumRows() && col >= 0 && col < getNumColumns()) {
        IDVector r = scalars[row];
        ID id = -1;
        for (int i = 0; i < getNumColumns(); ++i)
            if ((id = r[i]) != -1)
                break;

        if (id != -1) {
            const ScalarResult& scalar = manager.getScalar(id);
            Column c = getColumn(col);
            if (c.name == "File")
                return scalar.fileRunRef->fileRef->filePath;
            else if (c.name == "Run")
                return scalar.fileRunRef->runRef->runName;
            else if (c.name == "Module")
                return *scalar.moduleNameRef;
            else if (c.name == "Name")
                return *scalar.nameRef;
            else if (opp_stringbeginswith(c.name.c_str(), "runattr:"))
                return scalar.fileRunRef->runRef->getAttribute(opp_substringafter(c.name, ":"));
        }
    }
    return "";
}

/*=====================
 *       Histograms
 *=====================*/
HistogramDataTable::HistogramDataTable(const string& name, const string& description, const HistogramResult *histogramResult)
    : DataTable(name, description), histResult(histogramResult)
{
    header.push_back(Column("bin_lower", DOUBLE));
    header.push_back(Column("bin_upper", DOUBLE));
    header.push_back(Column("value", DOUBLE));  //
}

int HistogramDataTable::getNumRows() const
{
    return histResult->values.size();
}

bool HistogramDataTable::isNull(int row, int col) const
{
    return false;
}

string HistogramDataTable::getStringValue(int row, int col) const
{
    // no string column
    return "";
}

BigDecimal HistogramDataTable::getBigDecimalValue(int row, int col) const
{
    // no big decimal column
    return BigDecimal::NaN;
}

double HistogramDataTable::getDoubleValue(int row, int col) const
{
    if (col == 0)
        return histResult->bins[row];
    else if (col == 1)
        // use the next lower bound as the upper bound for this bin
        return row < (int)histResult->bins.size()-1 ? histResult->bins[row+1] : POSITIVE_INFINITY;
    else if (col == 2)
        return histResult->values[row];
    else
        return NaN;
}

/*=====================
 *  Join data tables
 *=====================*/
class DataTableIterator
{
    private:
        vector<DataTable::CellPtr> cells;
        vector<int> currentRows;
    public:
        DataTableIterator(const vector<DataTable*> &tables, int keyColumn)
            : cells(tables.size()), currentRows(tables.size())
        {
            for (int i = 0; i < (int)tables.size(); ++i)
            {
                this->cells[i] = DataTable::CellPtr(tables[i], 0, keyColumn);
                currentRows[i] = -1;
            }
        }

        bool hasNext() const
        {
            return find_if(cells.begin(), cells.end(), not1(mem_fun_ref(&DataTable::CellPtr::isNull)))
                    != cells.end();
        }

        int currentRow(int index) const { return currentRows[index]; }
        void reset() {  for_each(cells.begin(), cells.end(), mem_fun_ref(&DataTable::CellPtr::resetRow)); }
        void next();
};

void DataTableIterator::next()
{
    vector<DataTable::CellPtr>::iterator minElementPtr = min_element(cells.begin(), cells.end());
    if (minElementPtr != cells.end()) {
        DataTable::CellPtr minElement = *minElementPtr;
        currentRows.clear();
        for (vector<DataTable::CellPtr>::iterator cellPtr = cells.begin(); cellPtr != cells.end(); ++cellPtr) {
            if (cellPtr->isNull() || minElement < *cellPtr)
                currentRows.push_back(-1);
            else {
                currentRows.push_back(cellPtr->getRow());
                cellPtr->nextRow();
            }
        }
    }
}

JoinedDataTable::JoinedDataTable(const string& name, const string& description,
        const vector<DataTable *>& joinedTables, int joinOnColumn)
    : DataTable(name, description), joinedTables(joinedTables), rowMap(nullptr)
{
    tableCount = joinedTables.size();
    // checks
    for (int tableIndex = 0; tableIndex < tableCount; ++tableIndex) {
        DataTable *table = joinedTables[tableIndex];
        Assert(table && joinOnColumn < table->getNumColumns());
        Assert(table->getColumn(joinOnColumn).type == joinedTables[0]->getColumn(joinOnColumn).type);
    }

    // compute columns
    if (tableCount > 0)
        addColumn(joinedTables[0]->getColumn(joinOnColumn), 0, joinOnColumn);
    for (int tableIndex = 0; tableIndex < tableCount; ++tableIndex) {
        DataTable *table = joinedTables[tableIndex];
        int numColumns = table->getNumColumns();
        for (int col = 0; col < numColumns; ++col) {
            Column column = table->getColumn(col);
            if (col != joinOnColumn) {
                if (!table-name.empty())
                    column.name = table->name + "/" + column.name;
                addColumn(column, tableIndex, col);
            }
        }
    }

    // compute rows
    DataTableIterator iterator(joinedTables, joinOnColumn);
    rowCount = 0;
    while (iterator.hasNext()) {
        rowCount++;
        iterator.next();
    }

    rowMap = new int[rowCount*tableCount];

    iterator.reset();
    for (int row = 0; row < rowCount; ++row) {
        Assert(iterator.hasNext());
        iterator.next();

        for (int j = 0; j < (int)joinedTables.size(); ++j)
            rowMap[row*tableCount+j] = iterator.currentRow(j);
    }
}

JoinedDataTable::~JoinedDataTable()
{
    if (rowMap)
        delete[] rowMap;

    for (vector<DataTable *>::const_iterator it = joinedTables.begin(); it != joinedTables.end(); ++it) {
        if (*it)
            delete (*it);
    }
}

int JoinedDataTable::getNumRows() const
{
    return rowCount;
}

bool JoinedDataTable::isNull(int row, int col) const
{
    DataTable *table;
    int tableRow, tableCol;
    mapTableCell(row, col, table, tableRow, tableCol);
    return table == nullptr;
}

string JoinedDataTable::getStringValue(int row, int col) const
{
    DataTable *table;
    int tableRow, tableCol;
    mapTableCell(row, col, table, tableRow, tableCol);
    if (table)
        return table->getStringValue(tableRow, tableCol);
    return "";
}

BigDecimal JoinedDataTable::getBigDecimalValue(int row, int col) const
{
    DataTable *table;
    int tableRow, tableCol;
    mapTableCell(row, col, table, tableRow, tableCol);
    if (table)
        return table->getBigDecimalValue(tableRow, tableCol);
    return BigDecimal::Nil;
}

double JoinedDataTable::getDoubleValue(int row, int col) const
{
    DataTable *table;
    int tableRow, tableCol;
    mapTableCell(row, col, table, tableRow, tableCol);
    if (table)
        return table->getDoubleValue(tableRow, tableCol);
    return NaN;
}

void JoinedDataTable::mapTableCell(int row, int col, DataTable *& table, int& tableRow, int& tableCol) const
{
    Assert(0 <= row && row < rowCount && 0 <= col && col < (int)columnMap.size());

    if (col == 0) {
        tableCol = columnMap[col].second;
        table = nullptr;
        for (int tableIndex = 0; tableIndex < tableCount; ++tableIndex) {
            tableRow = rowMap[row*tableCount+tableIndex];
            if (tableRow >= 0) {
                table = joinedTables[tableIndex];
                return;
            }
        }
    }
    else {
        const pair<int, int>& tableAndColumn = columnMap[col];
        int tableIndex = tableAndColumn.first;
        tableCol = tableAndColumn.second;
        tableRow = rowMap[row*tableCount+tableIndex];
        table = tableRow >= 0 ? joinedTables[tableIndex] : nullptr;
    }
}

}  // namespace scave
}  // namespace omnetpp

