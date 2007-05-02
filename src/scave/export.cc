//==========================================================================
//  EXPORT.CC - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include <iostream>
#include "export.h"

using namespace std;

//XXX unfortunately, VC8.0 doesn't like the following lines, so Column needs to be fully qualified in the source...
//using DataTable::Column;
//using DataTable::ColumnType;

/*
 */
static double zero = 0.0;
static double dblNaN = zero / zero;
static double dblPositiveInfinity = 1 / zero;
static double dblNegativeInfinity = -1 / zero;
static inline bool isNaN(double d) { return d!=d; }
static inline bool isPositiveInfinity(double d) { return d==dblPositiveInfinity; }
static inline bool isNegativeInfinity(double d) { return d==dblNegativeInfinity; }

/*=====================
 *       Vectors
 *=====================*/
XYDataTable::XYDataTable(const string name, const string description, const XYArray *vec)
    : DataTable(name, description), vec(vec)
{
    header.push_back(Column("X", BIGDECIMAL));
    header.push_back(Column("Y", DOUBLE));
}

int XYDataTable::numOfRows() const
{
    return vec->length();
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
        return dblNaN;
}

/*================================
 *          Scalars
 *================================*/
ScalarDataTable::ScalarDataTable(const std::string name, const std::string description,
            IDList idlist, ScalarFields groupBy, ResultFileManager &manager)
            : DataTable(name, description), manager(manager)
{
    ScalarDataSorter sorter(&manager);
    scalars = sorter.groupByFields(idlist, groupBy);

    // add a column for each grouping field
    if (groupBy.hasField(ScalarFields::FILE))   header.push_back(Column("File", STRING));
    if (groupBy.hasField(ScalarFields::RUN))    header.push_back(Column("Run", STRING));
    if (groupBy.hasField(ScalarFields::MODULE)) header.push_back(Column("Module", STRING));
    if (groupBy.hasField(ScalarFields::NAME))   header.push_back(Column("Name", STRING));

    // add a column for each column in scalars headed by the non-grouping fields
    const IDVector firstRow = scalars[0];
    for (int col = 0; col < firstRow.size(); ++col)
    {
        ID id = -1;
        for (int row = 0; row < scalars.size(); ++row)
            if ((id = scalars[row][col]) != -1)
                break;

        if (id != -1)
        {
            const ScalarResult &scalar = manager.getScalar(id);
            string name;
            if (!groupBy.hasField(ScalarFields::FILE))   name += scalar.fileRunRef->fileRef->filePath+"_";
            if (!groupBy.hasField(ScalarFields::RUN))    name += scalar.fileRunRef->runRef->runName+"+";
            if (!groupBy.hasField(ScalarFields::MODULE)) name += *scalar.moduleNameRef+"_";
            if (!groupBy.hasField(ScalarFields::NAME))   name += *scalar.nameRef+"_";
            if (!name.empty()) name.erase(name.end()-1); // remove last '_'
            header.push_back(Column(name, DOUBLE));
        }
        else
        {
            header.push_back(Column("?", DOUBLE));
        }
    }
}

int ScalarDataTable::numOfRows() const
{
    return scalars.size();
}

double ScalarDataTable::getDoubleValue(int row, int col) const
{
    if (row >= 0 && row < numOfRows() && col >= 0 && col < numOfColumns())
    {
        IDVector r = scalars[row];
        ID id = r[col-(header.size() - r.size())];
        if (id != -1)
        {
            const ScalarResult &scalar = manager.getScalar(id);
            return scalar.value;
        }
    }
    return dblNaN;
}

BigDecimal ScalarDataTable::getBigDecimalValue(int row, int col) const
{
    // no BigDecimal scalars yet
    return BigDecimal::NaN;
}

std::string ScalarDataTable::getStringValue(int row, int col) const
{
    if (row >= 0 && row < numOfRows() && col >= 0 && col < numOfColumns())
    {
        IDVector r = scalars[row];
        ID id = -1;
        for (int i = 0; i < numOfColumns(); ++i)
            if ((id=r[i]) != -1)
                break;
        if (id != -1)
        {
            const ScalarResult &scalar = manager.getScalar(id);
            Column c = column(col);
            if (c.name == "File") return scalar.fileRunRef->fileRef->filePath;
            else if (c.name == "Run") return scalar.fileRunRef->runRef->runName;
            else if (c.name == "Module") return *scalar.moduleNameRef;
            else if (c.name == "Name") return *scalar.nameRef;
        }
    }
    return "";
}

/*===============================
 *           Export
 *===============================*/

ScaveExport::~ScaveExport()
{
}

void ScaveExport::open(const string fileName)
{
    if (out.is_open())
    {
        out.close();
    }

    out.open(fileName.c_str(), ios_base::binary); // no \n translation
    if (out.fail())
        throw opp_runtime_error("Cannot open file '%s'", fileName.c_str());

    this->fileName = fileName;
}

