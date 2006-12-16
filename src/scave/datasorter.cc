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
#include "resultfilemanager.h"
#include "util.h"
#include "datasorter.h"
#include "stringutil.h"


ResultFileManager *ScalarDataSorter::tmpScalarMgr;




bool ScalarDataSorter::sameGroupFileRunScalar(const ScalarResult& d1, const ScalarResult& d2)
{
    return d1.fileRunRef==d2.fileRunRef && d1.nameRef==d2.nameRef;
}

bool ScalarDataSorter::sameGroupModuleScalar(const ScalarResult& d1, const ScalarResult& d2)
{
    return d1.moduleNameRef==d2.moduleNameRef && d1.nameRef==d2.nameRef;
}

bool ScalarDataSorter::sameGroupFileRunModule(const ScalarResult& d1, const ScalarResult& d2)
{
    return d1.fileRunRef==d2.fileRunRef && d1.moduleNameRef==d2.moduleNameRef;
}

bool ScalarDataSorter::lessByModuleRef(ID id1, ID id2)
{
    if (id1==-1 || id2==-1) return id2!=-1; // -1 is the smallest
    const ScalarResult& d1 = tmpScalarMgr->getScalar(id1);
    const ScalarResult& d2 = tmpScalarMgr->getScalar(id2);
    return strdictcmp(d1.moduleNameRef->c_str(), d2.moduleNameRef->c_str()) < 0;
}

bool ScalarDataSorter::equalByModuleRef(ID id1, ID id2)
{
    if (id1==-1 || id2==-1) return id1==id2;
    const ScalarResult& d1 = tmpScalarMgr->getScalar(id1);
    const ScalarResult& d2 = tmpScalarMgr->getScalar(id2);
    return d1.moduleNameRef == d2.moduleNameRef;
}

bool ScalarDataSorter::lessByFileAndRun(ID id1, ID id2)
{
    if (id1==-1 || id2==-1) return id2!=-1; // -1 is the smallest
    const ScalarResult& d1 = tmpScalarMgr->getScalar(id1);
    const ScalarResult& d2 = tmpScalarMgr->getScalar(id2);

    // compare first by file, then by run
    int cmpFile = strdictcmp(d1.fileRunRef->fileRef->filePath.c_str(), d2.fileRunRef->fileRef->filePath.c_str());
    if (cmpFile!=0)
        return cmpFile < 0;
    int cmpRun = strdictcmp(d1.fileRunRef->runRef->runName.c_str(), d2.fileRunRef->runRef->runName.c_str());
    return cmpRun < 0;
}

bool ScalarDataSorter::equalByFileAndRun(ID id1, ID id2)
{
    if (id1==-1 || id2==-1) return id1==id2;
    const ScalarResult& d1 = tmpScalarMgr->getScalar(id1);
    const ScalarResult& d2 = tmpScalarMgr->getScalar(id2);
    return d1.fileRunRef == d2.fileRunRef;
}

bool ScalarDataSorter::lessByScalarNameRef(ID id1, ID id2)
{
    if (id1==-1 || id2==-1) return id2!=-1; // -1 is the smallest
    const ScalarResult& d1 = tmpScalarMgr->getScalar(id1);
    const ScalarResult& d2 = tmpScalarMgr->getScalar(id2);
    return strdictcmp(d1.nameRef->c_str(), d2.nameRef->c_str()) < 0;
}

bool ScalarDataSorter::equalByScalarNameRef(ID id1, ID id2)
{
    if (id1==-1 || id2==-1) return id1==id2;
    const ScalarResult& d1 = tmpScalarMgr->getScalar(id1);
    const ScalarResult& d2 = tmpScalarMgr->getScalar(id2);
    return d1.nameRef == d2.nameRef;
}

bool ScalarDataSorter::lessByValue(ID id1, ID id2)
{
    if (id1==-1 || id2==-1) return id2!=-1; // -1 is the smallest
    const ScalarResult& d1 = tmpScalarMgr->getScalar(id1);
    const ScalarResult& d2 = tmpScalarMgr->getScalar(id2);
    return d1.value < d2.value;
}


IDVectorVector ScalarDataSorter::doGrouping(const IDList& idlist, GroupingFunc sameGroup)
{
    tmpScalarMgr = resultFileMgr;

    // parse idlist and do grouping as well, on the fly
    IDVectorVector vv;
    int sz = idlist.size();
    for (int ii = 0; ii < sz; ii++)
    {
        ID id = idlist.get(ii);

        // check of this id shares fileRef, runNumber & scalarName with one of the
        // IDVectors already in vv
        const ScalarResult& d = resultFileMgr->getScalar(id);
        IDVectorVector::iterator i;
        for (i=vv.begin(); i!=vv.end(); ++i)
        {
            int vvid = (*i)[0];  // first element in IDVector selected by i
            const ScalarResult& vvd = resultFileMgr->getScalar(vvid);
            if (sameGroup(d,vvd))
                break;
        }
        if (i==vv.end())
        {
            // not found -- new one has to be added
            vv.push_back(IDVector());
            i = vv.end()-1;
        }

        // insert
        i->push_back(id);
    }
    return vv;
}

