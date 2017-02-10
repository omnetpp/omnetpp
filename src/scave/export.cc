//==========================================================================
//  EXPORT.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Tamas Borbely
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

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
#include "export.h"

using namespace std;
using namespace omnetpp::common;

namespace omnetpp {
namespace scave {

typedef DataTable::Column Column;  // shorthand

ScaveExport::~ScaveExport()
{
    close();
}

void ScaveExport::open()
{
    if (!out.is_open()) {
        fileName = makeFileName(baseFileName);
        out.open(fileName.c_str(), ios_base::binary);  // no \n translation
        if (out.fail())
            throw opp_runtime_error("Cannot open file '%s'", fileName.c_str());
    }
}

void ScaveExport::close()
{
    if (out.is_open()) {
        out.close();
    }
}

void ScaveExport::saveVector(const string& name, const string& description,
        ID vectorID, bool computed, const XYArray *xyarray, ResultFileManager& manager,
        int startIndex, int endIndex)
{
    const XYDataTable table(name, description, "X", "Y", xyarray);
    if (endIndex == -1)
        endIndex = table.getNumRows();
    saveTable(table, startIndex, endIndex);
}

void ScaveExport::saveVectors(const string& name, const string& description,
        const IDList& vectors, const vector<XYArray *> xyArrays, const ResultFileManager& manager)
{
    Assert((int)vectors.size() == (int)xyArrays.size());

    vector<DataTable *> tables;
    for (int i = 0; i < (int)xyArrays.size(); ++i) {
        const VectorResult& vector = manager.getVector(vectors.get(i));
        std::string yColumnName = makeUniqueIdentifier(*vector.moduleNameRef + "/" + *vector.nameRef);

        tables.push_back(new XYDataTable(name, description, "X", yColumnName, xyArrays[i]));
    }
    JoinedDataTable table(name, description, tables, 0);
    saveTable(table, 0, table.getNumRows());
}

void ScaveExport::saveScalars(const string& name, const string& description, const IDList& scalars, const ResultItemFields& groupBy, ResultFileManager& manager)
{
    const ScalarDataTable table(name, description, scalars, groupBy, manager);
    saveTable(table, 0, table.getNumRows());
}

void ScaveExport::saveScalars(const string& name, const string& description,
        const IDList& scalars, const string& moduleName, const string& scalarName,
        const ResultItemFields& columnFields,
        const std::vector<std::string>& isoModuleNames, const StringVector& isoScalarNames,
        const ResultItemFields& isoFields, ResultFileManager& manager)
{
    DataSorter sorter(&manager);
    StringVector rowFields;
    rowFields.push_back(ResultItemField::MODULE);
    rowFields.push_back(ResultItemField::NAME);
    XYDatasetVector xyDatasets = sorter.prepareScatterPlot3(scalars, moduleName.c_str(), scalarName.c_str(),
                ResultItemFields(rowFields), columnFields,
                isoModuleNames, isoScalarNames, isoFields);
    vector<DataTable *> tables;
    for (XYDatasetVector::const_iterator it = xyDatasets.begin(); it != xyDatasets.end(); ++it) {
        string name = "";  // TODO iso attr values
        string description = "";
        tables.push_back(new ScatterDataTable(name, description, *it));
    }

    JoinedDataTable table(name, description, tables, 0  /*first column is X*/);
    saveTable(table, 0, table.getNumRows());
}

void ScaveExport::saveHistograms(const std::string& name, const std::string& description,
        const IDList& histograms, ResultFileManager& manager)
{
    for (int i = 0; i < (int)histograms.size(); ++i) {
        const HistogramResult& histogramResult = manager.getHistogram(histograms.get(i));
        const HistogramDataTable table(name, description, &histogramResult);
        saveTable(table, 0, table.getNumRows());
    }
}

string ScaveExport::makeUniqueIdentifier(const string& name)
{
    string result = makeIdentifier(name);

    if (identifiers.find(result) != identifiers.end()) {
        string base = result;
        for (int i = 0; ; ++i) {
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
string MatlabStructExport::makeIdentifier(const string& name)
{
    string result(name);
    for (string::iterator it = result.begin(); it != result.end(); ++it)
        if (!opp_isalnum(*it))
            *it = '_';

    if (result[0] != '_' && !opp_isalpha(result[0]))
        result.insert(0, "_");
    return result;
}

string MatlabStructExport::quoteString(const string& str)
{
    string result;
    result.push_back('\'');
    for (string::const_iterator it = str.begin(); it != str.end(); ++it) {
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

string MatlabScriptExport::makeFileName(const string& name)
{
    string fileName = name;
    if (fileName.empty())
        return "matlab.m";
    else if (fileName.find('.') == string::npos)
        fileName.append(".m");
    return fileName;
}

void MatlabScriptExport::saveTable(const DataTable& table, int startRow, int endRow)
{
    open();
    string tableName = makeUniqueIdentifier(table.name);
    writeDescriptionField(table, tableName);
    writeColumnFields(table, startRow, endRow, tableName);
}

void MatlabScriptExport::writeDescriptionField(const DataTable& table, const string& tableName)
{
    out << tableName << ".description=" << quoteString(table.description) << '\n';
}

void MatlabScriptExport::writeColumnFields(const DataTable& table, int startRow, int endRow, const string& tableName)
{
    for (int col = 0; col < table.getNumColumns(); ++col) {
        Column column = table.getColumn(col);

        out << tableName + "." + makeIdentifier(column.name) << "=[" << '\n';
        switch (column.type) {
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

void MatlabScriptExport::writeStringColumn(const DataTable& table, int col, int startRow, int endRow)
{
    for (int row = startRow; row < endRow; ++row) {
        out << quoteString(table.getStringValue(row, col)) << ";\n";
    }
}

void MatlabScriptExport::writeDoubleColumn(const DataTable& table, int col, int startRow, int endRow)
{
    out.precision(prec);
    for (int row = startRow; row < endRow; ++row) {
        writeDouble(table.getDoubleValue(row, col));
        out << ";\n";
    }
}

void MatlabScriptExport::writeBigDecimalColumn(const DataTable& table, int col, int startRow, int endRow)
{
    // TODO: precision
    for (int row = startRow; row < endRow; ++row) {
        out << table.getBigDecimalValue(row, col).str() << ";\n";
    }
}

/*===============================
 *           Octave
 *===============================*/
string OctaveTextExport::makeFileName(const string& name)
{
    string fileName = name;
    if (fileName.empty())
        return "octave.dat";
    else
        return fileName;
}

void OctaveTextExport::saveTable(const DataTable& table, int startIndex, int endIndex)
{
    open();
    writeStructHeader(table);
    writeDescriptionField(table);
    writeColumnFields(table, startIndex, endIndex);
}

void OctaveTextExport::writeStructHeader(const DataTable& table)
{
    out << "# name: " << makeUniqueIdentifier(table.name) << "\n"
           "# type: struct\n"
           "# length: " << table.getNumColumns() + 1 << "\n";  // description + columns
}

void OctaveTextExport::writeDescriptionField(const DataTable& table)
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

void OctaveTextExport::writeColumnFields(const DataTable& table, int startRow, int endRow)
{
    for (int col = 0; col < table.getNumColumns(); ++col) {
        Column column = table.getColumn(col);
        switch (column.type) {
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
    }
}

void OctaveTextExport::writeDoubleColumn(const DataTable& table, int col, int startRow, int endRow)
{
    Column column = table.getColumn(col);
    out << "# name: " << makeIdentifier(column.name) << "\n"
           "# type: cell\n"
           "# rows: 1\n"
           "# columns: 1\n"
           "# name: <cell-element>\n"
           "# type: matrix\n"
           "# rows: " << (endRow - startRow) << "\n"
           "# columns: 1\n";

    out.precision(prec);
    for (int row = startRow; row < endRow; ++row) {
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

void OctaveTextExport::writeBigDecimalColumn(const DataTable& table, int col, int startRow, int endRow)
{
    Column column = table.getColumn(col);
    out << "# name: " << makeIdentifier(column.name) << "\n"
           "# type: cell\n"
           "# rows: 1\n"
           "# columns: 1\n"
           "# name: <cell-element>\n"
           "# type: matrix\n"
           "# rows: " << (endRow - startRow) << "\n"
           "# columns: 1\n";
    for (int row = startRow; row < endRow; ++row) {
        BigDecimal value = table.getBigDecimalValue(row, col);
        out << value.str() << "\n";
    }
}

void OctaveTextExport::writeStringColumn(const DataTable& table, int col, int startRow, int endRow)
{
    Column column = table.getColumn(col);
    out << "# name: " << makeIdentifier(column.name) << "\n"
           "# type: cell\n"
           "# rows: 1\n"
           "# columns: 1\n"
           "# name: <cell-element>\n"
           "# type: string\n"
           "# elements: " << (endRow - startRow) << "\n";
    for (int row = startRow; row < endRow; ++row) {
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
string CsvExport::makeFileName(const string& name)
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

void CsvExport::saveVector(const string& name, const string& description,
        ID vectorID, bool computed, const XYArray *xyarray, ResultFileManager& manager,
        int startIndex, int endIndex)
{
    string xColumn, yColumn;
    if (computed) {
        xColumn = "X";  // TODO generate proper names derived from the computation
        yColumn = "Y";
    }
    else {
        const VectorResult& vector = manager.getVector(vectorID);
        xColumn = "time";
        yColumn = *vector.moduleNameRef + "." + *vector.nameRef;
    }
    const XYDataTable table(name, description, xColumn, yColumn, xyarray);
    if (endIndex == -1)
        endIndex = table.getNumRows();
    saveTable(table, startIndex, endIndex);
}

void CsvExport::saveTable(const DataTable& table, int startRow, int endRow)
{
    open();
    writeHeader(table);
    for (int row = startRow; row < endRow; ++row)
        writeRow(table, row);
    if (fileNameSuffix > 0)
        close();
}

void CsvExport::writeHeader(const DataTable& table)
{
    if (columnNames) {
        for (int col = 0; col < table.getNumColumns(); ++col) {
            if (col > 0)
                out << separator;
            Column column = table.getColumn(col);
            writeString(column.name);
        }
        out << eol;
    }
}

void CsvExport::writeRow(const DataTable& table, int row)
{
    for (int col = 0; col < table.getNumColumns(); ++col) {
        Column column = table.getColumn(col);
        if (col > 0)
            out << separator;
        if (!table.isNull(row, col)) {
            switch (column.type) {
                case DataTable::DOUBLE:
                    writeDouble(table.getDoubleValue(row, col));
                    break;

                case DataTable::BIGDECIMAL:
                    writeBigDecimal(table.getBigDecimalValue(row, col));
                    break;

                case DataTable::STRING:
                    writeString(table.getStringValue(row, col));
                    break;
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

void CsvExport::writeString(const string& value)
{
    if (needsQuote(value)) {
        out << quoteChar;
        for (string::const_iterator it = value.begin(); it != value.end(); ++it)
            writeChar(*it);
        out << quoteChar;
    }
    else {
        out << value;
    }
}

bool CsvExport::needsQuote(const string& value)
{
    switch (quoteMethod) {
        case ESCAPE:
            for (string::const_iterator it = value.begin(); it != value.end(); ++it)
                if (*it == separator || *it == quoteChar || *it == '\\')
                    return true;
            return value.find(eol) != string::npos;

        case DOUBLE:
            for (string::const_iterator it = value.begin(); it != value.end(); ++it)
                if (*it == separator || *it == quoteChar || eol.find(*it) != string::npos)
                    return true;
            return value.find(eol) != string::npos;

        default:
            throw opp_runtime_error("Unknown quote method.");
    }
}

void CsvExport::writeChar(char ch)
{
    switch (quoteMethod) {
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

string CsvExport::makeIdentifier(const string& name)
{
    return name;
}

ScaveExport *ExporterFactory::createExporter(const string& format)
{
    if (format == "octave")
        return new OctaveTextExport;
    else if (format == "matlab")
        return new MatlabScriptExport;
    else if (format == "csv")
        return new CsvExport;
    else
        return nullptr;
}

}  // namespace scave
}  // namespace omnetpp

