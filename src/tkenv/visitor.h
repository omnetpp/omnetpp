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



/**
 * cVisitor base class
 *
 * FIXME after this class boils down, should be moved to sim/.
 */
class cVisitor
{
  private:
     // wraps old cObject::foreach()
     static bool do_foreach_child_call_visitor(cObject *obj, bool beg, cObject *_parent, cVisitor *_visitor);

  protected:
    /**
     * Can be thrown to get out in the middle of the traversal process.
     */
     class EndTraversalException { public: EndTraversalException() {} };

    /**
     * Called on each immediate child object. Should be redefined by user.
     */
    virtual void visit(cObject *obj) = 0;

    /**
     * Emulate cObject::foreachChild() with the foreach() we have...
     */
    virtual void traverseChildrenOf(cObject *obj);

  public:
    /**
     * Virtual destructor.
     */
    virtual ~cVisitor() {}

    /**
     * Starts the visiting process. This version simply calls visit(obj).
     * It also catches EndTraversalException. Return value is true if
     * traversal went through and false if EndTraversalException was caught.
     */
    virtual bool process(cObject *obj);
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
 *
 */
class cFilteredCollectObjectsVisitor : public cCollectObjectsVisitor
{
  private:
    unsigned int category;
    short *classnamepatterntf;
    short *objfullpathpatterntf;
  protected:
    virtual void visit(cObject *obj);
  public:
    cFilteredCollectObjectsVisitor();
    ~cFilteredCollectObjectsVisitor();
    /**
     * Category is the binary OR'ed value of CATEGORY_... constants.
     * The other two are glob-style patterns.
     */
    bool setFilterPars(unsigned int category,
                       const char *classnamepattern,
                       const char *objfullpathpattern);
};

/**
 *
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
 *
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


