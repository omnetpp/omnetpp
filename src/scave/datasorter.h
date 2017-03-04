//==========================================================================
//  DATASORTER.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga, Tamas Borbely
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_SCAVE_DATASORTER_H
#define __OMNETPP_SCAVE_DATASORTER_H

#include <algorithm>
#include <functional>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include "common/commonutil.h"
#include "common/patternmatcher.h"
#include "common/statistics.h"
#include "resultfilemanager.h"
#include "scaveutils.h"
#include "fields.h"

namespace omnetpp {
namespace scave {

typedef std::vector<ID> IDVector;
typedef std::vector<IDVector> IDVectorVector;
typedef std::vector<std::string> StringVector;

using omnetpp::common::Statistics;

/**
 * Values arranged in a two dimensional array.
 */
class SCAVE_API XYDataset
{
    private:
        typedef ScalarResult Key;
        typedef std::map<Key, int, ResultItemFieldsLess> KeyToIndexMap;
        typedef std::vector<Statistics> Row;

        ResultItemFields rowFields;        // data in each row has the same value of these fields
        ResultItemFields columnFields;     // data in each column has the same value of these fields
        KeyToIndexMap rowKeyToIndexMap;    // row field values -> row index
        KeyToIndexMap columnKeyToIndexMap; // column field values -> column index
        std::vector<Key> rowKeys;
        std::vector<Key> columnKeys;
        std::vector<Row> values;           // index by row/column
        std::vector<int> rowOrder;         // permutation of a subset of rows
        std::vector<int> columnOrder;      // permutation of a subset of columns
    public:
        XYDataset() {};
        XYDataset(const ResultItemFields& rowFields, const ResultItemFields& columnFields)
            : rowFields(rowFields), columnFields(columnFields),
            rowKeyToIndexMap(ResultItemFieldsLess(rowFields)),
            columnKeyToIndexMap(ResultItemFieldsLess(columnFields)) {};
        void add(const ScalarResult& d);
        void swapRows(int row1, int row2);
        void sortColumnsAccordingToFirstRowMean();
        void sortRows();
        void sortColumns();

        int getRowCount() const { return rowOrder.size(); }
        int getColumnCount() const { return columnOrder.size(); }
        const ResultItemFields& getRowFields() const { return rowFields; }
        const ResultItemFields& getColumnFields() const { return columnFields; }
        std::string getRowField(int row, ResultItemField field) const;
        std::string getRowFieldNoCheck(int row, ResultItemField field) const;
        std::string getColumnField(int column, ResultItemField field) const;
        const Statistics& getValue(int row, int column) const;
};

typedef std::vector<XYDataset> XYDatasetVector;

/**
 * Helps to organize scalars in a ResultFileManager into bar charts,
 * scatter plots, etc.
 */
class SCAVE_API DataSorter
{
  private:
    ResultFileManager *resultFileMgr;
    static ResultFileManager *tmpResultFileMgr;

    typedef bool (*GroupingFunc)(const ScalarResult&, const ScalarResult&);
    typedef bool (*CompareFunc)(ID id1, ID id2);

  private:
    // comparison functions for doGrouping() and sortAndAlign()
    static bool sameGroupFileRunScalar(const ResultItem& d1, const ResultItem& d2);
    static bool sameGroupModuleScalar(const ResultItem& d1, const ResultItem& d2);
    static bool sameGroupFileRunModule(const ResultItem& d1, const ResultItem& d2);
    static bool lessByModuleRef(ID id1, ID id2);
    static bool equalByModuleRef(ID id1, ID id2);
    static bool lessByFileAndRun(ID id1, ID id2);
    static bool equalByFileAndRun(ID id1, ID id2);
    static bool lessByName(ID id1, ID id2);
    static bool equalByName(ID id1, ID id2);
    static bool lessByValue(ID id1, ID id2);

    /**
     * Form groups (IDVectors) by the grouping function passed.
     * The grouping function called with two ScalarResult and returns true iff
     * the two scalar is in the same group.
     */
    template<class GroupingFn>
    IDVectorVector doGrouping(const IDList& idlist, GroupingFn sameGroup);

    /**
     * Sort every group (IDVectors) in place by the sorting function given
     * and aligns.
     *
     * Then inserts "null" elements (id=-1) so that every group is of same length,
     * and same indices are "equal", by the equal function passed.
     */
    template<class LessFn, class EqualFn>
    void sortAndAlign(IDVectorVector& vv, LessFn less, EqualFn equal);

  public:
    /**
     * Constructor.
     */
    DataSorter(ResultFileManager *sm) {resultFileMgr = sm;}

    /**
     * Form groups (IDVectors) by the specified fields.
     */
    IDVectorVector groupByFields(const IDList& idlist, const ResultItemFields& fields);

    /**
     * Form groups (IDVectors) by the specified fields.
     * Then order each group, and insert "null" elements (id=-1)
     * so that every group is of same length, and the same indices contain
     * the same values of the non-grouping fields.
     */
    IDVectorVector groupAndAlign(const IDList& idlist, const ResultItemFields& fields);

    /**
     * Form rows from data of given idlist by grouping according to rowFields.
     * Then for each row form columns by grouping the values according to columnFields.
     * Compute the mean of the values in each cell and align the cells into a table.
     * If no data for a row/column combination, NaN will be inserted into the cell.
     */
    XYDataset groupAndAggregate(const IDList& idlist, const ResultItemFields& rowFields, const ResultItemFields& columnFields);

    /**
     * Group and align data for a scatter plot. The first vector will contain the
     * x coordinates, further vectors the y1, y2, etc series. Points are sorted
     * by x coordinate. For missing points in y1, y2, etc, the vector contains -1.
     */
    IDVectorVector prepareScatterPlot(const IDList& idlist, const char *moduleName, const char *scalarName);

    /**
     * Group and align data for a scatter plot. The first row will contain the
     * x coordinates, further rows the y1, y2, etc series. Points are sorted
     * by x coordinate. For missing points in y1, y2, etc, the row contains NaN.
     */
    XYDataset prepareScatterPlot2(const IDList& idlist, const char *xModuleName, const char *xScalarName,
            const ResultItemFields& rowFields, const ResultItemFields& columnFields);

    /**
     *
     */
    XYDatasetVector prepareScatterPlot3(const IDList& idlist, const char *moduleName, const char *scalarName,
            const ResultItemFields& rowFields, const ResultItemFields& columnFields,
            const StringVector& isoModuleNames, const StringVector& isoScalarNames, const ResultItemFields& isoFields);


    /**
     * Looks at the data given by their Id, and returns a subset of them
     * where each (moduleName, scalarName) pair occurs only once.
     *
     * Purpose: these pairs should be offered as selection of X axis for a
     * scatter plot.
     */
    IDList getModuleAndNamePairs(const IDList& idlist, int maxcount);

    /**
     * Arranges the data into a tabular form suitable for pasting into a spreadsheet:
     * one dimension is module and scalar name, the other is file+run number.
     */
    IDVectorVector prepareCopyToClipboard(const IDList& idlist);
};

} // namespace scave
}  // namespace omnetpp


#endif


