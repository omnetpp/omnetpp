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

#include <stdlib.h>
#include <algorithm>
#include "idlist.h"
#include "resultfilemanager.h"

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

void IDList::set(int i, ID x)
{
    checkV();
    V::iterator it = std::find(v->begin(), v->end(), x);
    if (it==v->end())
        v->at(i) = x;  // at() includes bounds check
    else
        v->erase(it); // x is already in there -- just delete the element it replaces
}

void IDList::erase(int i)
{
    checkV();
    v->at(i);  // bounds check
    v->erase(v->begin()+i);
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
        throw new Exception("These items are not all scalars");
}

void IDList::checkIntegrityAllVectors(ResultFileManager *mgr) const
{
    checkV();
    for (V::const_iterator i=v->begin(); i!=v->end(); ++i)
        mgr->getItem(*i); // this will thow exception if id is not valid
    if (!areAllVectors())
        throw new Exception("These items are not all vectors");
}

class CmpBase {
    protected:
       ResultFileManager *mgr;
       bool less(const std::string& a, const std::string& b)
          {return strdictcmp(a.c_str(), b.c_str()) < 0;}
    public:
        CmpBase(ResultFileManager *m) {mgr = m;}
};

class CmpFileAndRun : public CmpBase {
    public:
        CmpFileAndRun(ResultFileManager *m) : CmpBase(m) {}
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

class CmpRunAndFile : public CmpBase {
    public:
        CmpRunAndFile(ResultFileManager *m) : CmpBase(m) {}
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

void IDList::sortByFileAndRun(ResultFileManager *mgr)
{
    checkV();
    std::sort(v->begin(), v->end(), CmpFileAndRun(mgr));
}

void IDList::sortByRunAndFile(ResultFileManager *mgr)
{
    checkV();
    std::sort(v->begin(), v->end(), CmpRunAndFile(mgr));
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
CMP(ValueMore, mgr->uncheckedGetScalar(a).value < mgr->uncheckedGetScalar(b).value)
CMP(ValueLess, mgr->uncheckedGetScalar(b).value < mgr->uncheckedGetScalar(a).value)

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
    // TODO
}

void IDList::sortVectorsByMean(ResultFileManager *mgr, bool ascending)
{
    checkIntegrityAllVectors(mgr);
    // TODO
}

void IDList::sortVectorsByStdDev(ResultFileManager *mgr, bool ascending)
{
    checkIntegrityAllVectors(mgr);
    // TODO
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

void IDList::toByteArray(char *array, int n) const
{
    checkV();
    if (n != v->size()*8)
        throw new Exception("byteArray is of wrong size -- must be 8*numIDs");
    std::copy(v->begin(), v->end(), (ID*)array);
}

void IDList::fromByteArray(char *array, int n)
{
    checkV();
    if (n%8 != 0)
        throw new Exception("byteArray size must be multiple of 8");
    v->resize(n/8);
    ID *a = (ID *)array;
    std::copy(a, a+n/8, v->begin());
}


