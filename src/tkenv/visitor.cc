//==========================================================================
//  VISITOR.CC -
//            for the Tcl/Tk windowing environment of
//                            OMNeT++
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2003 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "cenvir.h"
#include "cobject.h"
#include "patmatch.h"
#include "visitor.h"



static bool do_foreach_child_call_visitor(cObject *obj, bool beg, cObject *_parent, cVisitor *_visitor)
{
    static cVisitor *visitor;
    static cObject *parent;
    if (!obj) {       // setup
         visitor = _visitor;
         parent = _parent;
         return false;
    }
    if (beg && obj==parent)
        return true;
    if (beg && obj!=parent)
        visitor->visit(obj);
    return false;
}

void cVisitor::traverseChildrenOf(cObject *obj)
{
    do_foreach_child_call_visitor(0,false,obj,this);
    obj->forEach((ForeachFunc)do_foreach_child_call_visitor);
}

//-----------------------------------------------------------------------

cCollectObjectsVisitor::cCollectObjectsVisitor()
{
    size=16;
    arr = new cObject *[size];
    firstfree=0;
}

cCollectObjectsVisitor::~cCollectObjectsVisitor()
{
    delete arr;
}

void cCollectObjectsVisitor::addPointer(cObject *obj)
{
    // if array is full, reallocate
    if (size==firstfree)
    {
        cObject **arr2 = new cObject *[2*size];
        for (int i=0; i<firstfree; i++) arr2[i] = arr[i];
        delete [] arr;
        arr = arr2;
        size = 2*size;
    }

    // add pointer to array
    arr[firstfree++] = obj;
}

void cCollectObjectsVisitor::visit(cObject *obj)
{
    addPointer(obj);

    // go to children
    traverseChildrenOf(obj);
}

//-----------------------------------------------------------------------

cFilteredCollectObjectsVisitor::cFilteredCollectObjectsVisitor()
{
    classnamepatterntf = NULL;
    objfullpathpatterntf = NULL;
}

cFilteredCollectObjectsVisitor::~cFilteredCollectObjectsVisitor()
{
    delete classnamepatterntf;
    delete objfullpathpatterntf;
}

bool cFilteredCollectObjectsVisitor::setFilterPars(const char *classnamepattern,
                                                   const char *objfullpathpattern)
{
    if (classnamepattern && classnamepattern[0])
    {
        classnamepatterntf = new short[512]; // FIXME!
        if (!transform_pattern(classnamepattern,classnamepatterntf))
            return false; // bad pattern: unmatched '{'
    }
    if (objfullpathpattern && objfullpathpattern[0])
    {
        objfullpathpatterntf = new short[512]; // FIXME!
        if (!transform_pattern(objfullpathpattern,objfullpathpatterntf))
            return false; // bad pattern: unmatched '{'
    }
    return true;
}

void cFilteredCollectObjectsVisitor::visit(cObject *obj)
{
    const char *fullpath = obj->fullPath();
    const char *classname = obj->className();
    bool nameok = !objfullpathpatterntf || stringmatch(objfullpathpatterntf,fullpath);
    bool classok = !classnamepatterntf || stringmatch(classnamepatterntf,classname);
    if (nameok && classok)
    {
        addPointer(obj);
    }

    // go to children
    traverseChildrenOf(obj);
}

//----------------------------------------------------------------

void cCollectChildrenVisitor::visit(cObject *obj)
{
    if (obj==parent)
        traverseChildrenOf(obj);
    else
        addPointer(obj);
}

//----------------------------------------------------------------

void cCountChildrenVisitor::visit(cObject *obj)
{
    if (obj==parent)
        traverseChildrenOf(obj);
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


