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

USING_NAMESPACE

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

void IDList::substract(ID x)
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

void IDList::substract(IDList& ids)
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
CMP(CountLess, uncheckedGetVector(a).getCount() < uncheckedGetVector(b).getCount())
CMP(MeanLess, uncheckedGetVector(a).getMean() < uncheckedGetVector(b).getMean())
CMP(StddevLess, uncheckedGetVector(a).getStddev() < uncheckedGetVector(b).getStddev())
CMP(MinLess, uncheckedGetVector(a).getMin() < uncheckedGetVector(b).getMin())
CMP(MaxLess, uncheckedGetVector(a).getMax() < uncheckedGetVector(b).getMax())
CMP(StartTimeLess, uncheckedGetVector(a).startTime < uncheckedGetVector(b).startTime)
CMP(EndTimeLess, uncheckedGetVector(a).endTime < uncheckedGetVector(b).endTime)

//template <class T>
//void IDList::sortBy(ResultFileManager *mgr, bool ascending, T& comparator)
//{
//    checkIntegrity(mgr);
//    // optimization: maybe it's sorted the other way round, so we reverse it to speed up sorting
//    if (v->size()>=2 && comparator(v->at(0), v->at(v->size()-1))!=ascending)
//       reverse();
//    if (ascending)
//        std::sort(v->begin(), v->end(), comparator);
//    else
//        std::sort(v->begin(), v->end(), flipArgs(comparator));
//}
//
//template <class T>
//void IDList::sortScalarsBy(ResultFileManager *mgr, bool ascending, T& comparator)
//{
//    checkIntegrityAllScalars(mgr);
//    // optimization: maybe it's sorted the other way round, so we reverse it to speed up sorting
//    if (v->size()>=2 && comparator(v->at(0), v->at(v->size()-1))!=ascending)
//       reverse();
//    if (ascending)
//        std::sort(v->begin(), v->end(), comparator);
//    else
//        std::sort(v->begin(), v->end(), flipArgs(comparator));
//}
//
//template <class T>
//void IDList::sortVectorsBy(ResultFileManager *mgr, bool ascending, T& comparator)
//{
//    checkIntegrityAllVectors(mgr);
//    // optimization: maybe it's sorted the other way round, so we reverse it to speed up sorting
//    if (v->size()>=2 && comparator(v->at(0), v->at(v->size()-1))!=ascending)
//       reverse();
//    if (ascending)
//        std::sort(v->begin(), v->end(), comparator);
//    else
//        std::sort(v->begin(), v->end(), flipArgs(comparator));
//}


void IDList::sortByFileAndRun(ResultFileManager *mgr, bool ascending)
{
	//sortBy(mgr, ascending, FileAndRunLess(mgr));
    checkIntegrity(mgr);
    // optimization: maybe it's sorted the other way round, so we reverse it to speed up sorting
    if (v->size()>=2 && FileAndRunLess(mgr)(v->at(0), v->at(v->size()-1))!=ascending)
       reverse();
    if (ascending)
        std::sort(v->begin(), v->end(), FileAndRunLess(mgr));
    else
        std::sort(v->begin(), v->end(), flipArgs(FileAndRunLess(mgr)));
}

void IDList::sortByRunAndFile(ResultFileManager *mgr, bool ascending)
{
	//sortBy(mgr, ascending, RunAndFileLess(mgr));
    checkIntegrity(mgr);
    // optimization: maybe it's sorted the other way round, so we reverse it to speed up sorting
    if (v->size()>=2 && RunAndFileLess(mgr)(v->at(0), v->at(v->size()-1))!=ascending)
       reverse();
    if (ascending)
        std::sort(v->begin(), v->end(), RunAndFileLess(mgr));
    else
        std::sort(v->begin(), v->end(), flipArgs(RunAndFileLess(mgr)));
}

void IDList::sortByDirectory(ResultFileManager *mgr, bool ascending)
{
	//sortBy(mgr, ascending, DirectoryLess(mgr));
    checkIntegrity(mgr);
    // optimization: maybe it's sorted the other way round, so we reverse it to speed up sorting
    if (v->size()>=2 && DirectoryLess(mgr)(v->at(0), v->at(v->size()-1))!=ascending)
       reverse();
    if (ascending)
        std::sort(v->begin(), v->end(), DirectoryLess(mgr));
    else
        std::sort(v->begin(), v->end(), flipArgs(DirectoryLess(mgr)));
}

