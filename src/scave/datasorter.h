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
#include <functional>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "resultfilemanager.h"
#include "commonutil.h"
#include "patternmatcher.h"


typedef std::vector<ID> IDVector;
typedef std::vector<IDVector> IDVectorVector;
typedef std::vector<std::string> StringVector;

class ScalarFields
{
    public:
        static const int FILE      = 0x01;
        static const int RUN       = 0x02;
        static const int MODULE    = 0x04;
        static const int NAME      = 0x08;
        static const int ALL       = FILE | RUN | MODULE | NAME;
    private:
        int fields;
    public:
        static StringVector getFieldNames();

        ScalarFields(int fields) : fields(fields) {}
        ScalarFields(int fields, int except) : fields(fields & ~except) {};
        ScalarFields(const StringVector &fieldNames);
        
        ScalarFields complement() { return ScalarFields(ALL, fields); }
        bool hasField(int field) const { return (fields & field) == field; }
        
        bool sameGroup(const ScalarResult& d1, const ScalarResult& d2);
        bool less(ID id1, ID id2, ResultFileManager *manager);
        bool equal(ID id1, ID id2, ResultFileManager *manager);
};

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
    IDVectorVector groupByFields(const IDList& idlist, ScalarFields fields);

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


