//=========================================================================
//  EXPORT.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Tamas Borbely
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef _EXPORT_H_
#define _EXPORT_H_

#include <string>
#include <vector>
#include <set>
#include <fstream>
#include <utility>
#include "scavedefs.h"
#include "bigdecimal.h"
#include "xyarray.h"
#include "resultfilemanager.h"
#include "datasorter.h"

NAMESPACE_BEGIN

typedef std::vector<std::string> StringVector;

/**
 * Basic data structure to be exported.
 */
class SCAVE_API DataTable
{
    public:
        enum ColumnType
        {
            DOUBLE,
            BIGDECIMAL,
            STRING
        };

        struct Column
        {
            std::string name;
            ColumnType  type;
            Column(const std::string name, ColumnType type) : name(name), type(type) {}
        };

        class CellPtr {
            private:
                DataTable* table;
                int row;
                int column;
            public:
                CellPtr() :table(NULL), row(-1), column(-1) {}
                CellPtr(DataTable *table, int row, int column) : table(table), row(row), column(column) {}
                bool isNull() const { return table == NULL || row < 0 || row >= table->getNumRows() || column < 0 || column >= table->getNumColumns(); }
                int getRow() const {return row; }
                int getColumn() const { return column; }
                void resetRow() { row = 0; }
                void nextRow() { row++; }
                bool operator<(const CellPtr &other) const;
        };
    public:
        const std::string name;
        const std::string description;
    protected:
        std::vector<Column> header;
        DataTable(const std::string &name, const std::string &description) : name(name), description(description) {}
        void addColumn(const Column &column) { header.push_back(column); }
    public:
        virtual ~DataTable() {}
        int getNumColumns() const { return header.size(); }
        Column getColumn(int i) const { return header[i]; }
        virtual int getNumRows() const = 0;
        virtual bool isNull(int row, int col) const = 0;
        virtual double getDoubleValue(int row, int col) const = 0;
        virtual BigDecimal getBigDecimalValue(int row, int col) const = 0;
        virtual std::string getStringValue(int row, int col) const = 0;

    // rows (may contain NaN and infinity)
};



/**
 * A table containing an output vector or the result of processing output vectors.
 * The vector has two column, X and Y.
 */
class SCAVE_API XYDataTable : public DataTable
{
    private:
        const XYArray *vec;
    public:
        XYDataTable(const std::string &name, const std::string &description,
            const std::string &xColumnName, const std::string &yColumnName, const XYArray *vec);
        virtual int getNumRows() const;
        virtual bool isNull(int row, int col) const;
        virtual double getDoubleValue(int row, int col) const;
        virtual BigDecimal getBigDecimalValue(int row, int col) const;
        virtual std::string getStringValue(int row, int col) const;
};

/**
 * Table containing one X column and several Y columns.
 */
class SCAVE_API ScatterDataTable : public DataTable
{
    private:
        const XYDataset &dataset;
    public:
        ScatterDataTable(const std::string &name, const std::string &description, const XYDataset &data);
        virtual int getNumRows() const;
        virtual bool isNull(int row, int col) const;
        virtual double getDoubleValue(int row, int col) const;
        virtual BigDecimal getBigDecimalValue(int row, int col) const;
        virtual std::string getStringValue(int row, int col) const;
};

/**
 * A table containing scalars grouped by some of its fields.
 */
class SCAVE_API ScalarDataTable : public DataTable
{
    private:
        IDVectorVector scalars;
        ResultFileManager &manager;
    public:
        ScalarDataTable(const std::string name, const std::string description,
            const IDList &idlist, ResultItemFields groupBy, ResultFileManager &manager);

        virtual int getNumRows() const;
        virtual bool isNull(int row, int col) const;
        virtual double getDoubleValue(int row, int col) const;
        virtual BigDecimal getBigDecimalValue(int row, int col) const;
        virtual std::string getStringValue(int row, int col) const;
};

/**
 * A table containing a histogram. It has three columns: bin_lower, bin_upper and value.
 */
