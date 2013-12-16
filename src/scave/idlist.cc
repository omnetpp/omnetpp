//=========================================================================
//  IDLIST.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga, Tamas Borbely
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

// turn off 'Deprecated std::copy()' warning (MSVC80)
#ifdef _MSC_VER
#pragma warning(disable:4996)
#endif

#include <stdlib.h>
#include <algorithm>
#include <functional>
#include "idlist.h"
#include "resultfilemanager.h"
#include "stringutil.h"
#include "scaveutils.h"

#ifdef THREADED
#include "rwlock.h"
#define READER_MUTEX Mutex __reader_mutex_(mgr->getReadLock());
#else
#define READER_MUTEX
#endif

NAMESPACE_BEGIN

IDList::IDList(const IDList& ids)
{
    ids.checkV();
    v = ids.v;
    const_cast<IDList&>(ids).v = NULL;
}

void IDList::set(const IDList& ids)
{
    ids.checkV();
    delete v;
    v = new V(ids.v->size());
    *v = *ids.v;  // copy contents
}

void IDList::add(ID x)
{
    checkV();
    if (std::find(v->begin(), v->end(), x)==v->end())
        v->push_back(x);
}

/* XXX it is not used, and bogus anyway
void IDList::set(int i, ID x)
{
    checkV();
    V::iterator it = std::find(v->begin(), v->end(), x);
    if (it==v->end())
        v->at(i) = x;  // at() includes bounds check
    else
        v->erase(it); // x is already in there -- just delete the element it replaces
}
*/

void IDList::erase(int i)
{
    checkV();
    v->at(i);  // bounds check
    v->erase(v->begin()+i);
}

int IDList::indexOf(ID x) const
{
    checkV();
    V::iterator it = std::find(v->begin(), v->end(), x);
    if (it != v->end())
        return (int)(it - v->begin());
    else
        return -1;
}

void IDList::subtract(ID x)
{
    checkV();
    V::iterator it = std::find(v->begin(), v->end(), x);
    if (it!=v->end())
        v->erase(it);
}

void IDList::merge(IDList& ids)
{
    checkV();
    ids.checkV();

    // sort both vectors so that we can apply set_union
    std::sort(v->begin(), v->end());
    std::sort(ids.v->begin(), ids.v->end());

    // allocate a new vector, and merge the two vectors into it
    V *v2 = new V;
    v2->resize(v->size() + ids.v->size());
    V::iterator v2end = std::set_union(v->begin(), v->end(), ids.v->begin(), ids.v->end(), v2->begin());
    v2->resize(v2end - v2->begin());

    // replace the vector with the result
    delete v;
    v = v2;
}

void IDList::subtract(IDList& ids)
{
    checkV();
    ids.checkV();

    // sort both vectors so that we can apply set_difference
    std::sort(v->begin(), v->end());
    std::sort(ids.v->begin(), ids.v->end());

    // allocate a new vector, and compute difference into it
    V *v2 = new V;
    v2->resize(v->size());
    V::iterator v2end = std::set_difference(v->begin(), v->end(), ids.v->begin(), ids.v->end(), v2->begin());
    v2->resize(v2end - v2->begin());

    // replace the vector with the result
    delete v;
    v = v2;
}

void IDList::intersect(IDList& ids)
{
    checkV();
    ids.checkV();

    // sort both vectors so that we can apply set_intersect
    std::sort(v->begin(), v->end());
    std::sort(ids.v->begin(), ids.v->end());

    // allocate a new vector, and compute intersection into it
    V *v2 = new V;
    v2->resize(v->size());
    V::iterator v2end = std::set_intersection(v->begin(), v->end(), ids.v->begin(), ids.v->end(), v2->begin());
    v2->resize(v2end - v2->begin());

    // replace the vector with the result
    delete v;
    v = v2;
}

IDList IDList::getSubsetByIndices(int *indices, int n) const
{
    checkV();
    IDList newList;
    for (int i=0; i<n; i++)
        newList.v->push_back(v->at(indices[i]));
    return newList;
}

IDList IDList::dup() const
{
    IDList newList;
    newList.set(*this);
    return newList;
}

void IDList::checkIntegrity(ResultFileManager *mgr) const
{
    checkV();
    for (V::const_iterator i=v->begin(); i!=v->end(); ++i)
        mgr->getItem(*i); // this will thow exception if id is not valid
}

void IDList::checkIntegrityAllScalars(ResultFileManager *mgr) const
{
    checkIntegrity(mgr);
    if (!areAllScalars())
        throw opp_runtime_error("These items are not all scalars");
}

void IDList::checkIntegrityAllVectors(ResultFileManager *mgr) const
{
    checkIntegrity(mgr);
    if (!areAllVectors())
        throw opp_runtime_error("These items are not all vectors");
}

