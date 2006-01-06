//==========================================================================
//  DATASORTER.CC - part of
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

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "scalarmanager.h"
#include "util.h"
#include "datasorter.h"
#include "patmatch.h"


ScalarManager *DataSorter::tmpScalarMgr;

IntVector DataSorter::getFilteredScalarList(const char *fileAndRunFilter,
                                            const char *moduleFilter,
                                            const char *nameFilter)
{
    // prepare for wildcard matches
    bool patMatchFile = contains_wildcards(fileAndRunFilter);
    bool patMatchModule = contains_wildcards(moduleFilter);
    bool patMatchName = contains_wildcards(nameFilter);

    short *filePattern = NULL;
    if (patMatchFile)
    {
        filePattern = new short[512]; // FIXME!
        transform_pattern(fileAndRunFilter, filePattern);
    }

    short *modulePattern = NULL;
    if (patMatchModule)
    {
        modulePattern = new short[512]; // FIXME!
        transform_pattern(moduleFilter, modulePattern);
    }
    short *namePattern = NULL;
    if (patMatchName)
    {
        namePattern = new short[512]; // FIXME!
        transform_pattern(nameFilter, namePattern);
    }

    // iterate over all values and add matching ones to outVec.
    // we can exploit the fact that ScalarManager contains the data in the order
    // they were read from file, i.e. grouped by file and run number
    IntVector outVec;
    const ScalarManager::Values& scalars = scalarMgr->getValues();
    ScalarManager::RunRef lastRunRef = scalarMgr->getRuns().end();
    bool lastRunMatched = false;
    for (ScalarManager::Values::const_iterator i=scalars.begin(); i!=scalars.end(); i++)
    {
        const ScalarManager::Datum& d = *i;

        if (fileAndRunFilter && fileAndRunFilter[0])
        {
            if (lastRunRef!=d.runRef)
            {
                lastRunRef = d.runRef;
                lastRunMatched = (patMatchFile
                    ? stringmatch(filePattern, lastRunRef->fileAndRunName.c_str())
                    : !strcmp(lastRunRef->fileAndRunName.c_str(), fileAndRunFilter));
            }
            if (!lastRunMatched)
                continue;
        }

        if (moduleFilter && moduleFilter[0] &&
            (patMatchModule ? !stringmatch(modulePattern, d.moduleNameRef->c_str())
                            : strcmp(d.moduleNameRef->c_str(), moduleFilter))
           )
            continue; // no match

        if (nameFilter && nameFilter[0] &&
            (patMatchName ? !stringmatch(namePattern, d.scalarNameRef->c_str())
                          : strcmp(d.scalarNameRef->c_str(), nameFilter))
           )
            continue; // no match

        // everything matched
        int id = i - scalars.begin();
        outVec.push_back(id);
    }
    return outVec;
}

static void sortIntArray(IntVector::iterator begin, IntVector::iterator end, bool (*less)(int,int))
{
    std::sort(begin, end, less);
}

bool DataSorter::sameGroupFileRunScalar(const ScalarManager::Datum& d1, const ScalarManager::Datum& d2)
{
    return d1.runRef==d2.runRef && d1.scalarNameRef==d2.scalarNameRef;
}

bool DataSorter::sameGroupModuleScalar(const ScalarManager::Datum& d1, const ScalarManager::Datum& d2)
{
    return d1.moduleNameRef==d2.moduleNameRef && d1.scalarNameRef==d2.scalarNameRef;
}

bool DataSorter::sameGroupFileRunModule(const ScalarManager::Datum& d1, const ScalarManager::Datum& d2)
{
    return d1.runRef==d2.runRef && d1.moduleNameRef==d2.moduleNameRef;
}

bool DataSorter::lessByModuleRef(int id1, int id2)
{
    if (id1==-1 || id2==-1) return id2!=-1; // -1 is the smallest
    const ScalarManager::Datum& d1 = tmpScalarMgr->getValue(id1);
    const ScalarManager::Datum& d2 = tmpScalarMgr->getValue(id2);
    return strdictcmp(d1.moduleNameRef->c_str(), d2.moduleNameRef->c_str()) < 0;
}

bool DataSorter::equalByModuleRef(int id1, int id2)
{
    if (id1==-1 || id2==-1) return id1==id2;
    const ScalarManager::Datum& d1 = tmpScalarMgr->getValue(id1);
    const ScalarManager::Datum& d2 = tmpScalarMgr->getValue(id2);
    return d1.moduleNameRef == d2.moduleNameRef;
}