class SCAVE_API HistogramDataTable : public DataTable
{
    private:
        const HistogramResult *histResult;
    public:
        HistogramDataTable(const std::string &name, const std::string &description, const HistogramResult *histogramResult);
        virtual int getNumRows() const;
        virtual bool isNull(int row, int col) const;
        virtual double getDoubleValue(int row, int col) const;
        virtual BigDecimal getBigDecimalValue(int row, int col) const;
        virtual std::string getStringValue(int row, int col) const;
};

/**
 * Computes the outer join of DataTables.
 */
class SCAVE_API JoinedDataTable : public DataTable
{
    private:
        std::vector<DataTable*> joinedTables;
        int tableCount;
        int rowCount;
        std::vector<std::pair<int,int> > columnMap; // maps column -> (tableIndex,tableColumn)
        int* rowMap; // maps (row,tableIndex) -> tableRow
                     //   implemented as a two dimensional array (has rowCount*tableCount elements)
    public:
        JoinedDataTable(const std::string name, const std::string description,
            const std::vector<DataTable*> &joinedTables, int joinOnColumn);
        virtual ~JoinedDataTable();

        virtual int getNumRows() const;
        virtual bool isNull(int row, int col) const;
        virtual double getDoubleValue(int row, int col) const;
        virtual BigDecimal getBigDecimalValue(int row, int col) const;
        virtual std::string getStringValue(int row, int col) const;
    private:
        void addColumn(const Column &column, int tableIndex, int colIndex);
        void mapTableCell(int row, int column, DataTable *&table, int &tableRow, int &tableCol) const;
};

inline void JoinedDataTable::addColumn(const Column &column, int tableIndex, int colIndex)
{
    header.push_back(column); //addColumn(column);
    columnMap.push_back(std::make_pair(tableIndex, colIndex));
}



/**
 * Base class for scalar/vector exporters.
 */
class SCAVE_API ScaveExport
{
    private:
        std::set<std::string> identifiers;

    protected:
        std::string baseFileName;
        std::string fileName;
        std::ofstream out;
        int prec;

    protected:
        void open();
        void close();
        virtual std::string makeFileName(const std::string name) = 0;

    public:
        ScaveExport() : prec(DEFAULT_PRECISION) {}
        virtual ~ScaveExport();

        void setPrecision(int prec) { this->prec = prec; }
        void setBaseFileName(const std::string baseFileName) { this->baseFileName = baseFileName; }

        virtual void saveVector(const std::string &name, const std::string &description,
                        ID vectorID, bool computed, const XYArray *vec, ResultFileManager &manager,
                        int startIndex=0, int endIndex=-1);
        virtual void saveVectors(const std::string &name, const std::string &description,
                                     const IDList &vectors, const std::vector<XYArray*> xyarrays,
                                     const ResultFileManager &manager);
        virtual void saveScalars(const std::string &name, const std::string &description,
                                    const IDList &scalars, ResultItemFields groupBy, ResultFileManager &manager);

        virtual void saveScalars(const std::string &name, const std::string &description,
                                    const IDList &scalars, const std::string &moduleName, const std::string &scalarName,
                                    ResultItemFields columnFields,
                                    const std::vector<std::string> &isoModuleNames, const StringVector &isoScalarNames,
                                    ResultItemFields isoFields, ResultFileManager &manager);

        virtual void saveHistograms(const std::string &name, const std::string &description,
                                    const IDList &histograms, ResultFileManager &manager);

        const std::string &getLastFileName() const { return fileName; }
    protected:
        std::string makeUniqueIdentifier(const std::string &name);
        virtual std::string makeIdentifier(const std::string &name) = 0;
        virtual void saveTable(const DataTable &table, int startIndex, int endIndex) = 0;
};

/**
 * Common base class for formats that creates a matlab structure when loaded.
 */
class MatlabStructExport : public ScaveExport
{
    protected:
        void writeDouble(double value);
        virtual std::string makeIdentifier(const std::string &name);
        static std::string quoteString(const std::string &str);
};