void IDList::sortByFileName(ResultFileManager *mgr, bool ascending)
{
	//sortBy(mgr, ascending, FileNameLess(mgr));
    checkIntegrity(mgr);
    // optimization: maybe it's sorted the other way round, so we reverse it to speed up sorting
    if (v->size()>=2 && FileNameLess(mgr)(v->at(0), v->at(v->size()-1))!=ascending)
       reverse();
    if (ascending)
        std::sort(v->begin(), v->end(), FileNameLess(mgr));
    else
        std::sort(v->begin(), v->end(), flipArgs(FileNameLess(mgr)));
}

void IDList::sortByRun(ResultFileManager *mgr, bool ascending)
{
	//sortBy(mgr, ascending, RunLess(mgr));
    checkIntegrity(mgr);
    // optimization: maybe it's sorted the other way round, so we reverse it to speed up sorting
    if (v->size()>=2 && RunLess(mgr)(v->at(0), v->at(v->size()-1))!=ascending)
       reverse();
    if (ascending)
        std::sort(v->begin(), v->end(), RunLess(mgr));
    else
        std::sort(v->begin(), v->end(), flipArgs(RunLess(mgr)));
}

void IDList::sortByModule(ResultFileManager *mgr, bool ascending)
{
	//sortBy(mgr, ascending, ModuleLess(mgr));
    checkIntegrity(mgr);
    // optimization: maybe it's sorted the other way round, so we reverse it to speed up sorting
    if (v->size()>=2 && ModuleLess(mgr)(v->at(0), v->at(v->size()-1))!=ascending)
       reverse();
    if (ascending)
        std::sort(v->begin(), v->end(), ModuleLess(mgr));
    else
        std::sort(v->begin(), v->end(), flipArgs(ModuleLess(mgr)));
}

void IDList::sortByName(ResultFileManager *mgr, bool ascending)
{
	//sortBy(mgr, ascending, NameLess(mgr));
    checkIntegrity(mgr);
    // optimization: maybe it's sorted the other way round, so we reverse it to speed up sorting
    if (v->size()>=2 && NameLess(mgr)(v->at(0), v->at(v->size()-1))!=ascending)
       reverse();
    if (ascending)
        std::sort(v->begin(), v->end(), NameLess(mgr));
    else
        std::sort(v->begin(), v->end(), flipArgs(NameLess(mgr)));
}

void IDList::sortScalarsByValue(ResultFileManager *mgr, bool ascending)
{
	//sortScalarsBy(mgr, ascending, ValueLess(mgr));
    checkIntegrityAllScalars(mgr);
    // optimization: maybe it's sorted the other way round, so we reverse it to speed up sorting
    if (v->size()>=2 && ValueLess(mgr)(v->at(0), v->at(v->size()-1))!=ascending)
       reverse();
    if (ascending)
        std::sort(v->begin(), v->end(), ValueLess(mgr));
    else
        std::sort(v->begin(), v->end(), flipArgs(ValueLess(mgr)));
}

void IDList::sortVectorsByVectorId(ResultFileManager *mgr, bool ascending)
{
//	sortVectorsBy(mgr, ascending, VectorIdLess(mgr));
    checkIntegrityAllVectors(mgr);
    // optimization: maybe it's sorted the other way round, so we reverse it to speed up sorting
    if (v->size()>=2 && VectorIdLess(mgr)(v->at(0), v->at(v->size()-1))!=ascending)
       reverse();
    if (ascending)
        std::sort(v->begin(), v->end(), VectorIdLess(mgr));
    else
        std::sort(v->begin(), v->end(), flipArgs(VectorIdLess(mgr)));
}


void IDList::sortVectorsByLength(ResultFileManager *mgr, bool ascending)
{
	//sortVectorsBy(mgr, ascending, CountLess(mgr));
    checkIntegrityAllVectors(mgr);
    // optimization: maybe it's sorted the other way round, so we reverse it to speed up sorting
    if (v->size()>=2 && CountLess(mgr)(v->at(0), v->at(v->size()-1))!=ascending)
       reverse();
    if (ascending)
        std::sort(v->begin(), v->end(), CountLess(mgr));
    else
        std::sort(v->begin(), v->end(), flipArgs(CountLess(mgr)));

}

