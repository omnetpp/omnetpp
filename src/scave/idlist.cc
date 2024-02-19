//=========================================================================
//  IDLIST.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
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
#define READER_MUTEX(mgr)    Mutex __reader_mutex_((mgr)->getReadLock())
#else
#define READER_MUTEX(mgr)
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
    if (startIndex < 0 || endIndex > (int)v.size() || startIndex > endIndex)
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

IDList IDList::filterByRun(Run *run) const
{
    ResultFileManager *mgr = run->getResultFileManager();
    READER_MUTEX(mgr);
    IDList result;
    for (ID id : v)
        if (mgr->getFileRun(id)->getRun() == run)
            result.v.push_back(id);

    return result;
}

void IDList::checkIntegrity(ResultFileManager *mgr) const
{
    ScalarResult buffer;
    for (ID id : v)
        mgr->getItem(id, buffer);  // this will throw exception if id is not valid
}

void IDList::assertAllParameters() const
{
    if (!areAllParameters())
        throw opp_runtime_error("These items are not all parameters");
}

void IDList::assertAllScalars() const
{
    if (!areAllScalars())
        throw opp_runtime_error("These items are not all scalars");
}

void IDList::assertAllVectors() const
{
    if (!areAllVectors())
        throw opp_runtime_error("These items are not all vectors");
}

void IDList::assertAllStatistics() const
{
    if (!areAllStatistics())
        throw opp_runtime_error("These items are not all statistics");
}

void IDList::assertAllHistograms() const
{
    if (!areAllHistograms())
        throw opp_runtime_error("These items are not all histograms");
}

void IDList::checkIntegrityAllScalars(ResultFileManager *mgr) const
{
    checkIntegrity(mgr);
    assertAllScalars();
}

void IDList::checkIntegrityAllParameters(ResultFileManager *mgr) const
{
    checkIntegrity(mgr);
    assertAllParameters();
}

void IDList::checkIntegrityAllVectors(ResultFileManager *mgr) const
{
    checkIntegrity(mgr);
    assertAllVectors();
}

void IDList::checkIntegrityAllStatistics(ResultFileManager *mgr) const
{
    checkIntegrity(mgr);
    assertAllStatistics();
}

void IDList::checkIntegrityAllHistograms(ResultFileManager *mgr) const
{
    checkIntegrity(mgr);
    assertAllHistograms();
}

inline void check(InterruptedFlag *interrupted) {if (interrupted->flag) throw InterruptedException();}

template <typename T>
void IDList::doSort(const std::function<T(ID)>& getter, ResultFileManager *mgr, bool ascending, std::vector<int>& selectionIndices, InterruptedFlag *intrpt)
{
    READER_MUTEX(mgr);

    // Sort IDs by a key provided by the getter function, also updating the list of indices in selectionIndices.
    // Strategy: we make a temporary array of <key, value(=ID)> pairs, sort that by key, then extract the IDs from it.
    // Before sorting, we mark the selected ID by setting a reserved bit in them. After sorting,
    // we rebuild a new list of selected indices by examining which IDs have their "reserved" bit set.

    int n = (int)v.size();
    std::vector<std::pair<T,ID>> a(n);
    for (int i = 0; i < n; i++)
        a[i] = std::make_pair(getter(v[i]), v[i]);

    for (int index : selectionIndices)
        if (index >= 0 && index < n)
            ResultFileManager::_setreservedbit(a[index].second); // use ID's reserved bit to store whether that ID is part of the selection or not

    if (ascending)
        std::stable_sort(a.begin(), a.end(), [intrpt](const auto& lhs, const auto& rhs) {check(intrpt); return lhs.first < rhs.first;});
    else
        std::stable_sort(a.begin(), a.end(), [intrpt](const auto& lhs, const auto& rhs) {check(intrpt); return lhs.first > rhs.first;});

    selectionIndices.clear();
    for (int i = 0; i < n; i++) {
        ID id = a[i].second;
        if (ResultFileManager::_reservedbit(id)) {
            selectionIndices.push_back(i);
            ResultFileManager::_clearreservedbit(id);
        }
        v[i] = id;
    }
}