/**
 * Exports data as a Matlab script.
 *
 * The script defines a structure containing a description field,
 * and a column fields for each column. The column fields are
 * equal length vectors.
 *
 * Note: Although BigDecimal values (e.g. simulation time) are
 * written out with the specified precision, Matlab reads them as IEEE
 * double (max 15 digit), so their precision gets lost.
 *
 * FIXME: BigDecimals always written out with maximal precision (19 digit), prec is ignored.
 */
class SCAVE_API MatlabScriptExport : public MatlabStructExport
{
    protected:
        virtual std::string makeFileName(const std::string name);
        virtual void saveTable(const DataTable &table, int startRow, int endRow);
    private:
        void writeDescriptionField(const DataTable &rows, const std::string tableName);
        void writeColumnFields(const DataTable &rows, int startRow, int endRow, const std::string tableName);
        void writeDoubleColumn(const DataTable &table, int col, int startRow, int endRow);
        void writeBigDecimalColumn(const DataTable &table, int col, int startRow, int endRow);
        void writeStringColumn(const DataTable &table, int col, int startRow, int endRow);
};

/**
 * Exports data in Octave's "save -text" format. This format can be loaded
 * into R (r-project.org) as well.
 *
 * To see the an example, type the following commands in Octave:
 * <pre>
 *    a = [1 2; 3 4; 5 6]
 *    save -text - a
 * </pre>
 *
 * The output:
 * <pre>
 *  # Created by Octave 2.1.73 ...
 *  # name: a
 *  # type: matrix
 *  # rows: 3
 *  # columns: 2
 *   1 2
 *   3 4
 *   5 6
 * </pre>
 *
 * If several variables are saved, the above pattern repeats.
 *
 * FIXME: BigDecimals always written out with maximal precision (19 digit), prec is ignored.
 */
class SCAVE_API OctaveTextExport : public MatlabStructExport
{
    protected:
        virtual std::string makeFileName(const std::string name);
        virtual void saveTable(const DataTable &table, int startRow, int endRow);
    private:
        void writeStructHeader(const DataTable &table);
        void writeDescriptionField(const DataTable &table);
        void writeColumnFields(const DataTable &table, int startRow, int endRow);
        void writeDoubleColumn(const DataTable &table, int col, int startRow, int endRow);
        void writeBigDecimalColumn(const DataTable &table, int col, int startRow, int endRow);
        void writeStringColumn(const DataTable &table, int col, int startRow, int endRow);
};

/**
 * Export data in various CSV formats.
 * With default parameters the format is the same as described in RFC 4180.
 */
class SCAVE_API CsvExport : public ScaveExport
{
    public: // options
        typedef enum {DOUBLE, ESCAPE} QuoteMethod;
        char separator;
        char quoteChar;
        std::string eol;
        QuoteMethod quoteMethod;
        bool columnNames;
        // state
        int fileNameSuffix; // zero = do not add suffix, otherwise incremented when writing a new table
    public:
        CsvExport() : separator(','), quoteChar('"'), eol("\r\n"), quoteMethod(DOUBLE),
                        columnNames(true), fileNameSuffix(0) {}
        virtual void saveVector(const std::string &name, const std::string &description,
                        ID vectorID, bool computed, const XYArray *vec, ResultFileManager &manager,
                        int startIndex=0, int endIndex=-1);
    protected:
        virtual std::string makeFileName(const std::string name);
        virtual std::string makeIdentifier(const std::string &name);
        virtual void saveTable(const DataTable &table, int startRow, int endRow);
    private:
        void writeHeader(const DataTable &table);
        void writeRow(const DataTable &table, int row);
        void writeDouble(double value);
        void writeBigDecimal(BigDecimal value);
        void writeString(const std::string &value);
        bool needsQuote(const std::string &value);
        void writeChar(char ch);
};

class SCAVE_API ExporterFactory
{
    public:
        static ScaveExport *createExporter(const std::string format);
};

NAMESPACE_END


#endif