void IDList::checkIntegrityAllHistograms(ResultFileManager *mgr) const
{
    checkIntegrity(mgr);
    if (!areAllHistograms())
        throw opp_runtime_error("These items are not all histograms");
}

class CmpBase : public std::binary_function<ID, ID, bool> {
    protected:
       ResultFileManager *mgr;
       bool less(const std::string& a, const std::string& b)
          {return strdictcmp(a.c_str(), b.c_str()) < 0;}
       const ResultItem& uncheckedGetItem(ID id) const { return mgr->uncheckedGetItem(id); }
       const ScalarResult& uncheckedGetScalar(ID id) const { return mgr->uncheckedGetScalar(id); }
       const VectorResult& uncheckedGetVector(ID id) const { return mgr->uncheckedGetVector(id); }
       const HistogramResult& uncheckedGetHistogram(ID id) const { return mgr->uncheckedGetHistogram(id); }
    public:
        CmpBase(ResultFileManager *m) {mgr = m;}
};

class FileAndRunLess : public CmpBase {
    public:
        FileAndRunLess(ResultFileManager *m) : CmpBase(m) {}
        bool operator()(ID a, ID b) { // implements operator<
            const FileRun *da = uncheckedGetItem(a).fileRunRef;
            const FileRun *db = uncheckedGetItem(b).fileRunRef;
            if (da==db)
                return false;
            else if (da->fileRef==db->fileRef)
                return da->runRef->runName < db->runRef->runName;
            else
                return da->fileRef->filePath < db->fileRef->filePath;
        }
};

class RunAndFileLess : public CmpBase {
    public:
        RunAndFileLess(ResultFileManager *m) : CmpBase(m) {}
        bool operator()(ID a, ID b) { // implements operator<
            const FileRun *da = uncheckedGetItem(a).fileRunRef;
            const FileRun *db = uncheckedGetItem(b).fileRunRef;
            if (da==db)
                return false;
            else if (da->runRef==db->runRef)
                return da->fileRef->filePath < db->fileRef->filePath;
            else
                return da->runRef->runName < db->runRef->runName;
        }
};

class RunAttributeLess : public CmpBase {
    private:
        const char* attrName;
    public:
        RunAttributeLess(ResultFileManager* m, const char* attrName)
            : CmpBase(m), attrName(attrName) {}
        bool operator()(ID a, ID b) {
            const char* aValue = uncheckedGetItem(a).fileRunRef->runRef->getAttribute(attrName);
            const char* bValue = uncheckedGetItem(b).fileRunRef->runRef->getAttribute(attrName);
            return ((aValue && bValue) ? less(aValue, bValue) : aValue!=NULL);
        }
};

#define CMP(clazz,method) class clazz : public CmpBase { \
    public: \
        clazz(ResultFileManager *m) : CmpBase(m) {} \
        bool operator()(const ID a, const ID b) {return method;} \
    };

CMP(DirectoryLess, less(uncheckedGetItem(a).fileRunRef->fileRef->directory, uncheckedGetItem(b).fileRunRef->fileRef->directory))
CMP(FileNameLess, less(uncheckedGetItem(a).fileRunRef->fileRef->fileName, uncheckedGetItem(b).fileRunRef->fileRef->fileName))
CMP(RunLess, less(uncheckedGetItem(a).fileRunRef->runRef->runName, uncheckedGetItem(b).fileRunRef->runRef->runName))
CMP(ModuleLess, less(*(uncheckedGetItem(a).moduleNameRef), *(uncheckedGetItem(b).moduleNameRef)))
CMP(NameLess, less(*(uncheckedGetItem(a).nameRef), *(uncheckedGetItem(b).nameRef)))
CMP(ValueLess, uncheckedGetScalar(a).value < uncheckedGetScalar(b).value)
CMP(VectorIdLess, uncheckedGetVector(a).vectorId < uncheckedGetVector(b).vectorId)
CMP(VectorCountLess, uncheckedGetVector(a).stat.getCount() < uncheckedGetVector(b).stat.getCount())
CMP(VectorMeanLess, uncheckedGetVector(a).stat.getMean() < uncheckedGetVector(b).stat.getMean())
CMP(VectorStddevLess, uncheckedGetVector(a).stat.getStddev() < uncheckedGetVector(b).stat.getStddev())
CMP(VectorMinLess, uncheckedGetVector(a).stat.getMin() < uncheckedGetVector(b).stat.getMin())
CMP(VectorMaxLess, uncheckedGetVector(a).stat.getMax() < uncheckedGetVector(b).stat.getMax())
CMP(VectorVarianceLess, uncheckedGetVector(a).stat.getVariance() < uncheckedGetVector(b).stat.getVariance())
CMP(StartTimeLess, uncheckedGetVector(a).startTime < uncheckedGetVector(b).startTime)
CMP(EndTimeLess, uncheckedGetVector(a).endTime < uncheckedGetVector(b).endTime)
CMP(HistogramCountLess, uncheckedGetHistogram(a).stat.getCount() < uncheckedGetHistogram(b).stat.getCount())
CMP(HistogramMeanLess, uncheckedGetHistogram(a).stat.getMean() < uncheckedGetHistogram(b).stat.getMean())
CMP(HistogramStddevLess, uncheckedGetHistogram(a).stat.getStddev() < uncheckedGetHistogram(b).stat.getStddev())
CMP(HistogramMinLess, uncheckedGetHistogram(a).stat.getMin() < uncheckedGetHistogram(b).stat.getMin())
CMP(HistogramMaxLess, uncheckedGetHistogram(a).stat.getMax() < uncheckedGetHistogram(b).stat.getMax())
CMP(HistogramVarianceLess, uncheckedGetHistogram(a).stat.getVariance() < uncheckedGetHistogram(b).stat.getVariance())