template <>
void IDList::doSort<const char *>(const std::function<const char *(ID)>& getter, ResultFileManager *mgr, bool ascending, std::vector<int>& selectionIndices, InterruptedFlag *intrpt)
{
    READER_MUTEX(mgr);

    // This method only differs from the templated one in that it uses opp_strdicttmp() instead of op<.

    int n = (int)v.size();
    std::vector<std::pair<const char *,ID>> a(n);
    for (int i = 0; i < n; i++)
        a[i] = std::make_pair(getter(v[i]), v[i]);

    for (int index : selectionIndices)
        if (index >= 0 && index < n)
            ResultFileManager::_setreservedbit(a[index].second); // use ID's reserved bit to store whether that ID is part of the selection or not

    // sorting. note: in debug mode, opp_strdictcmp() is significantly slower than str(case)cmp(), but in release mode the difference is smaller
    if (ascending)
        std::stable_sort(a.begin(), a.end(), [intrpt](const auto& lhs, const auto& rhs) {check(intrpt); return opp_strdictcmp(lhs.first, rhs.first) < 0;});
    else
        std::stable_sort(a.begin(), a.end(), [intrpt](const auto& lhs, const auto& rhs) {check(intrpt); return opp_strdictcmp(lhs.first, rhs.first) > 0;});

    selectionIndices.clear();
    for (int i = 0; i < n; i++) {
        ID id = a[i].second;
        if (ResultFileManager::_reservedbit(id)) {
            selectionIndices.push_back(i);
            ResultFileManager::_clearreservedbit(id);
        }
        v[i] = id;
    }
}

void IDList::sortByFilePath(ResultFileManager *mgr, bool ascending, std::vector<int>& selectionIndices, InterruptedFlag *interrupted)
{
    doSort<const char *>([mgr](ID id) {return mgr->getFileRun(id)->getFile()->getFilePath().c_str();}, mgr, ascending, selectionIndices, interrupted);
}

void IDList::sortByDirectory(ResultFileManager *mgr, bool ascending, std::vector<int>& selectionIndices, InterruptedFlag *interrupted)
{
    doSort<const char *>([mgr](ID id) {return mgr->getFileRun(id)->getFile()->getDirectory().c_str();}, mgr, ascending, selectionIndices, interrupted);
}

void IDList::sortByFileName(ResultFileManager *mgr, bool ascending, std::vector<int>& selectionIndices, InterruptedFlag *interrupted)
{
    doSort<const char *>([mgr](ID id) {return mgr->getFileRun(id)->getFile()->getFileName().c_str();}, mgr, ascending, selectionIndices, interrupted);
}

void IDList::sortByRun(ResultFileManager *mgr, bool ascending, std::vector<int>& selectionIndices, InterruptedFlag *interrupted)
{
    doSort<const char *>([mgr](ID id) {return mgr->getFileRun(id)->getRun()->getRunName().c_str();}, mgr, ascending, selectionIndices, interrupted);
}

void IDList::sortByRunAttribute(ResultFileManager *mgr, const char *attrName, bool ascending, std::vector<int>& selectionIndices, InterruptedFlag *interrupted)
{
    doSort<const char *>([mgr,attrName](ID id) {return mgr->getFileRun(id)->getRun()->getAttribute(attrName).c_str();}, mgr, ascending, selectionIndices, interrupted);
}

void IDList::sortByRunIterationVariable(ResultFileManager *mgr, const char *itervarName, bool ascending, std::vector<int>& selectionIndices, InterruptedFlag *interrupted)
{
    doSort<const char *>([mgr,itervarName](ID id) {return mgr->getFileRun(id)->getRun()->getIterationVariable(itervarName).c_str();}, mgr, ascending, selectionIndices, interrupted);
}

void IDList::sortByRunConfigValue(ResultFileManager *mgr, const char *configKey, bool ascending, std::vector<int>& selectionIndices, InterruptedFlag *interrupted)
{
    doSort<const char *>([mgr,configKey](ID id) {return mgr->getFileRun(id)->getRun()->getConfigValue(configKey).c_str();}, mgr, ascending, selectionIndices, interrupted);
}

void IDList::sortByModule(ResultFileManager *mgr, bool ascending, std::vector<int>& selectionIndices, InterruptedFlag *interrupted)
{
    ScalarResult tmp;
    doSort<const char *>([mgr,&tmp](ID id) {return mgr->uncheckedGetItem(id, tmp)->getModuleName().c_str();}, mgr, ascending, selectionIndices, interrupted);
}

void IDList::sortByName(ResultFileManager *mgr, bool ascending, std::vector<int>& selectionIndices, InterruptedFlag *interrupted)
{
    ScalarResult tmp;
    doSort<const char *>([mgr,&tmp](ID id) {return mgr->uncheckedGetItem(id, tmp)->getName().c_str();}, mgr, ascending, selectionIndices, interrupted);
}

