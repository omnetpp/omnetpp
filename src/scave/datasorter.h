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


#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "resultfilemanager.h"
#include "util.h"
#include "patternmatcher.h"


typedef std::vector<ID> IDVector;
typedef std::vector<IDVector> IDVectorVector;

/**
 * Helps to organize scalars in a ResultFileManager into bar charts,
 * scatter plots, etc.
 */
class ScalarDataSorter
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
     * Form groups (IDVectors) by the grouping function passed
     * (sameGroupFileRunScalar, sameGroupModuleScalar or sameGroupFileRunModule).
     */
    IDVectorVector doGrouping(const IDList& idlist, GroupingFunc sameGroup);

    /**
     * Sort every group (IDVectors) in place by the sorting function given
     * (lessByModuleRef,lessByFileAndRun, lessByScalarNameRef, lessByValue),
     * and aligns.
     *
     * Then inserts "null" elements (id=-1) so that every group is of same length,
     * and same indices are "equal", by the eqaual function passed
     * (equalByModuleRef,equalByFileAndRun, equalByScalarNameRef).
     */
    void sortAndAlign(IDVectorVector& vv, CompareFunc less, CompareFunc equal);

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
     * Group and align data for a scatter plot. The first vector will contain the
     * x coordinates, further vectors the y1, y2, etc series. Points are sorted
     * by x coordinate. For missing points in y1, y2, etc, the vector contains -1.
     */
    IDVectorVector prepareScatterPlot(const IDList& idlist, const char *moduleName, const char *scalarName);

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

#endif


