//==========================================================================
//  VISITOR.CC - part of
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

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "cenvir.h"
#include "cobject.h"
#include "cmodule.h"
#include "cmessage.h"
#include "cqueue.h"
#include "cstat.h"
#include "coutvect.h"
#include "cwatch.h"
#include "cfsm.h"
#include "cpar.h"
#include "cchannel.h"
#include "cgate.h"

#include "patternmatcher.h"
#include "visitor.h"



cCollectObjectsVisitor::cCollectObjectsVisitor()
{
    sizelimit = 0; // no limit by default
    size = 16;
    arr = new cObject *[size];
    count = 0;
}

cCollectObjectsVisitor::~cCollectObjectsVisitor()
{
    delete [] arr;
}

void cCollectObjectsVisitor::setSizeLimit(int limit)
{
    sizelimit = limit;
}

void cCollectObjectsVisitor::addPointer(cObject *obj)
{
    if (sizelimit && count==sizelimit)
        throw EndTraversalException();

    // if array is full, reallocate
    if (count==size)
    {
        cObject **arr2 = new cObject *[2*size];
        for (int i=0; i<count; i++) arr2[i] = arr[i];
        delete [] arr;
        arr = arr2;
        size = 2*size;
    }

    // add pointer to array
    arr[count++] = obj;
}

void cCollectObjectsVisitor::visit(cObject *obj)
{
    addPointer(obj);

    // go to children
    obj->forEachChild(this);
}

//-----------------------------------------------------------------------

cFilteredCollectObjectsVisitor::cFilteredCollectObjectsVisitor()
{
    category = ~0U;
    classnamepattern = NULL;
    objfullpathpattern = NULL;
}

cFilteredCollectObjectsVisitor::~cFilteredCollectObjectsVisitor()
{
    delete classnamepattern;
    delete objfullpathpattern;
}

void cFilteredCollectObjectsVisitor::setFilterPars(unsigned int cat,
                                                   const char *classnamepatt,
                                                   const char *objfullpathpatt)
{
    // Note: pattern matcher will throw exception on pattern syntax error
    category = cat;
    if (classnamepatt && classnamepatt[0])
        classnamepattern = new MatchExpression(classnamepatt, false, true, true);

    if (objfullpathpatt && objfullpathpatt[0])
        objfullpathpattern = new MatchExpression(objfullpathpatt, false, true, true);
}

void cFilteredCollectObjectsVisitor::visit(cObject *obj)
{
    bool ok = (category==~0U) ||
        ((category&CATEGORY_MODULES) && dynamic_cast<cModule *>(obj)) ||
        ((category&CATEGORY_MESSAGES) && dynamic_cast<cMessage *>(obj)) ||
        ((category&CATEGORY_QUEUES) && dynamic_cast<cQueue *>(obj)) ||
        ((category&CATEGORY_VARIABLES) && (dynamic_cast<cWatchBase *>(obj) ||
                                           dynamic_cast<cFSM *>(obj))) ||
        ((category&CATEGORY_STATISTICS) &&(dynamic_cast<cOutVector *>(obj) ||
                                           dynamic_cast<cWatchBase *>(obj) ||
                                           dynamic_cast<cStatistic *>(obj))) ||
        ((category&CATEGORY_MODPARAMS) &&(dynamic_cast<cModulePar *>(obj))) ||
        ((category&CATEGORY_CHANSGATES) &&(dynamic_cast<cChannel *>(obj) ||
                                           dynamic_cast<cGate *>(obj))) ||
        ((category&CATEGORY_OTHERS) && (!dynamic_cast<cModule *>(obj) &&
                                        !dynamic_cast<cMessage *>(obj) &&
                                        !dynamic_cast<cQueue *>(obj) &&
                                        !dynamic_cast<cWatchBase *>(obj) &&
                                        !dynamic_cast<cFSM *>(obj) &&
                                        !dynamic_cast<cOutVector *>(obj) &&
                                        !dynamic_cast<cStatistic *>(obj) &&
                                        !dynamic_cast<cModulePar *>(obj) &&
                                        !dynamic_cast<cChannel *>(obj) &&
                                        !dynamic_cast<cGate *>(obj)));
    if (objfullpathpattern || classnamepattern)
    {
        MatchableObjectAdapter objAdapter(MatchableObjectAdapter::FULLPATH, obj);
        ok = ok && (!objfullpathpattern || objfullpathpattern->matches(&objAdapter));
        objAdapter.setDefaultAttribute(MatchableObjectAdapter::CLASSNAME);
        ok = ok && (!classnamepattern || classnamepattern->matches(&objAdapter));
    }

    if (ok)
    {
        addPointer(obj);
    }

    // go to children
    obj->forEachChild(this);
}

//----------------------------------------------------------------

void cCollectChildrenVisitor::visit(cObject *obj)
{
    if (obj==parent)
        obj->forEachChild(this);
    else
        addPointer(obj);
}

//----------------------------------------------------------------

void cCountChildrenVisitor::visit(cObject *obj)
{
    if (obj==parent)
        obj->forEachChild(this);
    else
        count++;
}

//----------------------------------------------------------------
// utilities for sorting objects:

#define OBJPTR(a) (*(cObject **)a)
static int qsort_cmp_byname(const void *a, const void *b)
{
    return opp_strcmp(OBJPTR(a)->fullName(), OBJPTR(b)->fullName());
}
static int qsort_cmp_byfullpath(const void *a, const void *b)
{
    char buf1[MAX_OBJECTFULLPATH];
    char buf2[MAX_OBJECTFULLPATH];
    return opp_strcmp(OBJPTR(a)->fullPath(buf1,MAX_OBJECTFULLPATH), OBJPTR(b)->fullPath(buf2,MAX_OBJECTFULLPATH));
}
static int qsort_cmp_byclass(const void *a, const void *b)
{
    return opp_strcmp(OBJPTR(a)->className(), OBJPTR(b)->className());
}
#undef OBJPTR

void sortObjectsByName(cObject **objs, int n)
{
    qsort(objs, n, sizeof(cObject*), qsort_cmp_byname);
}

void sortObjectsByFullPath(cObject **objs, int n)
{
    qsort(objs, n, sizeof(cObject*), qsort_cmp_byfullpath);
}

void sortObjectsByClassName(cObject **objs, int n)
{
    qsort(objs, n, sizeof(cObject*), qsort_cmp_byclass);
}


