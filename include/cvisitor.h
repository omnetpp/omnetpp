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


class cObject;


/**
 * Enables traversing an object tree. Should be subclassed and the visit()
 * method redefined according to your needs.
 *
 * @ingroup SimCore
 */
class cVisitor
{
  protected:
    /**
     * Can be thrown to get out in the middle of the traversal process.
     */
     class EndTraversalException { public: EndTraversalException() {} };

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

    /**
     * Should be redefined by user to encapsulate the operation to be performed
     * on the object. If you want recursively traversal, call 
     * obj->forearchChild(this) from here.
     */
    virtual void visit(cObject *obj) = 0;
};

#endif