void ScaveExport::close()
{
    if (out.is_open())
    {
        out.close();
    }
}

void ScaveExport::saveVector(const string name, const string description, const XYArray *vec, int startIndex, int endIndex)
{
    const XYDataTable table(name, description, vec);
    if (endIndex == -1)
        endIndex = table.numOfRows();
    saveTable(table, startIndex, endIndex);
}

void ScaveExport::saveScalars(const string name, const string description, IDList scalars, ScalarFields groupBy, ResultFileManager &manager)
{
    const ScalarDataTable table(name, description, scalars, groupBy, manager);
    saveTable(table, 0, table.numOfRows());
}

/*===============================
 *           Matlab
 *===============================*/
string MatlabStructExport::makeIdentifier(const string &name)
{
    string result(name);
    for (string::iterator it=result.begin(); it!=result.end(); ++it)
        if (!isalnum(*it))
            *it = '_';
    return result;
}

string MatlabStructExport::quoteString(const string &str)
{
    return string(str); // TODO
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
    writeDescriptionField(table);
    writeColumnFields(table, startRow, endRow);
}

void MatlabScriptExport::writeDescriptionField(const DataTable &table)
{
    out << makeIdentifier(table.name) << ".description=" << '"' << quoteString(table.description) << '"' << '\n';
}

void MatlabScriptExport::writeColumnFields(const DataTable &table, int startRow, int endRow)
{
    for (int col = 0; col < table.numOfColumns(); ++col)
    {
        DataTable::Column column = table.column(col);

        out << makeIdentifier(table.name) + "." + makeIdentifier(column.name) << "=[" << '\n';
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
        out << '"' << table.getStringValue(row, col) << '"' << ";\n";
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
    writeStructHeader(table);
    writeDescriptionField(table);
    writeColumnFields(table, startIndex, endIndex);
}

void OctaveTextExport::writeStructHeader(const DataTable &table)
{
    out << "# name: " << makeIdentifier(table.name) << "\n"
           "# type: struct\n"
           "# length: " << table.numOfColumns() + 1 // description + columns
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
    for (int col=0; col<table.numOfColumns(); ++col)
    {
        DataTable::Column column = table.column(col);
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
    DataTable::Column column = table.column(col);
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
    DataTable::Column column = table.column(col);
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
        BigDecimal &value = table.getBigDecimalValue(row, col);
        out << value.str() << "\n";
    }
}

void OctaveTextExport::writeStringColumn(const DataTable &table, int col, int startRow, int endRow)
{
    DataTable::Column column = table.column(col);
    out << "# name: " << makeIdentifier(column.name) << "\n"
           "# type: cell\n"
           "# rows: 1\n"
           "# columns: 1\n"
           "# name: <cell-element>\n"
           "# type: string\n"
           "# elements: " << (endRow - startRow) << "\n";
    for (int row = startRow; row < endRow; ++row)
    {
        std::string &value = table.getStringValue(row, col);
        out << "# length: " << value.size() << "\n"
            << value << "\n";
    }
}

/*===============================
 *           Csv
 *===============================*/
// XXX separate file for each table?
string CsvExport::makeFileName(const string name)
{
    string filename = name;
    if (filename.empty())
        return "table.csv";
    else if (fileName.find('.') == string::npos)
        return fileName + ".csv";
    else
        return fileName;
}

void CsvExport::saveTable(const DataTable &table, int startRow, int endRow)
{
    writeHeader(table);
    for (int row = startRow; row < endRow; ++row)
        writeRow(table, row);
}

void CsvExport::writeHeader(const DataTable &table)
{
    for (int col = 0; col < table.numOfColumns(); ++col)
    {
        if (col > 0)
            out << ",";
        DataTable::Column column = table.column(col);
        writeString(column.name);
    }
    out << "\r\n";
}

void CsvExport::writeRow(const DataTable &table, int row)
{
    for (int col = 0; col < table.numOfColumns(); ++col)
    {
        DataTable::Column column = table.column(col);
        if (col > 0)
            out << ",";
        switch (column.type)
        {
        case DataTable::DOUBLE: writeDouble(table.getDoubleValue(row, col)); break;
        case DataTable::BIGDECIMAL: writeBigDecimal(table.getBigDecimalValue(row, col)); break;
        case DataTable::STRING: writeString(table.getStringValue(row, col)); break;
        }
    }
    out << "\r\n";
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
    if (value.find(',') != string::npos || value.find('"') != string::npos || value.find("\r\n") != string::npos)
    {
        out << '"';
        for (string::const_iterator it = value.begin(); it != value.end(); ++it)
        {
            out << *it;
            if (*it == '"') out << '"';
        }
        out << '"';
    }
    else
    {
        out << value;
    }
}