void IDList::sortScalarsByValue(ResultFileManager *mgr, bool ascending, std::vector<int>& selectionIndices, InterruptedFlag *interrupted)
{
    assertAllScalars();
    ScalarResult tmp;
    doSort<double>([mgr,&tmp](ID id) {return mgr->uncheckedGetScalar(id, tmp)->getValue();}, mgr, ascending, selectionIndices, interrupted);
}

void IDList::sortParametersByValue(ResultFileManager *mgr, bool ascending, std::vector<int>& selectionIndices, InterruptedFlag *interrupted)
{
    assertAllParameters();
    doSort<const char *>([mgr](ID id) {return mgr->uncheckedGetParameter(id)->getValue().c_str();}, mgr, ascending, selectionIndices, interrupted);
}

void IDList::sortVectorsByVectorId(ResultFileManager *mgr, bool ascending, std::vector<int>& selectionIndices, InterruptedFlag *interrupted)
{
    assertAllVectors();
    doSort<int>([mgr](ID id) {return mgr->uncheckedGetVector(id)->getVectorId();}, mgr, ascending, selectionIndices, interrupted);
}

void IDList::sortVectorsByCount(ResultFileManager *mgr, bool ascending, std::vector<int>& selectionIndices, InterruptedFlag *interrupted)
{
    assertAllVectors();
    doSort<int64_t>([mgr](ID id) {return mgr->uncheckedGetVector(id)->getStatistics().getCount();}, mgr, ascending, selectionIndices, interrupted);
}

void IDList::sortVectorsByMean(ResultFileManager *mgr, bool ascending, std::vector<int>& selectionIndices, InterruptedFlag *interrupted)
{
    assertAllVectors();
    doSort<double>([mgr](ID id) {return mgr->uncheckedGetVector(id)->getStatistics().getMean();}, mgr, ascending, selectionIndices, interrupted);
}

void IDList::sortVectorsByStdDev(ResultFileManager *mgr, bool ascending, std::vector<int>& selectionIndices, InterruptedFlag *interrupted)
{
    assertAllVectors();
    doSort<double>([mgr](ID id) {return mgr->uncheckedGetVector(id)->getStatistics().getStddev();}, mgr, ascending, selectionIndices, interrupted);
}

void IDList::sortVectorsByMin(ResultFileManager *mgr, bool ascending, std::vector<int>& selectionIndices, InterruptedFlag *interrupted)
{
    assertAllVectors();
    doSort<double>([mgr](ID id) {return mgr->uncheckedGetVector(id)->getStatistics().getMin();}, mgr, ascending, selectionIndices, interrupted);
}

void IDList::sortVectorsByMax(ResultFileManager *mgr, bool ascending, std::vector<int>& selectionIndices, InterruptedFlag *interrupted)
{
    assertAllVectors();
    doSort<double>([mgr](ID id) {return mgr->uncheckedGetVector(id)->getStatistics().getMax();}, mgr, ascending, selectionIndices, interrupted);
}

void IDList::sortVectorsByVariance(ResultFileManager *mgr, bool ascending, std::vector<int>& selectionIndices, InterruptedFlag *interrupted)
{
    assertAllVectors();
    doSort<double>([mgr](ID id) {return mgr->uncheckedGetVector(id)->getStatistics().getVariance();}, mgr, ascending, selectionIndices, interrupted);
}

void IDList::sortVectorsBySum(ResultFileManager *mgr, bool ascending, std::vector<int>& selectionIndices, InterruptedFlag *interrupted)
{
    assertAllVectors();
    doSort<double>([mgr](ID id) {return mgr->uncheckedGetVector(id)->getStatistics().getSum();}, mgr, ascending, selectionIndices, interrupted);
}

void IDList::sortVectorsBySumWeights(ResultFileManager *mgr, bool ascending, std::vector<int>& selectionIndices, InterruptedFlag *interrupted)
{
    assertAllVectors();
    doSort<double>([mgr](ID id) {return mgr->uncheckedGetVector(id)->getStatistics().getSumWeights();}, mgr, ascending, selectionIndices, interrupted);
}