template <class T>
void IDList::sortBy(ResultFileManager *mgr, bool ascending, T& comparator)
{
    READER_MUTEX
    checkIntegrity(mgr);
    // optimization: maybe it's sorted the other way round, so we reverse it to speed up sorting
    if (v->size()>=2 && comparator(v->at(0), v->at(v->size()-1))!=ascending)
       reverse();
    if (ascending)
        std::sort(v->begin(), v->end(), comparator);
    else
        std::sort(v->begin(), v->end(), flipArgs(comparator));
}

template <class T>
void IDList::sortScalarsBy(ResultFileManager *mgr, bool ascending, T& comparator)
{
    READER_MUTEX
    checkIntegrityAllScalars(mgr);
    // optimization: maybe it's sorted the other way round, so we reverse it to speed up sorting
    if (v->size()>=2 && comparator(v->at(0), v->at(v->size()-1))!=ascending)
       reverse();
    if (ascending)
        std::sort(v->begin(), v->end(), comparator);
    else
        std::sort(v->begin(), v->end(), flipArgs(comparator));
}

template <class T>
void IDList::sortVectorsBy(ResultFileManager *mgr, bool ascending, T& comparator)
{
    READER_MUTEX
    checkIntegrityAllVectors(mgr);
    // optimization: maybe it's sorted the other way round, so we reverse it to speed up sorting
    if (v->size()>=2 && comparator(v->at(0), v->at(v->size()-1))!=ascending)
       reverse();
    if (ascending)
        std::sort(v->begin(), v->end(), comparator);
    else
        std::sort(v->begin(), v->end(), flipArgs(comparator));
}

template <class T>
void IDList::sortHistogramsBy(ResultFileManager *mgr, bool ascending, T& comparator)
{
    READER_MUTEX
    checkIntegrityAllHistograms(mgr);
    // optimization: maybe it's sorted the other way round, so we reverse it to speed up sorting
    if (v->size()>=2 && comparator(v->at(0), v->at(v->size()-1))!=ascending)
       reverse();
    if (ascending)
        std::sort(v->begin(), v->end(), comparator);
    else
        std::sort(v->begin(), v->end(), flipArgs(comparator));
}


void IDList::sortByFileAndRun(ResultFileManager *mgr, bool ascending)
{
    FileAndRunLess compare(mgr);
    sortBy(mgr, ascending, compare);
}

void IDList::sortByRunAndFile(ResultFileManager *mgr, bool ascending)
{
    RunAndFileLess compare(mgr);
    sortBy(mgr, ascending, compare);
}

void IDList::sortByDirectory(ResultFileManager *mgr, bool ascending)
{
    DirectoryLess compare(mgr);
    sortBy(mgr, ascending, compare);
}

void IDList::sortByFileName(ResultFileManager *mgr, bool ascending)
{
    FileNameLess compare(mgr);
    sortBy(mgr, ascending, compare);
}

void IDList::sortByRun(ResultFileManager *mgr, bool ascending)
{
    RunLess compare(mgr);
    sortBy(mgr, ascending, compare);
}

void IDList::sortByModule(ResultFileManager *mgr, bool ascending)
{
    ModuleLess compare(mgr);
    sortBy(mgr, ascending, compare);
}

void IDList::sortByName(ResultFileManager *mgr, bool ascending)
{
    NameLess compare(mgr);
    sortBy(mgr, ascending, compare);
}

void IDList::sortScalarsByValue(ResultFileManager *mgr, bool ascending)
{
    ValueLess compare(mgr);
    sortScalarsBy(mgr, ascending, compare);
}

void IDList::sortVectorsByVectorId(ResultFileManager *mgr, bool ascending)
{
    VectorIdLess compare(mgr);
    sortVectorsBy(mgr, ascending, compare);
}

