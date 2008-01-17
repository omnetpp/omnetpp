//=========================================================================
//  IDLIST.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

// turn off 'Deprecated std::copy()' warning (MSVC80)
#ifdef _MSC_VER
#pragma warning(disable:4996)
#endif

#include <stdlib.h>
#include <algorithm>
#include "idlist.h"
#include "resultfilemanager.h"
#include "stringutil.h"

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
    checkV();
    for (V::const_iterator i=v->begin(); i!=v->end(); ++i)
        mgr->getItem(*i); // this will thow exception if id is not valid
    if (!areAllScalars())
        throw opp_runtime_error("These items are not all scalars");
}

void IDList::checkIntegrityAllVectors(ResultFileManager *mgr) const
{
    checkV();
    for (V::const_iterator i=v->begin(); i!=v->end(); ++i)
        mgr->getItem(*i); // this will thow exception if id is not valid
    if (!areAllVectors())
        throw opp_runtime_error("These items are not all vectors");
}

void IDList::checkIntegrityAllHistograms(ResultFileManager *mgr) const
{
    checkV();
    for (V::const_iterator i=v->begin(); i!=v->end(); ++i)
        mgr->getItem(*i); // this will thow exception if id is not valid
    if (!areAllHistograms())
        throw opp_runtime_error("These items are not all histograms");
}

class CmpBase {
    protected:
       ResultFileManager *mgr;
       bool less(const std::string& a, const std::string& b)
          {return strdictcmp(a.c_str(), b.c_str()) < 0;}
    public:
        CmpBase(ResultFileManager *m) {mgr = m;}
};

class FileAndRunLess : public CmpBase {
    public:
        FileAndRunLess(ResultFileManager *m) : CmpBase(m) {}
        bool operator()(ID a, ID b) { // implements operator<
            const FileRun *da = mgr->getItem(a).fileRunRef;
            const FileRun *db = mgr->getItem(b).fileRunRef;
            if (da==db)
                return false;
            else if (da->fileRef==db->fileRef)
                return da->runRef->runName < db->runRef->runName;
            else
                return da->fileRef->filePath < db->fileRef->filePath;
        }
};

class FileAndRunMore : public CmpBase {
    public:
        FileAndRunMore(ResultFileManager *m) : CmpBase(m) {}
        bool operator()(ID a, ID b) { // implements operator<
            const FileRun *da = mgr->getItem(a).fileRunRef;
            const FileRun *db = mgr->getItem(b).fileRunRef;
            if (da==db)
                return false;
            else if (da->fileRef==db->fileRef)
                return db->runRef->runName < da->runRef->runName;
            else
                return db->fileRef->filePath < da->fileRef->filePath;
        }
};

class RunAndFileLess : public CmpBase {
    public:
        RunAndFileLess(ResultFileManager *m) : CmpBase(m) {}
        bool operator()(ID a, ID b) { // implements operator<
            const FileRun *da = mgr->getItem(a).fileRunRef;
            const FileRun *db = mgr->getItem(b).fileRunRef;
            if (da==db)
                return false;
            else if (da->runRef==db->runRef)
                return da->fileRef->filePath < db->fileRef->filePath;
            else
                return da->runRef->runName < db->runRef->runName;
        }
};

class RunAndFileMore : public CmpBase {
    public:
        RunAndFileMore(ResultFileManager *m) : CmpBase(m) {}
        bool operator()(ID a, ID b) { // implements operator<
            const FileRun *da = mgr->getItem(a).fileRunRef;
            const FileRun *db = mgr->getItem(b).fileRunRef;
            if (da==db)
                return false;
            else if (da->runRef==db->runRef)
                return db->fileRef->filePath < da->fileRef->filePath;
            else
                return db->runRef->runName < da->runRef->runName;
        }
};

void IDList::sortByFileAndRun(ResultFileManager *mgr, bool ascending)
{
    checkV();
    if (v->size()>=2 && FileAndRunLess(mgr)(v->at(0), v->at(v->size()-1))!=ascending)
       reverse();
    if (ascending)
        std::sort(v->begin(), v->end(), FileAndRunLess(mgr));
    else
        std::sort(v->begin(), v->end(), FileAndRunMore(mgr));
}

void IDList::sortByRunAndFile(ResultFileManager *mgr, bool ascending)
{
    checkV();
    if (v->size()>=2 && RunAndFileLess(mgr)(v->at(0), v->at(v->size()-1))!=ascending)
       reverse();
    if (ascending)
        std::sort(v->begin(), v->end(), RunAndFileLess(mgr));
    else
        std::sort(v->begin(), v->end(), RunAndFileMore(mgr));
}

#define CMP(clazz,method) class clazz : public CmpBase { \
    public: \
        clazz(ResultFileManager *m) : CmpBase(m) {} \
        bool operator()(ID a, ID b) {return method;} \
    };

