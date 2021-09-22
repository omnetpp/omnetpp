//==========================================================================
//  VISITOR.H - part of
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

#ifndef __OMNETPP_ENVIR_VISITOR_H
#define __OMNETPP_ENVIR_VISITOR_H

#include "common/patternmatcher.h"
#include "common/matchexpression.h"
#include "omnetpp/cenvir.h"
#include "omnetpp/cobject.h"
#include "omnetpp/cvisitor.h"
#include "envirdefs.h"
#include "matchableobject.h"

namespace omnetpp {
namespace envir {


/**
 * Adds the capability of storing object pointers to cVisitor.
 *
 * Sample code:
 *   cCollectObjectsVisitor v;
 *   v.visit(object);
 *   cObject **objs = v.getArray();
 */
class ENVIR_API cCollectObjectsVisitor : public cVisitor
{
  private:
    int sizeLimit;
    cObject **objs;
    int numObjs;
    int capacity;

  protected:
    // Used during visiting process
    virtual bool visit(cObject *obj) override;
    bool isFull() const {return sizeLimit && numObjs >= sizeLimit;}
    void addPointer(cObject *obj);

  public:
    cCollectObjectsVisitor();
    virtual ~cCollectObjectsVisitor();
    void setSizeLimit(int limit);
    cObject **getArray()  {return objs;}
    int getArraySize()  {return numObjs;}
};


/* Category constants for cFilteredCollectObjectsVisitor::setFilterPars() */
#define CATEGORY_ALL         (~0U)
#define CATEGORY_MODULES     0x01
#define CATEGORY_QUEUES      0x02
#define CATEGORY_STATISTICS  0x04
#define CATEGORY_MESSAGES    0x08
#define CATEGORY_WATCHES     0x10
#define CATEGORY_PARAMS      0x20
#define CATEGORY_CHANSGATES  0x40
#define CATEGORY_FIGURES     0x80
#define CATEGORY_OTHERS      0x100

/**
 * Traverses an object tree, and collects objects that belong to certain
 * "categories" (see CATEGORY_... constants) and their getClassName()
 * and/or getFullPath() matches a pattern. Used by the Qtenv "Find Objects" dialog.
 */
class ENVIR_API cFilteredCollectObjectsVisitor : public cCollectObjectsVisitor
{
  protected:
    typedef omnetpp::common::MatchExpression MatchExpression;
  private:
    unsigned int category;
    MatchExpression *classnamePattern;
    MatchExpression *objFullpathPattern;
  protected:
    virtual bool visit(cObject *obj) override;
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
class ENVIR_API cCollectChildrenVisitor : public cCollectObjectsVisitor
{
  private:
    cObject *parent;
  protected:
    virtual bool visit(cObject *obj) override;
  public:
    cCollectChildrenVisitor(cObject *_parent) {parent = _parent;}
};

/**
 * Visitor to count the number of children of an object.
 */
class ENVIR_API cCountChildrenVisitor : public cVisitor
{
  private:
    cObject *parent;
    int count;
  protected:
    virtual bool visit(cObject *obj) override;
  public:
    cCountChildrenVisitor(cObject *_parent) {parent = _parent; count=0;}
    int getCount() {return count;}
};

/**
 * Visitor to determine whether an object has any children at all.
 */
class ENVIR_API cHasChildrenVisitor : public cVisitor
{
  private:
    cObject *parent;
    bool hasChildren;
  protected:
    virtual bool visit(cObject *obj) override;
  public:
    cHasChildrenVisitor(cObject *_parent) {parent = _parent; hasChildren=false;}
    bool getResult() {return hasChildren;}
};

}  // namespace envir
}  // namespace omnetpp


#endif


