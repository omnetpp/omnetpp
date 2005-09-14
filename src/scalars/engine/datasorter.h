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

#include "scalarmanager.h"


typedef std::vector<int> IntVector;
typedef std::vector<IntVector> IntVectorVector;



/**
 * Helps organize the data in a ScalarManager into bar charts,
 * scatter plots, etc.
 */
class DataSorter
{
  private:
    ScalarManager *scalarMgr;
    static ScalarManager *tmpScalarMgr;

    typedef bool (*GroupingFunc)(const ScalarManager::Datum&, const ScalarManager::Datum&);
    typedef bool (*CompareFunc)(int id1, int id2);

  private:
    // comparison functions for doGrouping() and sortAndAlign()
    static bool sameGroupFileRunScalar(const ScalarManager::Datum& d1, const ScalarManager::Datum& d2);
    static bool sameGroupModuleScalar(const ScalarManager::Datum& d1, const ScalarManager::Datum& d2);
    static bool sameGroupFileRunModule(const ScalarManager::Datum& d1, const ScalarManager::Datum& d2);
    static bool lessByModuleRef(int id1, int id2);
    static bool equalByModuleRef(int id1, int id2);
    static bool lessByFileAndRun(int id1, int id2);
    static bool equalByFileAndRun(int id1, int id2);
    static bool lessByScalarNameRef(int id1, int id2);
    static bool equalByScalarNameRef(int id1, int id2);
    static bool lessByValue(int id1, int id2);

    /**
     * Form groups (IntVectors) by the grouping function passed
     * (sameGroupFileRunScalar, sameGroupModuleScalar or sameGroupFileRunModule).
     */
    IntVectorVector doGrouping(const IntVector& idlist, GroupingFunc sameGroup);

    /**
     * Sort every group (IntVectors) in place by the sorting function given
     * (lessByModuleRef,lessByFileAndRun, lessByScalarNameRef, lessByValue),
     * and aligns.
     *
     * Then inserts "null" elements (id=-1) so that every group is of same length,
     * and same indices are "equal", by the eqaual function passed
     * (equalByModuleRef,equalByFileAndRun, equalByScalarNameRef).
     */
    void sortAndAlign(IntVectorVector& vv, CompareFunc less, CompareFunc equal);

  public:
    /**
     * Constructor.
     */
    DataSorter(ScalarManager *sm) {scalarMgr = sm;}

    /**
     * Get a filtered subset of all scalars. All three filter parameters may be null,
     * or may contain wildcards (*,?). Full string search (substrings need * both ends).
     */
    IntVector getFilteredScalarList(const char *fileAndRunFilter,
                                    const char *moduleFilter,
                                    const char *nameFilter);

    /**
     * Form groups (IntVectors) by runRef+scalarName.
     * Then order each group by module name, and insert "null" elements (id=-1)
     * so that every group is of same length, and the same indices contain
     * the same moduleNameRefs.
     */
    IntVectorVector groupByRunAndName(const IntVector& idlist);

    /**
     * Form groups (IntVectors) by moduleName+scalarName.
     * Order each group by runRef, and insert "null" elements (id=-1) so that
     * every group is of same length, and the same indices contain the same runRef.
     */
    IntVectorVector groupByModuleAndName(const IntVector& idlist);

    /**
     * Group and align data for a scatter plot. The first vector will contain the 
     * x coordinates, further vectors the y1, y2, etc series. Points are sorted
     * by x coordinate. For missing points in y1, y2, etc, the vector contains -1. 
     */
    IntVectorVector prepareScatterPlot(const IntVector& idlist, const char *moduleName, const char *scalarName);

    /**
     * Looks at the data given by their Id, and returns a subset of them
     * where each (moduleName, scalarName) pair occurs only once.
     *
     * Purpose: these pairs should offered as selection of X axis for a
     * scatter plot.
     */
    IntVector getModuleAndNamePairs(const IntVector& idList, int maxcount);

    /**
     * Arranges the data into a tabular form suitable for pasting into a spreadsheet:
     * one dimension is module and scalar name, the other is file+run number.
     */
    IntVectorVector prepareCopyToClipboard(const IntVector& idlist);
};

#endif