void IDList::sortVectorsByStartTime(ResultFileManager *mgr, bool ascending, std::vector<int>& selectionIndices, InterruptedFlag *interrupted)
{
    assertAllVectors();
    doSort<simultime_t>([mgr](ID id) {return mgr->uncheckedGetVector(id)->getStartTime();}, mgr, ascending, selectionIndices, interrupted);
}

void IDList::sortVectorsByEndTime(ResultFileManager *mgr, bool ascending, std::vector<int>& selectionIndices, InterruptedFlag *interrupted)
{
    assertAllVectors();
    doSort<simultime_t>([mgr](ID id) {return mgr->uncheckedGetVector(id)->getEndTime();}, mgr, ascending, selectionIndices, interrupted);
}

void IDList::sortStatisticsByCount(ResultFileManager *mgr, bool ascending, std::vector<int>& selectionIndices, InterruptedFlag *interrupted)
{
    assertAllStatistics();
    doSort<int64_t>([mgr](ID id) {return mgr->uncheckedGetStatistics(id)->getStatistics().getCount();}, mgr, ascending, selectionIndices, interrupted);
}

void IDList::sortStatisticsByMean(ResultFileManager *mgr, bool ascending, std::vector<int>& selectionIndices, InterruptedFlag *interrupted)
{
    assertAllStatistics();
    doSort<double>([mgr](ID id) {return mgr->uncheckedGetStatistics(id)->getStatistics().getMean();}, mgr, ascending, selectionIndices, interrupted);
}

void IDList::sortStatisticsByStdDev(ResultFileManager *mgr, bool ascending, std::vector<int>& selectionIndices, InterruptedFlag *interrupted)
{
    assertAllStatistics();
    doSort<double>([mgr](ID id) {return mgr->uncheckedGetStatistics(id)->getStatistics().getStddev();}, mgr, ascending, selectionIndices, interrupted);
}

void IDList::sortStatisticsByMin(ResultFileManager *mgr, bool ascending, std::vector<int>& selectionIndices, InterruptedFlag *interrupted)
{
    assertAllStatistics();
    doSort<double>([mgr](ID id) {return mgr->uncheckedGetStatistics(id)->getStatistics().getMin();}, mgr, ascending, selectionIndices, interrupted);
}

void IDList::sortStatisticsByMax(ResultFileManager *mgr, bool ascending, std::vector<int>& selectionIndices, InterruptedFlag *interrupted)
{
    assertAllStatistics();
    doSort<double>([mgr](ID id) {return mgr->uncheckedGetStatistics(id)->getStatistics().getMax();}, mgr, ascending, selectionIndices, interrupted);
}

void IDList::sortStatisticsByVariance(ResultFileManager *mgr, bool ascending, std::vector<int>& selectionIndices, InterruptedFlag *interrupted)
{
    assertAllStatistics();
    doSort<double>([mgr](ID id) {return mgr->uncheckedGetStatistics(id)->getStatistics().getVariance();}, mgr, ascending, selectionIndices, interrupted);
}

void IDList::sortStatisticsBySum(ResultFileManager *mgr, bool ascending, std::vector<int>& selectionIndices, InterruptedFlag *interrupted)
{
    assertAllStatistics();
    doSort<double>([mgr](ID id) {return mgr->uncheckedGetStatistics(id)->getStatistics().getSum();}, mgr, ascending, selectionIndices, interrupted);
}

void IDList::sortStatisticsBySumWeights(ResultFileManager *mgr, bool ascending, std::vector<int>& selectionIndices, InterruptedFlag *interrupted)
{
    assertAllStatistics();
    doSort<double>([mgr](ID id) {return mgr->uncheckedGetStatistics(id)->getStatistics().getSumWeights();}, mgr, ascending, selectionIndices, interrupted);
}

void IDList::sortHistogramsByNumBins(ResultFileManager *mgr, bool ascending, std::vector<int>& selectionIndices, InterruptedFlag *interrupted)
{
    assertAllHistograms();
    doSort<int>([mgr](ID id) {return mgr->uncheckedGetHistogram(id)->getHistogram().getNumBins();}, mgr, ascending, selectionIndices, interrupted);
}

void IDList::sortHistogramsByHistogramRange(ResultFileManager *mgr, bool ascending, std::vector<int>& selectionIndices, InterruptedFlag *interrupted)
{
    assertAllHistograms();
    doSort<int>([mgr](ID id) {return mgr->uncheckedGetHistogram(id)->getHistogram().getBinEdge(0);}, mgr, ascending, selectionIndices, interrupted); // actually, lower edge of histogram range
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