CMP(DirectoryLess, less(mgr->uncheckedGetItem(a).fileRunRef->fileRef->directory, mgr->uncheckedGetItem(b).fileRunRef->fileRef->directory))
CMP(DirectoryMore, less(mgr->uncheckedGetItem(b).fileRunRef->fileRef->directory, mgr->uncheckedGetItem(a).fileRunRef->fileRef->directory))
CMP(FileNameLess, less(mgr->uncheckedGetItem(a).fileRunRef->fileRef->fileName, mgr->uncheckedGetItem(b).fileRunRef->fileRef->fileName))
CMP(FileNameMore, less(mgr->uncheckedGetItem(b).fileRunRef->fileRef->fileName, mgr->uncheckedGetItem(a).fileRunRef->fileRef->fileName))
CMP(RunLess, less(mgr->uncheckedGetItem(a).fileRunRef->runRef->runName, mgr->uncheckedGetItem(b).fileRunRef->runRef->runName))
CMP(RunMore, less(mgr->uncheckedGetItem(b).fileRunRef->runRef->runName, mgr->uncheckedGetItem(a).fileRunRef->runRef->runName))
CMP(ModuleLess, less(*(mgr->uncheckedGetItem(a).moduleNameRef), *(mgr->uncheckedGetItem(b).moduleNameRef)))
CMP(ModuleMore, less(*(mgr->uncheckedGetItem(b).moduleNameRef), *(mgr->uncheckedGetItem(a).moduleNameRef)))
CMP(NameLess, less(*(mgr->uncheckedGetItem(a).nameRef), *(mgr->uncheckedGetItem(b).nameRef)))
CMP(NameMore, less(*(mgr->uncheckedGetItem(b).nameRef), *(mgr->uncheckedGetItem(a).nameRef)))
CMP(ValueLess, mgr->uncheckedGetScalar(a).value < mgr->uncheckedGetScalar(b).value)
CMP(ValueMore, mgr->uncheckedGetScalar(b).value < mgr->uncheckedGetScalar(a).value)
CMP(CountLess, mgr->uncheckedGetVector(a).count() < mgr->uncheckedGetVector(b).count())
CMP(CountMore, mgr->uncheckedGetVector(b).count() < mgr->uncheckedGetVector(a).count())
CMP(MeanLess, mgr->uncheckedGetVector(a).mean() < mgr->uncheckedGetVector(b).mean())
CMP(MeanMore, mgr->uncheckedGetVector(b).mean() < mgr->uncheckedGetVector(a).mean())
CMP(StddevLess, mgr->uncheckedGetVector(a).stddev() < mgr->uncheckedGetVector(b).stddev())
CMP(StddevMore, mgr->uncheckedGetVector(b).stddev() < mgr->uncheckedGetVector(a).stddev())
CMP(MinLess, mgr->uncheckedGetVector(a).min() < mgr->uncheckedGetVector(b).min())
CMP(MinMore, mgr->uncheckedGetVector(b).min() < mgr->uncheckedGetVector(a).min())
CMP(MaxLess, mgr->uncheckedGetVector(a).max() < mgr->uncheckedGetVector(b).max())
CMP(MaxMore, mgr->uncheckedGetVector(b).max() < mgr->uncheckedGetVector(a).max())

void IDList::sortByDirectory(ResultFileManager *mgr, bool ascending)
{
    checkIntegrity(mgr);
    if (v->size()>=2 && DirectoryLess(mgr)(v->at(0), v->at(v->size()-1))!=ascending)
       reverse();
    if (ascending)
        std::sort(v->begin(), v->end(), DirectoryLess(mgr));
    else
        std::sort(v->begin(), v->end(), DirectoryMore(mgr));
}

void IDList::sortByFileName(ResultFileManager *mgr, bool ascending)
{
    checkIntegrity(mgr);
    if (v->size()>=2 && FileNameLess(mgr)(v->at(0), v->at(v->size()-1))!=ascending)
       reverse();
    if (ascending)
        std::sort(v->begin(), v->end(), FileNameLess(mgr));
    else
        std::sort(v->begin(), v->end(), FileNameMore(mgr));
}

void IDList::sortByRun(ResultFileManager *mgr, bool ascending)
{
    checkIntegrity(mgr);
    // optimization: maybe it's sorted the other way round, so we reverse it to speed up sorting
    if (v->size()>=2 && RunLess(mgr).operator()(v->at(0), v->at(v->size()-1))!=ascending)
        reverse();
    if (ascending)
        std::sort(v->begin(), v->end(), RunLess(mgr));
    else
        std::sort(v->begin(), v->end(), RunMore(mgr));
}

void IDList::sortByModule(ResultFileManager *mgr, bool ascending)
{
    checkIntegrity(mgr);
    if (v->size()>=2 && ModuleLess(mgr)(v->at(0), v->at(v->size()-1))!=ascending)
       reverse();
    if (ascending)
        std::sort(v->begin(), v->end(), ModuleLess(mgr));
    else
        std::sort(v->begin(), v->end(), ModuleMore(mgr));
}

void IDList::sortByName(ResultFileManager *mgr, bool ascending)
{
    checkIntegrity(mgr);
    if (v->size()>=2 && NameLess(mgr)(v->at(0), v->at(v->size()-1))!=ascending)
       reverse();
    if (ascending)
        std::sort(v->begin(), v->end(), NameLess(mgr));
    else
        std::sort(v->begin(), v->end(), NameMore(mgr));
}

