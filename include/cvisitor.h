//==========================================================================
//   CVISITOR.H - header for
//                             OMNeT++
//            Discrete System Simulation in C++
//
//  Declaration of the following classes:
//    cVisitor:  enables traversing the object tree
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2004 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CVISITOR_H
#define __CVISITOR_H

#include "cobject.h"


/**
 * Enables traversing an object tree. Should be subclassed and the visit()
 * method redefined according to your needs.
 *
 * @ingroup SimCore
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
     * Starts the visiting process. This version simply calls visit(obj),
     * and catches EndTraversalException if one occurs. Return value is true if
     * traversal went through and false if EndTraversalException was caught.
     */
    virtual bool process(cObject *obj);
};

#endif