void IDList::sortVectorsByLength(ResultFileManager *mgr, bool ascending)
{
    VectorCountLess compare(mgr);
    sortVectorsBy(mgr, ascending, compare);
}

void IDList::sortVectorsByMean(ResultFileManager *mgr, bool ascending)
{
    VectorMeanLess compare(mgr);
    sortVectorsBy(mgr, ascending, compare);
}

void IDList::sortVectorsByStdDev(ResultFileManager *mgr, bool ascending)
{
    VectorStddevLess compare(mgr);
    sortVectorsBy(mgr, ascending, compare);
}

void IDList::sortVectorsByMin(ResultFileManager *mgr, bool ascending)
{
    VectorMinLess compare(mgr);
    sortVectorsBy(mgr, ascending, compare);
}

void IDList::sortVectorsByMax(ResultFileManager *mgr, bool ascending)
{
    VectorMaxLess compare(mgr);
    sortVectorsBy(mgr, ascending, compare);
}

void IDList::sortVectorsByVariance(ResultFileManager *mgr, bool ascending)
{
    VectorVarianceLess compare(mgr);
    sortVectorsBy(mgr, ascending, compare);
}

void IDList::sortVectorsByStartTime(ResultFileManager *mgr, bool ascending)
{
    StartTimeLess compare(mgr);
    sortVectorsBy(mgr, ascending, compare);
}

void IDList::sortVectorsByEndTime(ResultFileManager *mgr, bool ascending)
{
    EndTimeLess compare(mgr);
    sortVectorsBy(mgr, ascending, compare);
}

void IDList::sortHistogramsByLength(ResultFileManager *mgr, bool ascending)
{
    HistogramCountLess compare(mgr);
    sortHistogramsBy(mgr, ascending, compare);
}

void IDList::sortHistogramsByMean(ResultFileManager *mgr, bool ascending)
{
    HistogramMeanLess compare(mgr);
    sortHistogramsBy(mgr, ascending, compare);
}

void IDList::sortHistogramsByStdDev(ResultFileManager *mgr, bool ascending)
{
    HistogramStddevLess compare(mgr);
    sortHistogramsBy(mgr, ascending, compare);
}

void IDList::sortHistogramsByMin(ResultFileManager *mgr, bool ascending)
{
    HistogramMinLess compare(mgr);
    sortHistogramsBy(mgr, ascending, compare);
}

void IDList::sortHistogramsByMax(ResultFileManager *mgr, bool ascending)
{
    HistogramMaxLess compare(mgr);
    sortHistogramsBy(mgr, ascending, compare);
}

void IDList::sortHistogramsByVariance(ResultFileManager *mgr, bool ascending)
{
    HistogramVarianceLess compare(mgr);
    sortHistogramsBy(mgr, ascending, compare);
}

void IDList::sortByRunAttribute(ResultFileManager *mgr, const char* runAttribute, bool ascending) {
    RunAttributeLess compare(mgr, runAttribute);
    sortBy(mgr, ascending, compare);
}

void IDList::reverse()
{
    checkV();
    std::reverse(v->begin(), v->end());
}

int IDList::getItemTypes() const
{
    checkV();
    int types = 0;
    for (V::const_iterator i=v->begin(); i!=v->end(); ++i)
        types |= ResultFileManager::_type(*i);
    return types;
}

bool IDList::areAllScalars() const
{
    int types = getItemTypes();
    return !types || types==ResultFileManager::SCALAR;
}

bool IDList::areAllVectors() const
{
    int types = getItemTypes();
    return !types || types==ResultFileManager::VECTOR;
}

bool IDList::areAllHistograms() const
{
    int types = getItemTypes();
    return !types || types==ResultFileManager::HISTOGRAM;
}

IDList IDList::filterByTypes(int typeMask) const
{
    IDList result;
    for (V::const_iterator i=v->begin(); i!=v->end(); ++i)
        if ((ResultFileManager::_type(*i) & typeMask) != 0)
            result.v->push_back(*i);
    return result;
}

void IDList::toByteArray(char *array, int n) const
{
    checkV();
    if (n != (int)v->size()*8)
        throw opp_runtime_error("byteArray is of wrong size -- must be 8*numIDs");
    std::copy(v->begin(), v->end(), (ID*)array);  //XXX VC8.0: warning C4996: 'std::_Copy_opt' was declared deprecated
}

void IDList::fromByteArray(char *array, int n)
{
    checkV();
    if (n%8 != 0)
        throw opp_runtime_error("byteArray size must be multiple of 8");
    v->resize(n/8);
    ID *a = (ID *)array;
    std::copy(a, a+n/8, v->begin());
}

NAMESPACE_END

