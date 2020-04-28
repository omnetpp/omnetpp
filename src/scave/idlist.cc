//=========================================================================
//  IDLIST.CC - part of
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

// turn off 'Deprecated std::copy()' warning (MSVC80)
#ifdef _MSC_VER
#pragma warning(disable:4996)
#endif

#include <cstdlib>
#include <algorithm>
#include <functional>
#include "common/stringutil.h"
#include "idlist.h"
#include "interruptedflag.h"
#include "resultfilemanager.h"
#include "scaveutils.h"

#ifdef THREADED
#include "common/rwlock.h"
#define READER_MUTEX    Mutex __reader_mutex_(mgr->getReadLock());
#else
#define READER_MUTEX
#endif

using namespace omnetpp::common;

namespace omnetpp {
namespace scave {

void IDList::discardDuplicates()
{
    sort(v);
    auto last = std::unique(v.begin(), v.end());
    v.erase(last, v.end());
}

int IDList::indexOf(ID x) const
{
    auto it = std::find(v.begin(), v.end(), x);
    if (it != v.end())
        return (int)(it - v.begin());
    else
        return -1;
}

IDList IDList::unionWith(IDList& ids) const
{
    // sort both vectors so that we can apply set_union
    sort(v);
    sort(ids.v);

    // allocate a new vector, and merge the two vectors into it
    IDList result;
    V& v2 = result.v;
    v2.resize(v.size() + ids.v.size());
    auto v2end = std::set_union(v.begin(), v.end(), ids.v.begin(), ids.v.end(), v2.begin());
    v2.resize(v2end - v2.begin());
    return result;
}

IDList IDList::subtract(IDList& ids) const
{
    // sort both vectors so that we can apply set_difference
    sort(v);
    sort(ids.v);

    // allocate a new vector, and compute difference into it
    IDList result;
    V& v2 = result.v;
    v2.resize(v.size());
    auto v2end = std::set_difference(v.begin(), v.end(), ids.v.begin(), ids.v.end(), v2.begin());
    v2.resize(v2end - v2.begin());
    return result;
}

IDList IDList::intersect(IDList& ids) const
{
    // sort both vectors so that we can apply set_intersect
    sort(v);
    sort(ids.v);

    // allocate a new vector, and compute intersection into it
    IDList result;
    V& v2 = result.v;
    v2.resize(v.size());
    auto v2end = std::set_intersection(v.begin(), v.end(), ids.v.begin(), ids.v.end(), v2.begin());
    v2.resize(v2end - v2.begin());
    return result;
}

bool IDList::isSubsetOf(IDList& ids)
{
    // sort both vectors so that we can apply std::includes()
    sort(v);
    sort(ids.v);

    return std::includes(ids.v.begin(), ids.v.end(), v.begin(), v.end()); // "ids includes this"
}

IDList IDList::getRange(int startIndex, int endIndex) const
{
    if (startIndex < 0 || endIndex > v.size() || startIndex > endIndex)
        throw opp_runtime_error("Bad start or end index");
    IDList newList;
    newList.v.resize(endIndex-startIndex);
    for (int i = startIndex; i < endIndex; i++)
        newList.v[i-startIndex] = v[i];
    return newList;
}

IDList IDList::getSubsetByIndices(int *indices, int n) const
{
    IDList newList;
    for (int i = 0; i < n; i++)
        newList.v.push_back(v.at(indices[i]));
    return newList;
}

void IDList::checkIntegrity(ResultFileManager *mgr) const
{
    ScalarResult buffer;
    for (ID id : v)
        mgr->getItem(id, buffer);  // this will throw exception if id is not valid
}

void IDList::checkIntegrityAllScalars(ResultFileManager *mgr) const
{
    checkIntegrity(mgr);
    if (!areAllScalars())
        throw opp_runtime_error("These items are not all scalars");
}

void IDList::checkIntegrityAllParameters(ResultFileManager *mgr) const
{
    checkIntegrity(mgr);
    if (!areAllParameters())
        throw opp_runtime_error("These items are not all parameters");
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

inline void check(InterruptedFlag *interrupted) {if (interrupted->flag) throw InterruptedException();}

template <typename T>
void IDList::doSort(const std::function<T(ID)>& getter, ResultFileManager *mgr, bool ascending, InterruptedFlag *intrpt)
{
    READER_MUTEX
    size_t n = v.size();
    std::vector<std::pair<T,ID>> a(n);
    for (int i = 0; i < n; i++)
        a[i] = std::make_pair(getter(v[i]), v[i]);

    if (ascending)
        std::stable_sort(a.begin(), a.end(), [intrpt](const auto& lhs, const auto& rhs) {check(intrpt); return lhs.first < rhs.first;});
    else
        std::stable_sort(a.begin(), a.end(), [intrpt](const auto& lhs, const auto& rhs) {check(intrpt); return lhs.first > rhs.first;});

    for (int i = 0; i < n; i++)
        v[i] = a[i].second;
}


template <>
void IDList::doSort<const char *>(const std::function<const char *(ID)>& getter, ResultFileManager *mgr, bool ascending, InterruptedFlag *intrpt)
{
    READER_MUTEX
    size_t n = v.size();
    std::vector<std::pair<const char *,ID>> a(n);
    for (int i = 0; i < n; i++)
        a[i] = std::make_pair(getter(v[i]), v[i]);

    // sorting. note: in debug mode, strdictcmp() is significantly slower than str(case)cmp(), but in release mode the difference is smaller
    if (ascending)
        std::stable_sort(a.begin(), a.end(), [intrpt](const auto& lhs, const auto& rhs) {check(intrpt); return strdictcmp(lhs.first, rhs.first) < 0;});
    else
        std::stable_sort(a.begin(), a.end(), [intrpt](const auto& lhs, const auto& rhs) {check(intrpt); return strdictcmp(lhs.first, rhs.first) > 0;});

    for (int i = 0; i < n; i++)
        v[i] = a[i].second;
}

void IDList::sortByFilePath(ResultFileManager *mgr, bool ascending, InterruptedFlag *interrupted)
{
    doSort<const char *>([mgr](ID id) {return mgr->getFileRun(id)->getFile()->getFilePath().c_str();}, mgr, ascending, interrupted);
}

void IDList::sortByDirectory(ResultFileManager *mgr, bool ascending, InterruptedFlag *interrupted)
{
    doSort<const char *>([mgr](ID id) {return mgr->getFileRun(id)->getFile()->getDirectory().c_str();}, mgr, ascending, interrupted);
}

void IDList::sortByFileName(ResultFileManager *mgr, bool ascending, InterruptedFlag *interrupted)
{
    doSort<const char *>([mgr](ID id) {return mgr->getFileRun(id)->getFile()->getFileName().c_str();}, mgr, ascending, interrupted);
}

void IDList::sortByRun(ResultFileManager *mgr, bool ascending, InterruptedFlag *interrupted)
{
    doSort<const char *>([mgr](ID id) {return mgr->getFileRun(id)->getRun()->getRunName().c_str();}, mgr, ascending, interrupted);
}

void IDList::sortByRunAttribute(ResultFileManager *mgr, const char *attrName, bool ascending, InterruptedFlag *interrupted)
{
    doSort<const char *>([mgr,attrName](ID id) {return mgr->getFileRun(id)->getRun()->getAttribute(attrName).c_str();}, mgr, ascending, interrupted);
}

void IDList::sortByRunIterationVariable(ResultFileManager *mgr, const char *itervarName, bool ascending, InterruptedFlag *interrupted)
{
    doSort<const char *>([mgr,itervarName](ID id) {return mgr->getFileRun(id)->getRun()->getIterationVariable(itervarName).c_str();}, mgr, ascending, interrupted);
}

void IDList::sortByRunConfigValue(ResultFileManager *mgr, const char *configKey, bool ascending, InterruptedFlag *interrupted)
{
    doSort<const char *>([mgr,configKey](ID id) {return mgr->getFileRun(id)->getRun()->getConfigValue(configKey).c_str();}, mgr, ascending, interrupted);
}

void IDList::sortByRunParamValue(ResultFileManager *mgr, const char *paramFullPath, bool ascending, InterruptedFlag *interrupted)
{
    doSort<const char *>([mgr,paramFullPath](ID id) {return mgr->getFileRun(id)->getRun()->getParamAssignment(paramFullPath).c_str();}, mgr, ascending, interrupted);
}

void IDList::sortByModule(ResultFileManager *mgr, bool ascending, InterruptedFlag *interrupted)
{
    ScalarResult tmp;
    doSort<const char *>([mgr,&tmp](ID id) {return mgr->uncheckedGetItem(id, tmp)->getModuleName().c_str();}, mgr, ascending, interrupted);
}

void IDList::sortByName(ResultFileManager *mgr, bool ascending, InterruptedFlag *interrupted)
{
    ScalarResult tmp;
    doSort<const char *>([mgr,&tmp](ID id) {return mgr->uncheckedGetItem(id, tmp)->getName().c_str();}, mgr, ascending, interrupted);
}

void IDList::sortScalarsByValue(ResultFileManager *mgr, bool ascending, InterruptedFlag *interrupted)
{
    ScalarResult tmp;
    doSort<double>([mgr,&tmp](ID id) {return mgr->uncheckedGetScalar(id, tmp)->getValue();}, mgr, ascending, interrupted);
}

void IDList::sortParametersByValue(ResultFileManager *mgr, bool ascending, InterruptedFlag *interrupted)
{
    doSort<const char *>([mgr](ID id) {return mgr->uncheckedGetParameter(id)->getValue().c_str();}, mgr, ascending, interrupted);
}

void IDList::sortVectorsByVectorId(ResultFileManager *mgr, bool ascending, InterruptedFlag *interrupted)
{
    doSort<int>([mgr](ID id) {return mgr->uncheckedGetVector(id)->getVectorId();}, mgr, ascending, interrupted);
}

void IDList::sortVectorsByLength(ResultFileManager *mgr, bool ascending, InterruptedFlag *interrupted)
{
    doSort<int64_t>([mgr](ID id) {return mgr->uncheckedGetVector(id)->getStatistics().getCount();}, mgr, ascending, interrupted);
}

void IDList::sortVectorsByMean(ResultFileManager *mgr, bool ascending, InterruptedFlag *interrupted)
{
    doSort<double>([mgr](ID id) {return mgr->uncheckedGetVector(id)->getStatistics().getMean();}, mgr, ascending, interrupted);
}

void IDList::sortVectorsByStdDev(ResultFileManager *mgr, bool ascending, InterruptedFlag *interrupted)
{
    doSort<double>([mgr](ID id) {return mgr->uncheckedGetVector(id)->getStatistics().getStddev();}, mgr, ascending, interrupted);
}

void IDList::sortVectorsByMin(ResultFileManager *mgr, bool ascending, InterruptedFlag *interrupted)
{
    doSort<double>([mgr](ID id) {return mgr->uncheckedGetVector(id)->getStatistics().getMin();}, mgr, ascending, interrupted);
}

void IDList::sortVectorsByMax(ResultFileManager *mgr, bool ascending, InterruptedFlag *interrupted)
{
    doSort<double>([mgr](ID id) {return mgr->uncheckedGetVector(id)->getStatistics().getMax();}, mgr, ascending, interrupted);
}

void IDList::sortVectorsByVariance(ResultFileManager *mgr, bool ascending, InterruptedFlag *interrupted)
{
    doSort<double>([mgr](ID id) {return mgr->uncheckedGetVector(id)->getStatistics().getVariance();}, mgr, ascending, interrupted);
}

void IDList::sortVectorsByStartTime(ResultFileManager *mgr, bool ascending, InterruptedFlag *interrupted)
{
    doSort<simultime_t>([mgr](ID id) {return mgr->uncheckedGetVector(id)->getStartTime();}, mgr, ascending, interrupted);
}

void IDList::sortVectorsByEndTime(ResultFileManager *mgr, bool ascending, InterruptedFlag *interrupted)
{
    doSort<simultime_t>([mgr](ID id) {return mgr->uncheckedGetVector(id)->getEndTime();}, mgr, ascending, interrupted);
}

void IDList::sortHistogramsByLength(ResultFileManager *mgr, bool ascending, InterruptedFlag *interrupted)
{
    doSort<int64_t>([mgr](ID id) {return mgr->uncheckedGetHistogram(id)->getStatistics().getCount();}, mgr, ascending, interrupted);
}

void IDList::sortHistogramsByMean(ResultFileManager *mgr, bool ascending, InterruptedFlag *interrupted)
{
    doSort<double>([mgr](ID id) {return mgr->uncheckedGetHistogram(id)->getStatistics().getMean();}, mgr, ascending, interrupted);
}

void IDList::sortHistogramsByStdDev(ResultFileManager *mgr, bool ascending, InterruptedFlag *interrupted)
{
    doSort<double>([mgr](ID id) {return mgr->uncheckedGetHistogram(id)->getStatistics().getStddev();}, mgr, ascending, interrupted);
}

void IDList::sortHistogramsByMin(ResultFileManager *mgr, bool ascending, InterruptedFlag *interrupted)
{
    doSort<double>([mgr](ID id) {return mgr->uncheckedGetHistogram(id)->getStatistics().getMin();}, mgr, ascending, interrupted);
}

void IDList::sortHistogramsByMax(ResultFileManager *mgr, bool ascending, InterruptedFlag *interrupted)
{
    doSort<double>([mgr](ID id) {return mgr->uncheckedGetHistogram(id)->getStatistics().getMax();}, mgr, ascending, interrupted);
}

void IDList::sortHistogramsByVariance(ResultFileManager *mgr, bool ascending, InterruptedFlag *interrupted)
{
    doSort<double>([mgr](ID id) {return mgr->uncheckedGetHistogram(id)->getStatistics().getVariance();}, mgr, ascending, interrupted);
}

void IDList::reverse()
{
    std::reverse(v.begin(), v.end());
}

int IDList::getItemTypes() const
{
    int types = 0;
    for (ID id : v)
        types |= ResultFileManager::_type(id);
    return types;
}

bool IDList::areAllScalars() const
{
    int types = getItemTypes();
    return !types || types == ResultFileManager::SCALAR;
}

bool IDList::areAllParameters() const
{
    int types = getItemTypes();
    return !types || types == ResultFileManager::PARAMETER;
}

bool IDList::areAllVectors() const
{
    int types = getItemTypes();
    return !types || types == ResultFileManager::VECTOR;
}

bool IDList::areAllStatistics() const
{
    int types = getItemTypes();
    return !types || types == ResultFileManager::STATISTICS;
}

bool IDList::areAllHistograms() const
{
    int types = getItemTypes();
    return !types || types == ResultFileManager::HISTOGRAM;
}

IDList IDList::filterByTypes(int typeMask) const
{
    IDList result;
    for (ID id : v)
        if ((ResultFileManager::_type(id) & typeMask) != 0)
            result.v.push_back(id);

    return result;
}

int IDList::countByTypes(int typeMask) const
{
    int count = 0;
    for (ID id : v)
        if ((ResultFileManager::_type(id) & typeMask) != 0)
            count++;
    return count;
}

bool IDList::equals(IDList& other)
{
    if (v.size() != other.v.size())
        return false;
    if (v == other.v)
        return true;
    std::sort(v.begin(), v.end());
    std::sort(other.v.begin(), other.v.end());
    return v == other.v;
}

int64_t IDList::hashCode64() const
{
    int64_t x = size();
    for (ID id : v)
        x = x*31 + id;
    return x;
}

}  // namespace scave
}  // namespace omnetpp

