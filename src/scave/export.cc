//==========================================================================
//  EXPORT.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Tamas Borbely
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cstdio>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <functional>
#include "opp_ctype.h"
#include "commonutil.h"
#include "scaveutils.h"
#include "export.h"

NAMESPACE_BEGIN

using namespace std;

//XXX unfortunately, VC8.0 doesn't like the following lines, so Column needs to be fully qualified in the source...
//using DataTable::Column;
//using DataTable::ColumnType;

/*=====================
 *       Vectors
 *=====================*/
bool DataTable::CellPtr::operator <(const DataTable::CellPtr &other) const
{
    if (this->isNull())
        return false;
    if (other.isNull())
        return true;

    ColumnType keyColumnType = this->table->getColumn(this->column).type;
    switch (keyColumnType)
    {
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
XYDataTable::XYDataTable(const string &name, const string &description,
                         const string &xColumnName, const string &yColumnName, const XYArray *vec)
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
    if (col == 0)
    {
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

static string createNameForXYDatasetRow(const XYDataset &data, int row, const string &separator = "/")
{
    string name;
    ResultItemFields rowFields = data.getRowFields();
    bool first = true;
    for (ResultItemFields::const_iterator rowField = rowFields.begin(); rowField != rowFields.end(); ++rowField)
    {
        if (!first) name += separator;
        name += data.getRowField(row, *rowField);
        first = false;
    }
    return name;
}

/*=====================
 *     Scatter plots
 *=====================*/
ScatterDataTable::ScatterDataTable(const string &name, const string &description, const XYDataset &data)
    : DataTable(name, description), dataset(data)
{
    for (int row = 0; row < data.getRowCount(); ++row)
    {
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
ScalarDataTable::ScalarDataTable(const std::string name, const std::string description,
            const IDList &idlist, ResultItemFields groupBy, ResultFileManager &manager)
            : DataTable(name, description), manager(manager)
{
    DataSorter sorter(&manager);
    scalars = sorter.groupAndAlign(idlist, groupBy);

    // add a column for each grouping field
    if (groupBy.hasField(ResultItemField::FILE))   header.push_back(Column("File", STRING));
    if (groupBy.hasField(ResultItemField::RUN))    header.push_back(Column("Run", STRING));
    if (groupBy.hasField(ResultItemField::MODULE)) header.push_back(Column("Module", STRING));
    if (groupBy.hasField(ResultItemField::NAME))   header.push_back(Column("Name", STRING));

    // add a column for each column in scalars headed by the non-grouping fields
    const IDVector firstRow = scalars[0]; // XXX empty idlist?
    for (int col = 0; col < (int)firstRow.size(); ++col)
    {
        ID id = -1;
        for (int row = 0; row < (int)scalars.size(); ++row)
            if ((id = scalars[row][col]) != -1)
                break;

        if (id != -1)
        {
            const ScalarResult &scalar = manager.getScalar(id);
            string name;
            if (!groupBy.hasField(ResultItemField::FILE))   name += scalar.fileRunRef->fileRef->filePath+"_";
            if (!groupBy.hasField(ResultItemField::RUN))    name += scalar.fileRunRef->runRef->runName+"_";
            if (!groupBy.hasField(ResultItemField::MODULE)) name += *scalar.moduleNameRef+"_";
            if (!groupBy.hasField(ResultItemField::NAME))   name += *scalar.nameRef+"_";
            if (!name.empty()) name.erase(name.end()-1); // remove last '_'
            header.push_back(Column(name, DOUBLE));
        }
        else
        {
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
    if (row >= 0 && row < getNumRows() && col >= 0 && col < getNumColumns())
    {
        IDVector r = scalars[row];
        ID id = r[col-(header.size() - r.size())];
        if (id != -1)
        {
            const ScalarResult &scalar = manager.getScalar(id);
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
    if (row >= 0 && row < getNumRows() && col >= 0 && col < getNumColumns())
    {
        IDVector r = scalars[row];
        ID id = -1;
        for (int i = 0; i < getNumColumns(); ++i)
            if ((id=r[i]) != -1)
                break;
        if (id != -1)
        {
            const ScalarResult &scalar = manager.getScalar(id);
            Column c = getColumn(col);
            if (c.name == "File") return scalar.fileRunRef->fileRef->filePath;
            else if (c.name == "Run") return scalar.fileRunRef->runRef->runName;
            else if (c.name == "Module") return *scalar.moduleNameRef;
            else if (c.name == "Name") return *scalar.nameRef;
        }
    }
    return "";
}

/*=====================
 *       Histograms
 *=====================*/
HistogramDataTable::HistogramDataTable(const string &name, const string &description, const HistogramResult *histogramResult)
    : DataTable(name, description), histResult(histogramResult)
{
    header.push_back(Column("bin_lower", DOUBLE));
    header.push_back(Column("bin_upper", DOUBLE));
    header.push_back(Column("value", DOUBLE)); //
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
    if (minElementPtr != cells.end())
    {
        DataTable::CellPtr minElement = *minElementPtr;
        currentRows.clear();
        for (vector<DataTable::CellPtr>::iterator cellPtr = cells.begin(); cellPtr != cells.end(); ++cellPtr)
        {
            if (cellPtr->isNull() || minElement < *cellPtr)
                currentRows.push_back(-1);
            else
            {
                currentRows.push_back(cellPtr->getRow());
                cellPtr->nextRow();
            }
        }
    }
}

JoinedDataTable::JoinedDataTable(const string name, const string description,
                         const vector<DataTable*> &joinedTables, int joinOnColumn)
    : DataTable(name, description), joinedTables(joinedTables), rowMap(NULL)
{
   tableCount = joinedTables.size();
   // checks
   for (int tableIndex = 0; tableIndex < tableCount; ++tableIndex)
   {
       DataTable *table = joinedTables[tableIndex];
       Assert(table && joinOnColumn < table->getNumColumns());
       Assert(table->getColumn(joinOnColumn).type == joinedTables[0]->getColumn(joinOnColumn).type);
   }

   // compute columns
   if (tableCount > 0)
       addColumn(joinedTables[0]->getColumn(joinOnColumn), 0, joinOnColumn);
   for (int tableIndex = 0; tableIndex < tableCount; ++tableIndex)
   {
       DataTable *table = joinedTables[tableIndex];
       int numColumns = table->getNumColumns();
       for (int col = 0; col < numColumns; ++col)
       {
           Column column = table->getColumn(col);
           if (col != joinOnColumn)
           {
               if (!table-name.empty())
                   column.name = table->name + "/" + column.name;
               addColumn(column, tableIndex, col);
           }
       }
   }

   // compute rows
   DataTableIterator iterator(joinedTables, joinOnColumn);
   rowCount = 0;
   while (iterator.hasNext())
   {
       rowCount++;
       iterator.next();
   }

   rowMap = new int[rowCount*tableCount];

   iterator.reset();
   for (int row = 0; row < rowCount; ++row)
   {
       Assert(iterator.hasNext());
       iterator.next();

       for (int j = 0; j < (int)joinedTables.size(); ++j)
           rowMap[row*tableCount+j]=iterator.currentRow(j);
   }
}

JoinedDataTable::~JoinedDataTable()
{
   if (rowMap)
       delete[] rowMap;

   for (vector<DataTable*>::const_iterator it = joinedTables.begin(); it != joinedTables.end(); ++it)
   {
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
    return table == NULL;
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

void JoinedDataTable::mapTableCell(int row, int col, DataTable* &table, int &tableRow, int &tableCol) const
{
    Assert(0 <= row && row < rowCount && 0 <= col && col < (int)columnMap.size());

    if (col == 0)
    {
        tableCol = columnMap[col].second;
        table = NULL;
        for (int tableIndex = 0; tableIndex < tableCount; ++tableIndex) {
            tableRow = rowMap[row*tableCount+tableIndex];
            if (tableRow >= 0)
            {
                table = joinedTables[tableIndex];
                return;
            }
        }
    }
    else
    {
        const pair<int,int> &tableAndColumn = columnMap[col];
        int tableIndex = tableAndColumn.first;
        tableCol = tableAndColumn.second;
        tableRow = rowMap[row*tableCount+tableIndex];
        table = tableRow >= 0 ? joinedTables[tableIndex] : NULL;
    }
}


/*===============================
 *           Export
 *===============================*/

ScaveExport::~ScaveExport()
{
    close();
}

void ScaveExport::open()
{
    if (!out.is_open())
    {
        fileName = makeFileName(baseFileName);
        out.open(fileName.c_str(), ios_base::binary); // no \n translation
        if (out.fail())
            throw opp_runtime_error("Cannot open file '%s'", fileName.c_str());
    }
}

void ScaveExport::close()
{
    if (out.is_open())
    {
        out.close();
    }
}

void ScaveExport::saveVector(const string &name, const string &description,
                             ID vectorID, bool computed, const XYArray *xyarray, ResultFileManager &manager,
                             int startIndex, int endIndex)
{
    const XYDataTable table(name, description, "X", "Y", xyarray);
    if (endIndex == -1)
        endIndex = table.getNumRows();
    saveTable(table, startIndex, endIndex);
}

void ScaveExport::saveVectors(const string &name, const string &description,
                             const IDList &vectors, const vector<XYArray*> xyArrays, const ResultFileManager &manager)
{
    Assert((int)vectors.size() == (int)xyArrays.size());

    vector<DataTable*> tables;
    for (int i = 0; i < (int)xyArrays.size(); ++i)
    {
        const VectorResult& vector = manager.getVector(vectors.get(i));
        std::string yColumnName = makeUniqueIdentifier(*vector.moduleNameRef + "/" + *vector.nameRef);

        tables.push_back(new XYDataTable(name, description, "X", yColumnName, xyArrays[i]));
    }
    JoinedDataTable table(name, description, tables, 0);
    saveTable(table, 0, table.getNumRows());
}

void ScaveExport::saveScalars(const string &name, const string &description, const IDList &scalars, ResultItemFields groupBy, ResultFileManager &manager)
{
    const ScalarDataTable table(name, description, scalars, groupBy, manager);
    saveTable(table, 0, table.getNumRows());
}

void ScaveExport::saveScalars(const string &name, const string &description,
                                const IDList &scalars, const string &moduleName, const string &scalarName,
                                ResultItemFields columnFields,
                                const std::vector<std::string> &isoModuleNames, const StringVector &isoScalarNames,
                                ResultItemFields isoFields, ResultFileManager &manager)
{
    DataSorter sorter(&manager);
    StringVector rowFields;
    rowFields.push_back(ResultItemField::MODULE);
    rowFields.push_back(ResultItemField::NAME);
    XYDatasetVector xyDatasets = sorter.prepareScatterPlot3(scalars, moduleName.c_str(), scalarName.c_str(),
                                                            ResultItemFields(rowFields), columnFields,
                                                            isoModuleNames, isoScalarNames, isoFields);
    vector<DataTable *> tables;
    for (XYDatasetVector::const_iterator it = xyDatasets.begin(); it != xyDatasets.end(); it++)
    {
        string name = ""; // TODO iso attr values
        string description = "";
        tables.push_back(new ScatterDataTable(name, description, *it));
    }

    JoinedDataTable table(name, description, tables, 0 /*first column is X*/);
    saveTable(table, 0, table.getNumRows());
}

void ScaveExport::saveHistograms(const std::string &name, const std::string &description,
                                 const IDList &histograms, ResultFileManager &manager)
{
    for (int i = 0; i < (int)histograms.size(); ++i)
    {
        const HistogramResult& histogramResult = manager.getHistogram(histograms.get(i));
        const HistogramDataTable table(name, description, &histogramResult);
        saveTable(table, 0, table.getNumRows());
    }
}


string ScaveExport::makeUniqueIdentifier(const string &name)
{
    string result = makeIdentifier(name);

    if (identifiers.find(result) != identifiers.end())
    {
        string base = result;
        for (int i=0; ; ++i)
        {
            char suffix[32];
            sprintf(suffix, "_%d", i);
            result = base + suffix;
            if (identifiers.find(result) == identifiers.end())
                break;
        }
    }

    identifiers.insert(result);
    return result;
}

/*===============================
 *           Matlab
 *===============================*/
string MatlabStructExport::makeIdentifier(const string &name)
{
    string result(name);
    for (string::iterator it=result.begin(); it!=result.end(); ++it)
        if (!opp_isalnum(*it))
            *it = '_';
    if (result[0] != '_' && !opp_isalpha(result[0]))
        result.insert(0, "_");
    return result;
}

string MatlabStructExport::quoteString(const string &str)
{
    string result;
    result.push_back('\'');
    for (string::const_iterator it = str.begin(); it != str.end(); ++it)
    {
        if (*it == '\'')
            result.push_back('\'');  // quotes (') are escaped as two quotes ('') in matlab
        result.push_back(*it);
    }
    result.push_back('\'');

    return result;
}

void MatlabStructExport::writeDouble(double value)
{
    if (isNaN(value))
        out << "NaN";
    else if (isPositiveInfinity(value))
        out << "Inf";
    else if (isNegativeInfinity(value))
        out << "-Inf";
    else
        out << value;
}


string MatlabScriptExport::makeFileName(const string name)
{
    string fileName = name;
    if (fileName.empty())
        return "matlab.m";
    else if (fileName.find('.') == string::npos)
        fileName.append(".m");
    return fileName;
}

void MatlabScriptExport::saveTable(const DataTable &table, int startRow, int endRow)
{
    open();
    string tableName = makeUniqueIdentifier(table.name);
    writeDescriptionField(table, tableName);
    writeColumnFields(table, startRow, endRow, tableName);
}

void MatlabScriptExport::writeDescriptionField(const DataTable &table, const string tableName)
{
    out << tableName << ".description=" << quoteString(table.description) << '\n';
}

void MatlabScriptExport::writeColumnFields(const DataTable &table, int startRow, int endRow, const string tableName)
{
    for (int col = 0; col < table.getNumColumns(); ++col)
    {
        DataTable::Column column = table.getColumn(col);

        out << tableName + "." + makeIdentifier(column.name) << "=[" << '\n';
        switch (column.type)
        {
        case DataTable::DOUBLE:
            writeDoubleColumn(table, col, startRow, endRow);
            break;
        case DataTable::BIGDECIMAL:
            writeBigDecimalColumn(table, col, startRow, endRow);
            break;
        case DataTable::STRING:
            writeStringColumn(table, col, startRow, endRow);
            break;
        }
        out << "]\n\n";
    }
}

void MatlabScriptExport::writeStringColumn(const DataTable &table, int col, int startRow, int endRow)
{
    for (int row = startRow; row < endRow; ++row)
    {
        out << quoteString(table.getStringValue(row, col)) << ";\n";
    }
}

void MatlabScriptExport::writeDoubleColumn(const DataTable &table, int col, int startRow, int endRow)
{
    out.precision(prec);
    for (int row = startRow; row < endRow; ++row)
    {
        writeDouble(table.getDoubleValue(row, col));
        out << ";\n";
    }
}

void MatlabScriptExport::writeBigDecimalColumn(const DataTable &table, int col, int startRow, int endRow)
{
    // TODO: precision
    for (int row = startRow; row < endRow; ++row)
    {
        out << table.getBigDecimalValue(row, col).str() << ";\n";
    }
}

/*===============================
 *           Octave
 *===============================*/
string OctaveTextExport::makeFileName(const string name)
{
    string fileName = name;
    if (fileName.empty())
        return "octave.dat";
    else
        return fileName;
}

void OctaveTextExport::saveTable(const DataTable &table, int startIndex, int endIndex)
{
    open();
    writeStructHeader(table);
    writeDescriptionField(table);
    writeColumnFields(table, startIndex, endIndex);
}

void OctaveTextExport::writeStructHeader(const DataTable &table)
{
    out << "# name: " << makeUniqueIdentifier(table.name) << "\n"
           "# type: struct\n"
           "# length: " << table.getNumColumns() + 1 // description + columns
        << "\n";
}

void OctaveTextExport::writeDescriptionField(const DataTable &table)
{
    out << "# name: description\n"
           "# type: cell\n"
           "# rows: 1\n"
           "# columns: 1\n"
           "# name: <cell-element>\n"
           "# type: string\n"
           "# elements: 1\n"
           "# length: " << table.description.size() << "\n"
        << table.description << "\n";
}

void OctaveTextExport::writeColumnFields(const DataTable &table, int startRow, int endRow)
{
    for (int col=0; col<table.getNumColumns(); ++col)
    {
        DataTable::Column column = table.getColumn(col);
        switch (column.type)
        {
        case DataTable::DOUBLE: writeDoubleColumn(table, col, startRow, endRow); break;
        case DataTable::BIGDECIMAL: writeBigDecimalColumn(table, col, startRow, endRow); break;
        case DataTable::STRING: writeStringColumn(table, col, startRow, endRow); break;
        }
    }
}

void OctaveTextExport::writeDoubleColumn(const DataTable &table, int col, int startRow, int endRow)
{
    DataTable::Column column = table.getColumn(col);
    out << "# name: " << makeIdentifier(column.name) << "\n"
           "# type: cell\n"
           "# rows: 1\n"
           "# columns: 1\n"
           "# name: <cell-element>\n"
           "# type: matrix\n"
           "# rows: " << (endRow - startRow) << "\n"
           "# columns: 1\n";

    out.precision(prec);
    for (int row = startRow; row <endRow; ++row)
    {
        double value = table.getDoubleValue(row, col);
        if (isNaN(value))
            out << "NaN";
        else if (isPositiveInfinity(value))
            out << "Inf";
        else if (isNegativeInfinity(value))
            out << "-Inf";
        else
            out << value;
        out << "\n";
    }
}

void OctaveTextExport::writeBigDecimalColumn(const DataTable &table, int col, int startRow, int endRow)
{
    DataTable::Column column = table.getColumn(col);
    out << "# name: " << makeIdentifier(column.name) << "\n"
           "# type: cell\n"
           "# rows: 1\n"
           "# columns: 1\n"
           "# name: <cell-element>\n"
           "# type: matrix\n"
           "# rows: " << (endRow - startRow) << "\n"
           "# columns: 1\n";
    for (int row = startRow; row <endRow; ++row)
    {
        BigDecimal value = table.getBigDecimalValue(row, col);
        out << value.str() << "\n";
    }
}

void OctaveTextExport::writeStringColumn(const DataTable &table, int col, int startRow, int endRow)
{
    DataTable::Column column = table.getColumn(col);
    out << "# name: " << makeIdentifier(column.name) << "\n"
           "# type: cell\n"
           "# rows: 1\n"
           "# columns: 1\n"
           "# name: <cell-element>\n"
           "# type: string\n"
           "# elements: " << (endRow - startRow) << "\n";
    for (int row = startRow; row < endRow; ++row)
    {
        std::string value = table.getStringValue(row, col);
        out << "# length: " << value.size() << "\n"
            << value << "\n";
    }
}

/*===============================
 *           Csv
 *===============================*/

/**
 * Generate a new filename for each table by appending '-1','-2',... suffixes to the base filename.
 */
string CsvExport::makeFileName(const string name)
{
    string file(name), extension(".csv");
    stringstream suffix;

    if (fileNameSuffix > 0)
        suffix << '-' << (fileNameSuffix++);

    if (name.empty())
        file = "table";
    else {
        string::size_type pos = name.rfind('.');
        if (pos == string::npos)
            file = name;
        else {
            file = name.substr(0, pos);
            extension = name.substr(pos, name.size() - pos);
        }
    }
    return file + suffix.str() + extension;
}

void CsvExport::saveVector(const string &name, const string &description,
                             ID vectorID, bool computed, const XYArray *xyarray, ResultFileManager &manager,
                             int startIndex, int endIndex)
{
    string xColumn, yColumn;
    if (computed)
    {
        xColumn = "X"; // TODO generate proper names derived from the computation
        yColumn = "Y";
    }
    else
    {
        const VectorResult &vector = manager.getVector(vectorID);
        xColumn = "time";
        yColumn = *vector.moduleNameRef + "." + *vector.nameRef;
    }
    const XYDataTable table(name, description, xColumn, yColumn, xyarray);
    if (endIndex == -1)
        endIndex = table.getNumRows();
    saveTable(table, startIndex, endIndex);
}

void CsvExport::saveTable(const DataTable &table, int startRow, int endRow)
{
    open();
    writeHeader(table);
    for (int row = startRow; row < endRow; ++row)
        writeRow(table, row);
    if (fileNameSuffix > 0)
        close();
}

void CsvExport::writeHeader(const DataTable &table)
{
    if (columnNames)
    {
        for (int col = 0; col < table.getNumColumns(); ++col)
        {
            if (col > 0)
                out << separator;
            DataTable::Column column = table.getColumn(col);
            writeString(column.name);
        }
        out << eol;
    }
}

void CsvExport::writeRow(const DataTable &table, int row)
{
    for (int col = 0; col < table.getNumColumns(); ++col)
    {
        DataTable::Column column = table.getColumn(col);
        if (col > 0)
            out << separator;
        if (!table.isNull(row, col))
        {
            switch (column.type)
            {
            case DataTable::DOUBLE: writeDouble(table.getDoubleValue(row, col)); break;
            case DataTable::BIGDECIMAL: writeBigDecimal(table.getBigDecimalValue(row, col)); break;
            case DataTable::STRING: writeString(table.getStringValue(row, col)); break;
            }
        }
    }
    out << eol;
}

void CsvExport::writeDouble(double value)
{
    if (isNaN(value))
        out << "NaN";
    else if (isPositiveInfinity(value))
        out << "Inf";
    else if (isNegativeInfinity(value))
        out << "-Inf";
    else
        out << value;
}

void CsvExport::writeBigDecimal(BigDecimal value)
{
    out << value.str();
}

void CsvExport::writeString(const string &value)
{
    if (needsQuote(value))
    {
        out << quoteChar;
        for (string::const_iterator it = value.begin(); it != value.end(); ++it)
        {
            writeChar(*it);
        }
        out << quoteChar;
    }
    else
    {
        out << value;
    }
}

bool CsvExport::needsQuote(const string &value)
{
    switch (quoteMethod)
    {
    case ESCAPE:
        for (string::const_iterator it = value.begin(); it != value.end(); ++it)
        {
            if (*it == separator || *it == quoteChar || *it == '\\')
                return true;
        }
        return value.find(eol) != string::npos;
    case DOUBLE:
        for (string::const_iterator it = value.begin(); it != value.end(); ++it)
        {
            if (*it == separator || *it == quoteChar || eol.find(*it) != string::npos)
                return true;
        }
        return value.find(eol) != string::npos;
    default:
        throw opp_runtime_error("Unknown quote method.");
    }
}

void CsvExport::writeChar(char ch)
{
    switch (quoteMethod)
    {
    case ESCAPE:
        if (ch == '\\' || ch == separator)
            out << '\\';
        out << ch;
        break;
    case DOUBLE:
        if (ch == separator)
            out << ch;
        out << ch;
        break;
    }
}

string CsvExport::makeIdentifier(const string &name)
{
    return name;
}


ScaveExport *ExporterFactory::createExporter(const string format)
{
    if (format == "octave")
        return new OctaveTextExport;
    else if (format == "matlab")
        return new MatlabScriptExport;
    else if (format == "csv")
        return new CsvExport;
    else
        return NULL;
}

NAMESPACE_END