void IDList::sortScalarsByValue(ResultFileManager *mgr, bool ascending)
{
    checkIntegrityAllScalars(mgr);
    if (v->size()>=2 && ValueLess(mgr)(v->at(0), v->at(v->size()-1))!=ascending)
       reverse();
    if (ascending)
        std::sort(v->begin(), v->end(), ValueLess(mgr));
    else
        std::sort(v->begin(), v->end(), ValueMore(mgr));
}

void IDList::sortVectorsByLength(ResultFileManager *mgr, bool ascending)
{
    checkIntegrityAllVectors(mgr);
    if (v->size()>=2 && CountLess(mgr)(v->at(0), v->at(v->size()-1))!=ascending)
       reverse();
    if (ascending)
        std::sort(v->begin(), v->end(), CountLess(mgr));
    else
        std::sort(v->begin(), v->end(), CountMore(mgr));
}

void IDList::sortVectorsByMean(ResultFileManager *mgr, bool ascending)
{
    checkIntegrityAllVectors(mgr);
    if (v->size()>=2 && MeanLess(mgr)(v->at(0), v->at(v->size()-1))!=ascending)
       reverse();
    if (ascending)
        std::sort(v->begin(), v->end(), MeanLess(mgr));
    else
        std::sort(v->begin(), v->end(), MeanMore(mgr));
}

void IDList::sortVectorsByStdDev(ResultFileManager *mgr, bool ascending)
{
    checkIntegrityAllVectors(mgr);
    if (v->size()>=2 && StddevLess(mgr)(v->at(0), v->at(v->size()-1))!=ascending)
       reverse();
    if (ascending)
        std::sort(v->begin(), v->end(), StddevLess(mgr));
    else
        std::sort(v->begin(), v->end(), StddevMore(mgr));
}

void IDList::sortVectorsByMin(ResultFileManager *mgr, bool ascending)
{
    checkIntegrityAllVectors(mgr);
    if (v->size()>=2 && MinLess(mgr)(v->at(0), v->at(v->size()-1))!=ascending)
       reverse();
    if (ascending)
        std::sort(v->begin(), v->end(), MinLess(mgr));
    else
        std::sort(v->begin(), v->end(), MinMore(mgr));
}

void IDList::sortVectorsByMax(ResultFileManager *mgr, bool ascending)
{
    checkIntegrityAllVectors(mgr);
    if (v->size()>=2 && MaxLess(mgr)(v->at(0), v->at(v->size()-1))!=ascending)
       reverse();
    if (ascending)
        std::sort(v->begin(), v->end(), MaxLess(mgr));
    else
        std::sort(v->begin(), v->end(), MaxMore(mgr));
}

class RunAttributeLess : CmpBase {
    private:
        const char* attrName;
    public:
        RunAttributeLess(ResultFileManager* m, const char* attrName)
            : CmpBase(m), attrName(attrName) {}
        bool operator()(ID a, ID b) {
            const char* aValue = mgr->uncheckedGetItem(a).fileRunRef->runRef->getAttribute(attrName);
            const char* bValue = mgr->uncheckedGetItem(b).fileRunRef->runRef->getAttribute(attrName);
            return ((aValue && bValue) ? less(aValue, bValue) : aValue!=NULL);
        }
};

class RunAttributeMore : CmpBase {
    private:
        const char* attrName;
    public:
        RunAttributeMore(ResultFileManager* m, const char* attrName)
            : CmpBase(m), attrName(attrName) {}
        bool operator()(ID a, ID b) {
            const char* aValue = mgr->uncheckedGetItem(a).fileRunRef->runRef->getAttribute(attrName);
            const char* bValue = mgr->uncheckedGetItem(b).fileRunRef->runRef->getAttribute(attrName);
            return ((bValue && aValue) ? less(bValue, aValue) : bValue!=NULL);
        }
};

void IDList::sortByRunAttribute(ResultFileManager *mgr, const char* runAttribute, bool ascending) {
    checkIntegrity(mgr);
    if (v->size()>=2 && RunAttributeLess(mgr, runAttribute)(v->at(0), v->at(v->size()-1)) != ascending)
       reverse();
    if (ascending)
        std::sort(v->begin(), v->end(), RunAttributeLess(mgr, runAttribute));
    else
        std::sort(v->begin(), v->end(), RunAttributeMore(mgr, runAttribute));
}

void IDList::reverse()
{
    checkV();
    std::reverse(v->begin(), v->end());
}

int IDList::itemTypes() const
{
    checkV();
    int types = 0;
    for (V::const_iterator i=v->begin(); i!=v->end(); ++i)
        types |= ResultFileManager::_type(*i);
    return types;
}

bool IDList::areAllScalars() const
{
    return itemTypes()==ResultFileManager::SCALAR;
}

bool IDList::areAllVectors() const
{
    return itemTypes()==ResultFileManager::VECTOR;
}

bool IDList::areAllHistograms() const
{
    return itemTypes()==ResultFileManager::HISTOGRAM;
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


