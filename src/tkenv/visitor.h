//==========================================================================
//  VISITOR.H - part of
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

#ifndef __VISITOR_H
#define __VISITOR_H

#include "cenvir.h"
#include "cobject.h"
#include "cvisitor.h"
#include "tkapp.h"
#include "patternmatcher.h"
#include "matchexpression.h"
#include "matchableobject.h"



/**
 * Adds the capability of storing object pointers to cVisitor.
 *
 * Sample code:
 *   cCollectObjectsVisitor v;
 *   v.visit(object);
 *   cObject **objs = v.getArray();
 */
class cCollectObjectsVisitor : public cVisitor
{
  private:
    int sizelimit;
    cObject **arr;
    int count;
    int size;

  protected:
    // Used during visiting process
    virtual void visit(cObject *obj);
    void addPointer(cObject *obj);

  public:
    cCollectObjectsVisitor();
    virtual ~cCollectObjectsVisitor();
    void setSizeLimit(int limit);
    cObject **getArray()  {return arr;}
    int getArraySize()  {return count;}
};


/* Category constants for cFilteredCollectObjectsVisitor::setFilterPars() */
#define CATEGORY_MODULES     0x01
#define CATEGORY_QUEUES      0x02
#define CATEGORY_STATISTICS  0x04
#define CATEGORY_MESSAGES    0x08
#define CATEGORY_VARIABLES   0x10
#define CATEGORY_MODPARAMS   0x20
#define CATEGORY_CHANSGATES  0x40
#define CATEGORY_OTHERS      0x80

/**
 * Traverses an object tree, and collects objects that belong to certain
 * "categories" (see CATEGORY_... constants) and their className()
 * and/or fullPath() matches a pattern. Used by the Tkenv "Object Filter" dialog.
 */
class cFilteredCollectObjectsVisitor : public cCollectObjectsVisitor
{
  private:
    unsigned int category;
    MatchExpression *classnamepattern;
    MatchExpression *objfullpathpattern;
  protected:
    virtual void visit(cObject *obj);
  public:
    cFilteredCollectObjectsVisitor();
    ~cFilteredCollectObjectsVisitor();
    /**
     * Category is the binary OR'ed value of CATEGORY_... constants.
     * The other two are glob-style patterns.
     */
    void setFilterPars(unsigned int category,
                       const char *classnamepattern,
                       const char *objfullpathpattern);
};

/**
 * Visitor to collect the children of an object only (depth==1).
 */
class cCollectChildrenVisitor : public cCollectObjectsVisitor
{
  private:
    cObject *parent;
  protected:
    virtual void visit(cObject *obj);
  public:
    cCollectChildrenVisitor(cObject *_parent) {parent = _parent;}
};

/**
 * Visitor to count the number of children of an object.
 */
class cCountChildrenVisitor : public cVisitor
{
  private:
    cObject *parent;
    int count;
  protected:
    virtual void visit(cObject *obj);
  public:
    cCountChildrenVisitor(cObject *_parent) {parent = _parent; count=0;}
    int getCount() {return count;}
};


//----------------------------------------------------------------
// utilities for sorting objects:

void sortObjectsByName(cObject **objs, int n);
void sortObjectsByFullPath(cObject **objs, int n);
void sortObjectsByClassName(cObject **objs, int n);

#endif