void ScalarDataSorter::sortAndAlign(IDVectorVector& vv, CompareFunc less, CompareFunc equal)
{
    tmpScalarMgr = resultFileMgr;

    // order each group by module name
    for (IDVectorVector::iterator i=vv.begin(); i!=vv.end(); ++i)
        std::sort(i->begin(), i->end(), less);

    // now insert "null" elements (id=-1) so that every group is of same length,
    // and same indices are "equal()"
    for (int pos=0; ; pos++)
    {
        // determine "smallest" element in all vectors, on position "pos"
        int minId = -1;
        IDVectorVector::iterator i;
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

IDVectorVector ScalarDataSorter::groupByRunAndName(const IDList& idlist)
{
    // form groups (IDVectors) by fileRef+runNumber+scalarName
    IDVectorVector vv = doGrouping(idlist, sameGroupFileRunScalar);

    // order each group by module name, and insert "null" elements (id=-1) so that
    // every group is of same length, and same indices contain same moduleNameRefs
    sortAndAlign(vv, lessByModuleRef, equalByModuleRef);

    return vv;
}

IDVectorVector ScalarDataSorter::groupByModuleAndName(const IDList& idlist)
{
    // form groups (IDVectors) by moduleName+scalarName
    IDVectorVector vv = doGrouping(idlist, sameGroupModuleScalar);

    // order each group by fileRef+runNumber, and insert "null" elements (id=-1) so that
    // every group is of same length, and same indices contain same fileRef+runNumber
    sortAndAlign(vv, lessByFileAndRun, equalByFileAndRun);

    return vv;
}

IDVectorVector ScalarDataSorter::prepareScatterPlot(const IDList& idlist, const char *moduleName, const char *scalarName)
{
    // form groups (IDVectors) by moduleName+scalarName
    IDVectorVector vv = doGrouping(idlist, sameGroupModuleScalar);
    if (vv.size()==0)
        return vv;

    // order each group by fileRef+runNumber, and insert "null" elements (id=-1) so that
    // every group is of same length, and same indices contain same fileRef+runNumber
    sortAndAlign(vv, lessByFileAndRun, equalByFileAndRun);

    // find series for X axis (modulename, scalarname)...
    int xpos = -1;
    for (IDVectorVector::iterator i=vv.begin(); i!=vv.end(); ++i)
    {
        ID id = -1;
        for (IDVector::iterator j=i->begin(); j!=i->end(); ++j)
            if (*j!=-1)
                {id = *j;break;}
        if (id==-1)
            continue;
        const ScalarResult& d = resultFileMgr->getScalar(id);
        if (*d.moduleNameRef==moduleName && *d.nameRef==scalarName)
            {xpos = i-vv.begin();break;}
    }
    if (xpos==-1)
        throw new Exception("data for X axis not found");

    // ... and bring X series to 1st place
    if (xpos!=0)
        std::swap(vv[0], vv[xpos]);

    // sort x axis, moving elements in all other vectors as well.
    // Strategy: we'll construct the result in vv2. First we sort X axis, then
    // move elements of the other vectors to the same positions where the
    // X values went.

    // step one: sort X axis
    IDVectorVector vv2;
    vv2.resize(vv.size());
    vv2[0] = vv[0];
    std::sort(vv2[0].begin(), vv2[0].end(), lessByValue);

    // step two: remove id=-1 elements from the beginning of X series
    IDVector::iterator firstvalue=vv2[0].begin();
    while (*firstvalue==-1 && firstvalue!=vv2[0].end()) ++firstvalue;
    vv2[0].erase(vv2[0].begin(),firstvalue);

    // step three: allocate all other vectors in vv2 to be the same length
    // (necessary because we'll fill them in via assignment, NOT push_back() or insert())
    for (int k=1; k<(int)vv2.size(); k++)
        vv2[k].resize(vv2[0].size());

    // step four: copy over elements
    for (int pos=0; pos<(int)vv[0].size(); pos++)
    {
        ID id = vv[0][pos];
        if (id==-1) continue;
        int destpos = std::find(vv2[0].begin(),vv2[0].end(),id) - vv2[0].begin();
        for (int k=1; k<(int)vv.size(); k++)
            vv2[k][destpos] = vv[k][pos];
    }

    return vv2;
}

IDList ScalarDataSorter::getModuleAndNamePairs(const IDList& idlist, int maxcount)
{
    IDList out;

    // go through idlist and pick ids that represent a new (module, name pair)
    for (int ii = 0; ii < (int)idlist.size(); ii++)
    {
        ID id = idlist.get(ii);

        // check if module and name of this id is already in out[]
        const ScalarResult& d = resultFileMgr->getScalar(id);
        int i;
        int outSize = out.size();
        for (i=0; i<outSize; i++)
        {
            const ScalarResult& vd = resultFileMgr->getScalar(out.get(i));
            if (d.moduleNameRef==vd.moduleNameRef && d.nameRef==vd.nameRef)
                break;
        }

        // not yet -- then add it
        if (i==outSize)
        {
            out.add(id);
            if (outSize>maxcount)
                break; // enough is enough
        }
    }

    return out;
}

IDVectorVector ScalarDataSorter::prepareCopyToClipboard(const IDList& idlist)
{
    // form groups (IDVectors) by fileRef+runNumber+moduleNameRef
    IDVectorVector vv = doGrouping(idlist, sameGroupFileRunModule);

    // order each group by scalar name, and insert "null" elements (id=-1) so that
    // every group is of same length, and same indices contain same scalarNameRefs
    sortAndAlign(vv, lessByScalarNameRef, equalByScalarNameRef);

    return vv;
}

