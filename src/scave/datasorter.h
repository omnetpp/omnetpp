//==========================================================================
//  DATASORTER.H - part of
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

#ifndef _DATASORTER_H_
#define _DATASORTER_H_

#include <algorithm>
#include <functional>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "resultfilemanager.h"
#include "commonutil.h"
#include "patternmatcher.h"
#include "scaveutils.h"
#include "statistics.h"
#include "fields.h"

NAMESPACE_BEGIN

typedef std::vector<ID> IDVector;
typedef std::vector<IDVector> IDVectorVector;
typedef std::vector<std::string> StringVector;

/**
 * Values arranged in a two dimensional array.
 */
class SCAVE_API XYDataset
{
    private:
        typedef ScalarResult Key;
        typedef std::map<Key, int, ResultItemFieldsLess> KeyToIndexMap;
        typedef std::vector<Statistics> Row;

        ResultItemFields rowFields;            // data in each row has the same value of these fields
        ResultItemFields columnFields;         // data in each column has the same value of these fields
        KeyToIndexMap rowKeyToIndexMap;    // row field values -> row index
        KeyToIndexMap columnKeyToIndexMap; // column field values -> column index
        std::vector<Key> rowKeys;
        std::vector<Key> columnKeys;
        std::vector<Row> values; // index by row/column
        std::vector<int> rowOrder;         // permutation of a subset of rows
        std::vector<int> columnOrder;      // permutation of a subset of columns
    public:
        XYDataset() {};
        XYDataset(ResultItemFields rowFields, ResultItemFields columnFields)
            : rowFields(rowFields), columnFields(columnFields),
            rowKeyToIndexMap(ResultItemFieldsLess(rowFields)),
            columnKeyToIndexMap(ResultItemFieldsLess(columnFields)) {};
        void add(const ScalarResult &d);
        void swapRows(int row1, int row2);
        void sortColumnsAccordingToFirstRowMean();
        void sortRows();
        void sortColumns();

        int getRowCount() { return rowOrder.size(); }
        int getColumnCount() { return columnOrder.size(); }
        ResultItemFields getRowFields() { return rowFields; }
        ResultItemFields getColumnFields() { return columnFields; }
        std::string getRowField(int row, ResultItemField field);
        std::string getColumnField(int column, ResultItemField field);
        Statistics getValue(int row, int column);
};

typedef std::vector<XYDataset> XYDatasetVector;

inline std::string XYDataset::getRowField(int row, ResultItemField field)
{
    if (row < 0 || row >= getRowCount() || !rowFields.hasField(field))
        return "";
    return field.getFieldValue(rowKeys[rowOrder[row]]);
}

inline std::string XYDataset::getColumnField(int column, ResultItemField field)
{
    if (column < 0 || column >= getColumnCount() || !columnFields.hasField(field))
        return "";
    return field.getFieldValue(columnKeys[columnOrder[column]]);
}

inline Statistics XYDataset::getValue(int row, int column)
{
    if (row < 0 || column < 0 || row >= getRowCount() || column >= getColumnCount())
        return Statistics();
    return values.at(rowOrder[row]).at(columnOrder[column]);
}

/**
 * Helps to organize scalars in a ResultFileManager into bar charts,
 * scatter plots, etc.
 */
class SCAVE_API ScalarDataSorter
{
  private:
    ResultFileManager *resultFileMgr;
    static ResultFileManager *tmpScalarMgr;

    typedef bool (*GroupingFunc)(const ScalarResult&, const ScalarResult&);
    typedef bool (*CompareFunc)(ID id1, ID id2);

  private:
    // comparison functions for doGrouping() and sortAndAlign()
    static bool sameGroupFileRunScalar(const ScalarResult& d1, const ScalarResult& d2);
    static bool sameGroupModuleScalar(const ScalarResult& d1, const ScalarResult& d2);
    static bool sameGroupFileRunModule(const ScalarResult& d1, const ScalarResult& d2);
    static bool lessByModuleRef(ID id1, ID id2);
    static bool equalByModuleRef(ID id1, ID id2);
    static bool lessByFileAndRun(ID id1, ID id2);
    static bool equalByFileAndRun(ID id1, ID id2);
    static bool lessByScalarNameRef(ID id1, ID id2);
    static bool equalByScalarNameRef(ID id1, ID id2);
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
    ScalarDataSorter(ResultFileManager *sm) {resultFileMgr = sm;}

    /**
     * Form groups (IDVectors) by runRef+scalarName.
     * Then order each group by module name, and insert "null" elements (id=-1)
     * so that every group is of same length, and the same indices contain
     * the same moduleNameRefs.
     */
    IDVectorVector groupByRunAndName(const IDList& idlist);

    /**
     * Form groups (IDVectors) by moduleName+scalarName.
     * Order each group by runRef, and insert "null" elements (id=-1) so that
     * every group is of same length, and the same indices contain the same runRef.
     */
    IDVectorVector groupByModuleAndName(const IDList& idlist);

    /**
     * Form groups (IDVectors) by the specified fields.
     * Then order each group, and insert "null" elements (id=-1)
     * so that every group is of same length, and the same indices contain
     * the same values of the non-grouping fields.
     */
    IDVectorVector groupByFields(const IDList& idlist, ResultItemFields fields);

    /**
     * Form rows from data of given idlist by grouping according to rowFields.
     * Then for each row form columns by grouping the values according to columnFields.
     * Compute the mean of the values in each cell and align the cells into a table.
     * If no data for a row/column combination, NaN will be inserted into the cell.
     */
    XYDataset groupAndAggregate(const IDList& idlist, ResultItemFields rowFields, ResultItemFields columnFields);

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
            ResultItemFields rowFields, ResultItemFields columnFields);

    /**
     * TODO
     */
    XYDatasetVector prepareScatterPlot3(const IDList& idlist, const char *moduleName, const char *scalarName,
            ResultItemFields rowFields, ResultItemFields columnFields,
            const StringVector isoModuleNames, const StringVector isoScalarNames);


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

NAMESPACE_END


#endif