bool DataSorter::lessByFileAndRun(int id1, int id2)
{
    if (id1==-1 || id2==-1) return id2!=-1; // -1 is the smallest
    const ScalarManager::Datum& d1 = tmpScalarMgr->getValue(id1);
    const ScalarManager::Datum& d2 = tmpScalarMgr->getValue(id2);
    return strdictcmp(d1.runRef->fileAndRunName.c_str(), d2.runRef->fileAndRunName.c_str()) < 0;
}

bool DataSorter::equalByFileAndRun(int id1, int id2)
{
    if (id1==-1 || id2==-1) return id1==id2;
    const ScalarManager::Datum& d1 = tmpScalarMgr->getValue(id1);
    const ScalarManager::Datum& d2 = tmpScalarMgr->getValue(id2);
    return d1.runRef == d2.runRef;
}

bool DataSorter::lessByScalarNameRef(int id1, int id2)
{
    if (id1==-1 || id2==-1) return id2!=-1; // -1 is the smallest
    const ScalarManager::Datum& d1 = tmpScalarMgr->getValue(id1);
    const ScalarManager::Datum& d2 = tmpScalarMgr->getValue(id2);
    return strdictcmp(d1.scalarNameRef->c_str(), d2.scalarNameRef->c_str()) < 0;
}

bool DataSorter::equalByScalarNameRef(int id1, int id2)
{
    if (id1==-1 || id2==-1) return id1==id2;
    const ScalarManager::Datum& d1 = tmpScalarMgr->getValue(id1);
    const ScalarManager::Datum& d2 = tmpScalarMgr->getValue(id2);
    return d1.scalarNameRef == d2.scalarNameRef;
}

bool DataSorter::lessByValue(int id1, int id2)
{
    if (id1==-1 || id2==-1) return id2!=-1; // -1 is the smallest
    const ScalarManager::Datum& d1 = tmpScalarMgr->getValue(id1);
    const ScalarManager::Datum& d2 = tmpScalarMgr->getValue(id2);
    return d1.value < d2.value;
}


IntVectorVector DataSorter::doGrouping(const IntVector& idlist, GroupingFunc sameGroup)
{
    tmpScalarMgr = scalarMgr;

    // parse idlist and do grouping as well, on the fly
    IntVectorVector vv;
    for (int ii = 0; ii < (int)idlist.size(); ii++)
    {
        int id = idlist[ii];

        // check of this id shares fileRef, runNumber & scalarName with one of the
        // IntVectors already in vv
        const ScalarManager::Datum& d = scalarMgr->getValue(id);
        IntVectorVector::iterator i;
        for (i=vv.begin(); i!=vv.end(); ++i)
        {
            int vvid = (*i)[0];  // first element in IntVector selected by i
            const ScalarManager::Datum& vvd = scalarMgr->getValue(vvid);
            if (sameGroup(d,vvd))
                break;
        }
        if (i==vv.end())
        {
            // not found -- new one has to be added
            vv.push_back(IntVector());
            i = vv.end()-1;
        }

        // insert
        i->push_back(id);
    }
    return vv;
}

void DataSorter::sortAndAlign(IntVectorVector& vv, CompareFunc less, CompareFunc equal)
{
    tmpScalarMgr = scalarMgr;

    // order each group by module name
    for (IntVectorVector::iterator i=vv.begin(); i!=vv.end(); ++i)
        sortIntArray(i->begin(), i->end(), less); // MSVC 6.0 produces linker error for std::sort

    // now insert "null" elements (id=-1) so that every group is of same length,
    // and same indices are "equal()"
    for (int pos=0; ; pos++)
    {
        // determine "smallest" element in all vectors, on position "pos"
        int minId = -1;
        IntVectorVector::iterator i;
        for (i=vv.begin(); i!=vv.end(); ++i)
            if ((int)i->size()>pos)
                minId = (minId==-1) ? (*i)[pos] : less((*i)[pos],minId) ? (*i)[pos] : minId;

        // if pos is past the end of all vectors, we're done
        if (minId==-1)
            break;

        // if a vector has something different on this position, add a "null" element here
        for (i=vv.begin(); i!=vv.end(); ++i)
            if ((int)i->size()<=pos || !equal((*i)[pos],minId))
                i->insert(i->begin()+pos,-1);
    }
}

IntVectorVector DataSorter::groupByRunAndName(const IntVector& idlist)
{
    // form groups (IntVectors) by fileRef+runNumber+scalarName
    IntVectorVector vv = doGrouping(idlist, sameGroupFileRunScalar);

    // order each group by module name, and insert "null" elements (id=-1) so that
    // every group is of same length, and same indices contain same moduleNameRefs
    sortAndAlign(vv, lessByModuleRef, equalByModuleRef);

    return vv;
}

