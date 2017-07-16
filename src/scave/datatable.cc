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
XYDataTable::XYDataTable(const string& xColumnName, const string& yColumnName, const XYArray *vec) : vec(vec)
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
    throw opp_runtime_error("no such column with string type");
}

BigDecimal XYDataTable::getBigDecimalValue(int row, int col) const
{
    if (col == 0) {
        BigDecimal xp = vec->getPreciseX(row);
        return xp.isNil() ? BigDecimal(vec->getX(row)) : xp;
    }
    else
        return BigDecimal::NaN; //TODO throw opp_runtime_error("no such column with BigDecimal type");
}

double XYDataTable::getDoubleValue(int row, int col) const
{
    if (col == 1)
        return vec->getY(row);
    else
        return NaN; //TODO throw opp_runtime_error("no such column with double type");
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
ScatterDataTable::ScatterDataTable(const XYDataset& data) : dataset(data)
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
    throw opp_runtime_error("no such column with string type");
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
ScalarDataTable::ScalarDataTable(const IDList& idlist, const ResultItemFields& groupBy, ResultFileManager& manager) : manager(manager)
{
    DataSorter sorter(&manager);
    scalars = sorter.groupAndAlign(idlist, groupBy);
    if (idlist.isEmpty())
        return; // otherwise we can't set up columns from scalars[0]

//    // add a column for each grouping field
//    if (groupBy.hasField(ResultItemField::FILE))
//        header.push_back(Column("File", STRING));
//    if (groupBy.hasField(ResultItemField::RUN))
//        header.push_back(Column("Run", STRING));

//    RunList *runList = manager.getUniqueRuns(idlist);
//    StringSet *runAttrNames = manager.getUniqueRunAttributeNames(runList); //TODO save configuration, itervars and repetition instead
//    for (auto name : *runAttrNames)
//        header.push_back(Column((std::string)"runattr:"+name, STRING));
//    delete runAttrNames;
//    delete runList;

    if (groupBy.hasField(ResultItemField::MODULE))
        header.push_back(Column("Module", STRING));
    if (groupBy.hasField(ResultItemField::NAME))
        header.push_back(Column("Name", STRING));

    // add a column for each column in scalars headed by the non-grouping fields
    assert(!scalars.empty());
    const IDVector firstRow = scalars[0];
    for (int col = 0; col < (int)firstRow.size(); ++col) {
        ID id = -1;
        for (int row = 0; row < (int)scalars.size(); ++row)
            if ((id = scalars[row][col]) != -1)
                break;

        if (id != -1) {
            const ScalarResult& scalar = manager.getScalar(id);
            string name;
//            if (!groupBy.hasField(ResultItemField::FILE))
//                name += scalar.getFile()->getFilePath()+" ";
//            if (!groupBy.hasField(ResultItemField::RUN))
//                name += scalar.getRun()->getRunName()+" ";
            if (!groupBy.hasField(ResultItemField::MODULE))
                name += scalar.getModuleName()+" ";
            if (!groupBy.hasField(ResultItemField::NAME))
                name += scalar.getName()+" ";
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
            return scalar.getValue();
        }
    }
    return NaN; //TODO throw opp_runtime_error("no such column with double type");
}

BigDecimal ScalarDataTable::getBigDecimalValue(int row, int col) const
{
    throw opp_runtime_error("no such column with BigDecimal type");
}

const ScalarResult& ScalarDataTable::getAnyScalarInRow(int row) const
{
    if (row >= 0 && row < getNumRows()) {
        IDVector r = scalars[row];
        ID id = -1;
        for (int i = 0; i < getNumColumns(); ++i)
            if ((id = r[i]) != -1)
                break;
        if (id != -1)
            return manager.getScalar(id);
    }
    throw opp_runtime_error("row/col out of bounds, or row contains no data");
}

std::string ScalarDataTable::getStringValue(int row, int col) const
{
    const ScalarResult& scalar = getAnyScalarInRow(row);
    Column c = getColumn(col);
    if (c.name == "File")
        return scalar.getFile()->getFilePath();
    else if (c.name == "Run")
        return scalar.getRun()->getRunName();
    else if (c.name == "Module")
        return scalar.getModuleName();
    else if (c.name == "Name")
        return scalar.getName();
    else if (opp_stringbeginswith(c.name.c_str(), "runattr:"))
        return scalar.getRun()->getAttribute(opp_substringafter(c.name, ":"));
    throw opp_runtime_error("no such column with string type");
}

/*=====================
 *       Histograms
 *=====================*/
HistogramDataTable::HistogramDataTable(const HistogramResult *histogramResult) : histResult(histogramResult)
{
    header.push_back(Column("bin_lower", DOUBLE));
    header.push_back(Column("bin_upper", DOUBLE));
    header.push_back(Column("value", DOUBLE));  //
}

int HistogramDataTable::getNumRows() const
{
    return histResult->getHistogram().getNumBins();
}

bool HistogramDataTable::isNull(int row, int col) const
{
    return false;
}

string HistogramDataTable::getStringValue(int row, int col) const
{
    // no string column
    throw opp_runtime_error("no such column with string type");
}

BigDecimal HistogramDataTable::getBigDecimalValue(int row, int col) const
{
    // no big decimal column
    throw opp_runtime_error("no such column with BigDecimal type");
}

double HistogramDataTable::getDoubleValue(int row, int col) const
{
    const Histogram& hist = histResult->getHistogram();
    if (col == 0)
        return hist.getBinLowerBounds()[row];
    else if (col == 1)
        // use the next lower bound as the upper bound for this bin
        return row < (int)hist.getBinLowerBounds().size()-1 ? hist.getBinLowerBounds()[row+1] : POSITIVE_INFINITY;
    else if (col == 2)
        return hist.getBinValues()[row];
    else
        throw opp_runtime_error("no such column with double type");
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

JoinedDataTable::JoinedDataTable(const vector<DataTable *>& joinedTables, int joinOnColumn)
    : joinedTables(joinedTables), rowMap(nullptr)
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
            if (col != joinOnColumn)
                addColumn(column, tableIndex, col);
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
    throw opp_runtime_error("no such column with that type");
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
    throw opp_runtime_error("no such column with double type");
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