void IDList::sortVectorsByMean(ResultFileManager *mgr, bool ascending)
{
	//sortVectorsBy(mgr, ascending, MeanLess(mgr));
    checkIntegrityAllVectors(mgr);
    // optimization: maybe it's sorted the other way round, so we reverse it to speed up sorting
    if (v->size()>=2 && MeanLess(mgr)(v->at(0), v->at(v->size()-1))!=ascending)
       reverse();
    if (ascending)
        std::sort(v->begin(), v->end(), MeanLess(mgr));
    else
        std::sort(v->begin(), v->end(), flipArgs(MeanLess(mgr)));
}

void IDList::sortVectorsByStdDev(ResultFileManager *mgr, bool ascending)
{
	//sortVectorsBy(mgr, ascending, StddevLess(mgr));
    checkIntegrityAllVectors(mgr);
    // optimization: maybe it's sorted the other way round, so we reverse it to speed up sorting
    if (v->size()>=2 && StddevLess(mgr)(v->at(0), v->at(v->size()-1))!=ascending)
       reverse();
    if (ascending)
        std::sort(v->begin(), v->end(), StddevLess(mgr));
    else
        std::sort(v->begin(), v->end(), flipArgs(StddevLess(mgr)));
}

void IDList::sortVectorsByMin(ResultFileManager *mgr, bool ascending)
{
	//sortVectorsBy(mgr, ascending, MinLess(mgr));
    checkIntegrityAllVectors(mgr);
    // optimization: maybe it's sorted the other way round, so we reverse it to speed up sorting
    if (v->size()>=2 && MinLess(mgr)(v->at(0), v->at(v->size()-1))!=ascending)
       reverse();
    if (ascending)
        std::sort(v->begin(), v->end(), MinLess(mgr));
    else
        std::sort(v->begin(), v->end(), flipArgs(MinLess(mgr)));
}

void IDList::sortVectorsByMax(ResultFileManager *mgr, bool ascending)
{
	//sortVectorsBy(mgr, ascending, MaxLess(mgr));
    checkIntegrityAllVectors(mgr);
    // optimization: maybe it's sorted the other way round, so we reverse it to speed up sorting
    if (v->size()>=2 && MaxLess(mgr)(v->at(0), v->at(v->size()-1))!=ascending)
       reverse();
    if (ascending)
        std::sort(v->begin(), v->end(), MaxLess(mgr));
    else
        std::sort(v->begin(), v->end(), flipArgs(MaxLess(mgr)));
}

void IDList::sortVectorsByStartTime(ResultFileManager *mgr, bool ascending)
{
	//sortVectorsBy(mgr, ascending, StartTimeLess(mgr));
    checkIntegrityAllVectors(mgr);
    // optimization: maybe it's sorted the other way round, so we reverse it to speed up sorting
    if (v->size()>=2 && StartTimeLess(mgr)(v->at(0), v->at(v->size()-1))!=ascending)
       reverse();
    if (ascending)
        std::sort(v->begin(), v->end(), StartTimeLess(mgr));
    else
        std::sort(v->begin(), v->end(), flipArgs(StartTimeLess(mgr)));
}

void IDList::sortVectorsByEndTime(ResultFileManager *mgr, bool ascending)
{
	//sortVectorsBy(mgr, ascending, EndTimeLess(mgr));
    checkIntegrityAllVectors(mgr);
    // optimization: maybe it's sorted the other way round, so we reverse it to speed up sorting
    if (v->size()>=2 && EndTimeLess(mgr)(v->at(0), v->at(v->size()-1))!=ascending)
       reverse();
    if (ascending)
        std::sort(v->begin(), v->end(), EndTimeLess(mgr));
    else
        std::sort(v->begin(), v->end(), flipArgs(EndTimeLess(mgr)));
}

void IDList::sortByRunAttribute(ResultFileManager *mgr, const char* runAttribute, bool ascending) {
	//sortBy(mgr, ascending, RunAttributeLess(mgr, runAttribute));
    checkIntegrity(mgr);
    // optimization: maybe it's sorted the other way round, so we reverse it to speed up sorting
    if (v->size()>=2 && RunAttributeLess(mgr, runAttribute)(v->at(0), v->at(v->size()-1))!=ascending)
       reverse();
    if (ascending)
        std::sort(v->begin(), v->end(), RunAttributeLess(mgr, runAttribute));
    else
        std::sort(v->begin(), v->end(), flipArgs(RunAttributeLess(mgr, runAttribute)));
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
    return getItemTypes()==ResultFileManager::SCALAR;
}

bool IDList::areAllVectors() const
{
    return getItemTypes()==ResultFileManager::VECTOR;
}

bool IDList::areAllHistograms() const
{
    return getItemTypes()==ResultFileManager::HISTOGRAM;
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


