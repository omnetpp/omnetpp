//=========================================================================
//  EXPORT.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2006 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef _EXPORT_H_
#define _EXPORT_H_

#include <string>
#include <vector>
#include <set>
#include <fstream>
#include "scavedefs.h"
#include "bigdecimal.h"
#include "xyarray.h"
#include "resultfilemanager.h"
#include "datasorter.h"

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
    public:
        const std::string name;
        const std::string description;
    protected:
        std::vector<Column> header;
        DataTable(const std::string name, const std::string description) : name(name), description(description) {}
    public:
        int numOfColumns() const { return header.size(); }
        Column column(int i) const { return header[i]; }
        virtual ~DataTable() {}
        virtual int numOfRows() const = 0;
        virtual double getDoubleValue(int row, int col) const = 0;
        virtual BigDecimal getBigDecimalValue(int row, int col) const = 0;
        virtual std::string getStringValue(int row, int col) const = 0;

    // rows (may contain NaN and infinity)
};

/**
 * A table containing an output vector or the result of processing output vectors.
 * The vector has two column, X and Y.
 */
class XYDataTable : public DataTable
{
    private:
        const XYArray *vec;
    public:
        XYDataTable(const std::string name, const std::string description,
            const std::string xColumnName, const std::string yColumnName, const XYArray *vec);
        virtual int numOfRows() const;
        virtual double getDoubleValue(int row, int col) const;
        virtual BigDecimal getBigDecimalValue(int row, int col) const;
        virtual std::string getStringValue(int row, int col) const;
};

/**
 * A table containing scalars grouped by some of its fields.
 */
class ScalarDataTable : public DataTable
{
    private:
        IDVectorVector scalars;
        ResultFileManager &manager;
    public:
        ScalarDataTable(const std::string name, const std::string description,
            const IDList &idlist, ResultItemFields groupBy, ResultFileManager &manager);

        virtual int numOfRows() const;
        virtual double getDoubleValue(int row, int col) const;
        virtual BigDecimal getBigDecimalValue(int row, int col) const;
        virtual std::string getStringValue(int row, int col) const;
};



/**
 * Base class for scalar/vector exporters.
 */
class SCAVE_API ScaveExport
{
    protected:
        std::string fileName;
        std::ofstream out;
        int prec;

    public:
        ScaveExport() : prec(DEFAULT_PRECISION) {}
        virtual ~ScaveExport();

        void setPrecision(int prec) { this->prec = prec; }
        virtual std::string makeFileName(const std::string name) = 0;
        void open(const std::string filename);
        void close();
        
        virtual void saveVector(const std::string name, const std::string description, 
                        ID vectorID, bool computed, const XYArray *vec, ResultFileManager &manager,
                        int startIndex=0, int endIndex=-1);
        virtual void saveScalars(const std::string name, const std::string description, const IDList &scalars, ResultItemFields groupBy, ResultFileManager &manager);
    protected:
        virtual void saveTable(const DataTable &rows, int startIndex, int endIndex) = 0;
};

/**
 * Common base class for formats that creates a matlab structure when loaded.
 */
class MatlabStructExport : public ScaveExport
{
    private:
        std::set<std::string> identifiers;
    protected:
        void writeDouble(double value);
        std::string makeUniqueIdentifier(const std::string &name);
        static std::string makeIdentifier(const std::string &name);
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
 * double (max 15 digit), so their precision lost.
 *
 * FIXME: BigDecimals always written out with maximal precision (19 digit), prec is ignored.
 */
class SCAVE_API MatlabScriptExport : public MatlabStructExport
{
    public:
        virtual std::string makeFileName(const std::string name);
    protected:
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
    public:
        virtual std::string makeFileName(const std::string name);
    protected:
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
        const char *eol;
        QuoteMethod quoteMethod;
        bool columnNames;
    public:
        CsvExport() : separator(','), quoteChar('"'), eol("\r\n"), quoteMethod(DOUBLE), columnNames(true) {}
        virtual std::string makeFileName(const std::string name);
        virtual void saveVector(const std::string name, const std::string description, 
                        ID vectorID, bool computed, const XYArray *vec, ResultFileManager &manager,
                        int startIndex=0, int endIndex=-1);
    protected:
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

#endif


