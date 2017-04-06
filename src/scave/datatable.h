//=========================================================================
//  DATATABLE.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Tamas Borbely
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_SCAVE_DATATABLE_H
#define __OMNETPP_SCAVE_DATATABLE_H

#include <string>
#include <vector>
#include <set>
#include <fstream>
#include <utility>
#include "common/bigdecimal.h"
#include "scavedefs.h"
#include "xyarray.h"
#include "resultfilemanager.h"
#include "datasorter.h"

namespace omnetpp {
namespace scave {

typedef std::vector<std::string> StringVector;

/**
 * Abstract base class for tabular data. Used by exporters.
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
            Column(const std::string& name, ColumnType type) : name(name), type(type) {}
        };

        class CellPtr {
            private:
                DataTable* table;
                int row;
                int column;
            public:
                CellPtr() :table(nullptr), row(-1), column(-1) {}
                CellPtr(DataTable *table, int row, int column) : table(table), row(row), column(column) {}
                bool isNull() const { return table == nullptr || row < 0 || row >= table->getNumRows() || column < 0 || column >= table->getNumColumns(); }
                int getRow() const {return row; }
                int getColumn() const { return column; }
                void resetRow() { row = 0; }
                void nextRow() { row++; }
                bool operator<(const CellPtr& other) const;
        };
    protected:
        std::vector<Column> header;
        DataTable() {}
        void addColumn(const Column& column) { header.push_back(column); }
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
        XYDataTable(const std::string& xColumnName, const std::string& yColumnName, const XYArray *vec);
        virtual int getNumRows() const override;
        virtual bool isNull(int row, int col) const override;
        virtual double getDoubleValue(int row, int col) const override;
        virtual BigDecimal getBigDecimalValue(int row, int col) const override;
        virtual std::string getStringValue(int row, int col) const override;
};

/**
 * Table containing one X column and several Y columns.
 */
class SCAVE_API ScatterDataTable : public DataTable
{
    private:
        const XYDataset& dataset;
    public:
        ScatterDataTable(const XYDataset& data);
        virtual int getNumRows() const override;
        virtual bool isNull(int row, int col) const override;
        virtual double getDoubleValue(int row, int col) const override;
        virtual BigDecimal getBigDecimalValue(int row, int col) const override;
        virtual std::string getStringValue(int row, int col) const override;
};

/**
 * A table containing scalars grouped by some of its fields.
 */
class SCAVE_API ScalarDataTable : public DataTable
{
    private:
        IDVectorVector scalars;
        ResultFileManager& manager;
    public:
        ScalarDataTable(const IDList& idlist, const ResultItemFields& groupBy, ResultFileManager& manager);

        const ScalarResult& getAnyScalarInRow(int row) const;
        virtual int getNumRows() const override;
        virtual bool isNull(int row, int col) const override;
        virtual double getDoubleValue(int row, int col) const override;
        virtual BigDecimal getBigDecimalValue(int row, int col) const override;
        virtual std::string getStringValue(int row, int col) const override;
};

/**
 * A table containing a histogram. It has three columns: bin_lower, bin_upper and value.
 */
class SCAVE_API HistogramDataTable : public DataTable
{
    private:
        const HistogramResult *histResult;
    public:
        HistogramDataTable(const HistogramResult *histogramResult);
        virtual int getNumRows() const override;
        virtual bool isNull(int row, int col) const override;
        virtual double getDoubleValue(int row, int col) const override;
        virtual BigDecimal getBigDecimalValue(int row, int col) const override;
        virtual std::string getStringValue(int row, int col) const override;
};

/**
 * Computes the outer join of DataTables. Note: if there are conflicting
 * column names, they should be renamed beforehand.
 */
class SCAVE_API JoinedDataTable : public DataTable
{
    private:
        std::vector<DataTable*> joinedTables;
        int tableCount;
        int rowCount;
        std::vector<std::pair<int,int> > columnMap; // maps column -> (tableIndex,tableColumn)
        int* rowMap = nullptr; // maps (row,tableIndex) -> tableRow; implemented as a two dimensional array (has rowCount*tableCount elements)
    public:
        JoinedDataTable(const std::vector<DataTable*>& joinedTables, int joinOnColumn);
        virtual ~JoinedDataTable();

        virtual int getNumRows() const override;
        virtual bool isNull(int row, int col) const override;
        virtual double getDoubleValue(int row, int col) const override;
        virtual BigDecimal getBigDecimalValue(int row, int col) const override;
        virtual std::string getStringValue(int row, int col) const override;
    private:
        void addColumn(const Column& column, int tableIndex, int colIndex);
        void mapTableCell(int row, int column, DataTable *&table, int& tableRow, int& tableCol) const;
};

inline void JoinedDataTable::addColumn(const Column& column, int tableIndex, int colIndex)
{
    header.push_back(column); //addColumn(column);
    columnMap.push_back(std::make_pair(tableIndex, colIndex));
}

} // namespace scave
}  // namespace omnetpp


#endif


