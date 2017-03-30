//=========================================================================
//  IDLIST.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga, Tamas Borbely
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_SCAVE_IDLIST_H
#define __OMNETPP_SCAVE_IDLIST_H

#include <vector>
#include <exception>
#include <stdexcept>
#include "scavedefs.h" // int64_t

namespace omnetpp {
namespace scave {

class ResultFileManager;

/**
 * Result ID -- identifies a scalar or a vector in a ResultFileManager
 */
typedef int64_t ID;


/**
 * Stores a set of unique IDs. Order is not important, and may occasionally
 * change (after merge(), subtract() or intersect()).
 *
 * Beware: Copy ctor implements transfer-of-ownership semantics!
 */
class SCAVE_API IDList
{
    private:
        friend class ResultFileManager;
        typedef std::vector<ID> V;
        V *v;

        void operator=(const IDList&); // undefined, to prevent calling it
        void checkV() const {if (!v) throw std::runtime_error("this is a zombie IDList");}
        void uncheckedAdd(ID id) {v->push_back(id);} // doesn't check if already in there
        void discardDuplicates();
        void checkIntegrity(ResultFileManager *mgr) const;
        void checkIntegrityAllScalars(ResultFileManager *mgr) const;
        void checkIntegrityAllVectors(ResultFileManager *mgr) const;
        void checkIntegrityAllHistograms(ResultFileManager *mgr) const;

        template <class T> void sortBy(ResultFileManager *mgr, bool ascending, T& comparator);
        template <class T> void sortScalarsBy(ResultFileManager *mgr, bool ascending, T& comparator);
        template <class T> void sortVectorsBy(ResultFileManager *mgr, bool ascending, T& comparator);
        template <class T> void sortHistogramsBy(ResultFileManager *mgr, bool ascending, T& comparator);

    public:
        IDList()  {v = new V;}
        IDList(unsigned int sz)  {v = new V(sz);}
        IDList(const IDList& ids); // transfer of ownership semantics!
        ~IDList()  {delete v;}
        int size() const  {checkV(); return (int)v->size();}
        bool isEmpty() const  {checkV(); return v->empty();}
        void clear()  {checkV(); v->clear();}
        void set(const IDList& ids);
        void add(ID x);
        ID get(int i) const {checkV(); return v->at(i);} // at() includes bounds check
        // void set(int i, ID x);
        void erase(int i);
        void subtract(ID x); // this -= {x}
        int indexOf(ID x) const;
        void merge(IDList& ids);  // this += ids
        void subtract(IDList& ids);  // this -= ids
        void intersect(IDList& ids);  // this = intersection(this,ids)
        IDList getSubsetByIndices(int *array, int n) const;
        IDList dup() const;
        int getItemTypes() const;  // SCALAR, VECTOR or their binary OR
        bool areAllScalars() const;
        bool areAllVectors() const;
        bool areAllStatistics() const;
        bool areAllHistograms() const;

        // support for range-based for loops
        V::const_iterator begin() const {return v->begin();}
        V::const_iterator end() const {return v->end();}

        // filtering
        int countByTypes(int typeMask) const;
        IDList filterByTypes(int typeMask) const;

        // sorting
        // TODO: there's a duplication between vector and histogram sorting due to not having a proper superclass with the statistics inside
        void sortByFileAndRun(ResultFileManager *mgr, bool ascending);
        void sortByRunAndFile(ResultFileManager *mgr, bool ascending);
        void sortByDirectory(ResultFileManager *mgr, bool ascending);
        void sortByFileName(ResultFileManager *mgr, bool ascending);
        void sortByRun(ResultFileManager *mgr, bool ascending);
        void sortByModule(ResultFileManager *mgr, bool ascending);
        void sortByName(ResultFileManager *mgr, bool ascending);
        void sortScalarsByValue(ResultFileManager *mgr, bool ascending);
        void sortVectorsByVectorId(ResultFileManager *mgr, bool ascending);
        void sortVectorsByLength(ResultFileManager *mgr, bool ascending);
        void sortVectorsByMean(ResultFileManager *mgr, bool ascending);
        void sortVectorsByStdDev(ResultFileManager *mgr, bool ascending);
        void sortVectorsByMin(ResultFileManager *mgr, bool ascending);
        void sortVectorsByMax(ResultFileManager *mgr, bool ascending);
        void sortVectorsByVariance(ResultFileManager *mgr, bool ascending);
        void sortVectorsByStartTime(ResultFileManager *mgr, bool ascending);
        void sortVectorsByEndTime(ResultFileManager *mgr, bool ascending);
        void sortHistogramsByLength(ResultFileManager *mgr, bool ascending);
        void sortHistogramsByMean(ResultFileManager *mgr, bool ascending);
        void sortHistogramsByStdDev(ResultFileManager *mgr, bool ascending);
        void sortHistogramsByMin(ResultFileManager *mgr, bool ascending);
        void sortHistogramsByMax(ResultFileManager *mgr, bool ascending);
        void sortHistogramsByVariance(ResultFileManager *mgr, bool ascending);
        void sortByRunAttribute(ResultFileManager *mgr, const char* runAttr, bool ascending);
        void reverse();
        void toByteArray(char *array, int n) const;
        void fromByteArray(char *array, int n);
};

} // namespace scave
}  // namespace omnetpp


#endif


