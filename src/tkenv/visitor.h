//==========================================================================
//  VISITOR.H -
//            for the Tcl/Tk windowing environment of
//                            OMNeT++
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2003 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __VISITOR_H
#define __VISITOR_H

#include "cenvir.h"
#include "cobject.h"
#include "tkapp.h"
#include "patmatch.h"


// FIXME temporary place for visitor stuff -- after it boils down, should be moved to sim/

/**
 * cVisitor base class
 */
class cVisitor
{
  public:
    /**
     * Virtual destructor.
     */
    virtual ~cVisitor() {}
    /**
     * Called on each immediate child object. Should be redefined by user.
     */
    virtual void visit(cObject *obj) = 0;

    /**
     * Emulate cObject::foreachChild() with the foreach() we have...
     */
    virtual void traverseChildrenOf(cObject *obj);
};


/**
 * Sample code:
 *   cCollectObjectsVisitor v;
 *   v.visit(object);
 *   cObject **objs = v.getArray();
 */
class cCollectObjectsVisitor : public cVisitor
{
  private:
    cObject **arr;
    int firstfree;
    int size;
  public:
    cCollectObjectsVisitor();
    ~cCollectObjectsVisitor();
    virtual void visit(cObject *obj);
    void addPointer(cObject *obj);
    cObject **getArray()  {return arr;}
    int getArraySize()  {return firstfree;}
};


/**
 *
 */
class cFilteredCollectObjectsVisitor : public cCollectObjectsVisitor
{
  private:
    short *classnamepatterntf;
    short *objfullpathpatterntf;
  public:
    cFilteredCollectObjectsVisitor();
    ~cFilteredCollectObjectsVisitor();
    bool setFilterPars(const char *classnamepattern, const char *objfullpathpattern);
    virtual void visit(cObject *obj);
};

/**
 *
 */
class cCollectChildrenVisitor : public cCollectObjectsVisitor
{
  private:
    cObject *parent;
  public:
    cCollectChildrenVisitor(cObject *_parent) {parent = _parent;}
    virtual void visit(cObject *obj);
};

/**
 *
 */
class cCountChildrenVisitor : public cVisitor
{
  private:
    cObject *parent;
    int count;
  public:
    cCountChildrenVisitor(cObject *_parent) {parent = _parent; count=0;}
    virtual void visit(cObject *obj);
    int getCount() {return count;}
};


//----------------------------------------------------------------
// utilities for sorting objects:

void sortObjectsByName(cObject **objs, int n);
void sortObjectsByFullPath(cObject **objs, int n);
void sortObjectsByClassName(cObject **objs, int n);

#endif