IntVectorVector DataSorter::groupByModuleAndName(const IntVector& idlist)
{
    // form groups (IntVectors) by moduleName+scalarName
    IntVectorVector vv = doGrouping(idlist, sameGroupModuleScalar);

    // order each group by fileRef+runNumber, and insert "null" elements (id=-1) so that
    // every group is of same length, and same indices contain same fileRef+runNumber
    sortAndAlign(vv, lessByFileAndRun, equalByFileAndRun);

    return vv;
}

IntVectorVector DataSorter::prepareScatterPlot(const IntVector& idlist, const char *moduleName, const char *scalarName)
{
    // form groups (IntVectors) by moduleName+scalarName
    IntVectorVector vv = doGrouping(idlist, sameGroupModuleScalar);
    if (vv.size()==0)
        return vv;

    // order each group by fileRef+runNumber, and insert "null" elements (id=-1) so that
    // every group is of same length, and same indices contain same fileRef+runNumber
    sortAndAlign(vv, lessByFileAndRun, equalByFileAndRun);

    // find series for X axis (modulename, scalarname)...
    int xpos = -1;
    for (IntVectorVector::iterator i=vv.begin(); i!=vv.end(); ++i)
    {
        int id = -1;
        for (IntVector::iterator j=i->begin(); j!=i->end(); ++j)
            if (*j!=-1)
                {id = *j;break;}
        if (id==-1)
            continue;
        const ScalarManager::Datum& d = scalarMgr->getValue(id);
        if (*d.moduleNameRef==moduleName && *d.scalarNameRef==scalarName)
            {xpos = i-vv.begin();break;}
    }
    if (xpos==-1)
        throw new TException("data for X axis not found");

    // ... and bring X series to 1st place
    if (xpos!=0)
        std::swap(vv[0], vv[xpos]);

    // sort x axis, moving elements in all other vectors as well.
    // Strategy: we'll construct the result in vv2. First we sort X axis, then
    // move elements of the other vectors to the same positions where the
    // X values went.

    // step one: sort X axis
    IntVectorVector vv2;
    vv2.resize(vv.size());
    vv2[0] = vv[0];
    sortIntArray(vv2[0].begin(), vv2[0].end(), lessByValue);  // MSVC 6.0 produces linker error for std::sort

    // step two: remove id=-1 elements from the beginning of X series
    IntVector::iterator firstvalue=vv2[0].begin();
    while (*firstvalue==-1 && firstvalue!=vv2[0].end()) ++firstvalue;
    vv2[0].erase(vv2[0].begin(),firstvalue);

    // step three: allocate all other vectors in vv2 to be the same length
    // (necessary because we'll fill them in via assignment, NOT push_back() or insert())
    for (int k=1; k<(int)vv2.size(); k++)
        vv2[k].resize(vv2[0].size());

    // step four: copy over elements
    for (int pos=0; pos<(int)vv[0].size(); pos++)
    {
        int id = vv[0][pos];
        if (id==-1) continue;
        int destpos = std::find(vv2[0].begin(),vv2[0].end(),id) - vv2[0].begin();
        for (int k=1; k<(int)vv.size(); k++)
            vv2[k][destpos] = vv[k][pos];
    }

    return vv2;
}

IntVector DataSorter::getModuleAndNamePairs(const IntVector& idlist, int maxcount)
{
    IntVector vec;

    // go through idlist and pick ids that represent a new (module, name pair)
    for (int ii = 0; ii < (int)idlist.size(); ii++)
    {
        int id = idlist[ii];

        // check if module and name of this id is already in vec[]
        const ScalarManager::Datum& d = scalarMgr->getValue(id);
        IntVector::iterator i;
        for (i=vec.begin(); i!=vec.end(); ++i)
        {
            const ScalarManager::Datum& vd = scalarMgr->getValue(*i);
            if (d.moduleNameRef==vd.moduleNameRef && d.scalarNameRef==vd.scalarNameRef)
                break;
        }

        // not yet -- then add it
        if (i==vec.end())
        {
            vec.push_back(id);
            if ((int)vec.size()>=maxcount)
                break; // enough is enough
        }
    }

    return vec;
}

IntVectorVector DataSorter::prepareCopyToClipboard(const IntVector& idlist)
{
    // form groups (IntVectors) by fileRef+runNumber+moduleNameRef
    IntVectorVector vv = doGrouping(idlist, sameGroupFileRunModule);

    // order each group by scalar name, and insert "null" elements (id=-1) so that
    // every group is of same length, and same indices contain same scalarNameRefs
    sortAndAlign(vv, lessByScalarNameRef, equalByScalarNameRef);

    return vv;
}

