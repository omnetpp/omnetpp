//==========================================================================
//  VISITOR.CC - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cstdio>
#include <cstring>
#include <cassert>

#include "omnetpp/cenvir.h"
#include "omnetpp/cobject.h"
#include "omnetpp/cmodule.h"
#include "omnetpp/cmessage.h"
#include "omnetpp/cqueue.h"
#include "omnetpp/cstatistic.h"
#include "omnetpp/coutvector.h"
#include "omnetpp/cwatch.h"
#include "omnetpp/cfsm.h"
#include "omnetpp/cpar.h"
#include "omnetpp/cchannel.h"
#include "omnetpp/cgate.h"
#include "omnetpp/ccanvas.h"
#include "omnetpp/cosgcanvas.h"
#include "omnetpp/cresultrecorder.h"

#include "common/patternmatcher.h"
#include "visitor.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace envir {

cCollectObjectsVisitor::cCollectObjectsVisitor()
{
    sizeLimit = 0;  // no limit by default
    capacity = 16;
    objs = new cObject *[capacity];
    numObjs = 0;
}

cCollectObjectsVisitor::~cCollectObjectsVisitor()
{
    delete[] objs;
}

void cCollectObjectsVisitor::setSizeLimit(int limit)
{
    sizeLimit = limit;
}

void cCollectObjectsVisitor::addPointer(cObject *obj)
{
    if (sizeLimit && numObjs == sizeLimit)
        throw EndTraversalException();

    // if array is full, reallocate
    if (numObjs == capacity) {
        cObject **arr2 = new cObject *[2*capacity];
        for (int i = 0; i < numObjs; i++)
            arr2[i] = objs[i];
        delete[] objs;
        objs = arr2;
        capacity = 2*capacity;
    }

    // add pointer to array
    objs[numObjs++] = obj;
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
    classnamePattern = nullptr;
    objFullpathPattern = nullptr;
}

cFilteredCollectObjectsVisitor::~cFilteredCollectObjectsVisitor()
{
    delete classnamePattern;
    delete objFullpathPattern;
}

void cFilteredCollectObjectsVisitor::setFilterPars(unsigned int cat,
        const char *classnamepatt,
        const char *objfullpathpatt)
{
    // Note: pattern matcher will throw exception on pattern syntax error
    category = cat;
    if (classnamepatt && classnamepatt[0])
        classnamePattern = new MatchExpression(classnamepatt, false, true, true);

    if (objfullpathpatt && objfullpathpatt[0])
        objFullpathPattern = new MatchExpression(objfullpathpatt, false, true, true);
}

void cFilteredCollectObjectsVisitor::visit(cObject *obj)
{
    bool ok = (category == ~0U) ||
        ((category&CATEGORY_MODULES) && dynamic_cast<cModule *>(obj)) ||
        ((category&CATEGORY_MESSAGES) && dynamic_cast<cMessage *>(obj)) ||
        ((category&CATEGORY_QUEUES) && dynamic_cast<cQueue *>(obj)) ||
        ((category&CATEGORY_WATCHES) && (dynamic_cast<cWatchBase *>(obj) ||
                                         dynamic_cast<cFSM *>(obj))) ||
        ((category&CATEGORY_STATISTICS) && (dynamic_cast<cOutVector *>(obj) ||
                                            dynamic_cast<cResultRecorder *>(obj) ||
                                            dynamic_cast<cStatistic *>(obj))) ||
        ((category&CATEGORY_PARAMS) && (dynamic_cast<cPar *>(obj))) ||
        ((category&CATEGORY_CHANSGATES) && (dynamic_cast<cChannel *>(obj) ||
                                            dynamic_cast<cGate *>(obj))) ||
        ((category&CATEGORY_FIGURES) && (dynamic_cast<cFigure *>(obj) ||
                                         dynamic_cast<cCanvas *>(obj) ||
                                         dynamic_cast<cOsgCanvas *>(obj))) ||
        ((category&CATEGORY_OTHERS) && (!dynamic_cast<cModule *>(obj) &&
                                        !dynamic_cast<cMessage *>(obj) &&
                                        !dynamic_cast<cQueue *>(obj) &&
                                        !dynamic_cast<cWatchBase *>(obj) &&
                                        !dynamic_cast<cFSM *>(obj) &&
                                        !dynamic_cast<cOutVector *>(obj) &&
                                        !dynamic_cast<cResultRecorder *>(obj) &&
                                        !dynamic_cast<cStatistic *>(obj) &&
                                        !dynamic_cast<cPar *>(obj) &&
                                        !dynamic_cast<cChannel *>(obj) &&
                                        !dynamic_cast<cGate *>(obj) &&
                                        !dynamic_cast<cFigure *>(obj) &&
                                        !dynamic_cast<cCanvas *>(obj) &&
                                        !dynamic_cast<cOsgCanvas *>(obj)));
    if (objFullpathPattern || classnamePattern) {
        MatchableObjectAdapter objAdapter(MatchableObjectAdapter::FULLPATH, obj);
        ok = ok && (!objFullpathPattern || objFullpathPattern->matches(&objAdapter));
        objAdapter.setDefaultAttribute(MatchableObjectAdapter::CLASSNAME);
        ok = ok && (!classnamePattern || classnamePattern->matches(&objAdapter));
    }

    if (ok) {
        addPointer(obj);
    }

    // go to children
    obj->forEachChild(this);
}

//----------------------------------------------------------------

void cCollectChildrenVisitor::visit(cObject *obj)
{
    if (obj == parent)
        obj->forEachChild(this);
    else
        addPointer(obj);
}

//----------------------------------------------------------------

void cCountChildrenVisitor::visit(cObject *obj)
{
    if (obj == parent)
        obj->forEachChild(this);
    else
        count++;
}

//----------------------------------------------------------------

void cHasChildrenVisitor::visit(cObject *obj)
{
    if (obj == parent)
        obj->forEachChild(this);
    else {
        hasChildren = true;
        throw EndTraversalException();
    }
}

//----------------------------------------------------------------
// utilities for sorting objects:

static const char *getObjectShortTypeName(cObject *object)
{
    if (cComponent *component = dynamic_cast<cComponent *>(object))
        return component->getComponentType()->getName();
    return object->getClassName();
}

#define OBJPTR(a)    (*(cObject **)a)
static int qsort_cmp_byname(const void *a, const void *b)
{
    return opp_strcmp(OBJPTR(a)->getFullName(), OBJPTR(b)->getFullName());
}

static int qsort_cmp_byfullpath(const void *a, const void *b)
{
    return opp_strcmp(OBJPTR(a)->getFullPath().c_str(), OBJPTR(b)->getFullPath().c_str());
}

static int qsort_cmp_by_shorttypename(const void *a, const void *b)
{
    return opp_strcmp(getObjectShortTypeName(OBJPTR(a)), getObjectShortTypeName(OBJPTR(b)));
}

#undef OBJPTR

void sortObjectsByName(cObject **objs, int n)
{
    qsort(objs, n, sizeof(cObject *), qsort_cmp_byname);
}

void sortObjectsByFullPath(cObject **objs, int n)
{
    qsort(objs, n, sizeof(cObject *), qsort_cmp_byfullpath);
}

void sortObjectsByShortTypeName(cObject **objs, int n)
{
    qsort(objs, n, sizeof(cObject *), qsort_cmp_by_shorttypename);
}

}  // namespace envir
}  // namespace omnetpp

