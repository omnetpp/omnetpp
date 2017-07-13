//==========================================================================
//  CVISITOR.H - part of
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

#ifndef __OMNETPP_CVISITOR_H
#define __OMNETPP_CVISITOR_H

#include "simkerneldefs.h"

namespace omnetpp {

class cObject;


/**
 * @brief Enables traversing the tree of (cObject-rooted) simulation objects.
 *
 * Subclasses should redefine the visit() method and add the desired
 * functionality.
 *
 * An example: the cRecursiveObjectFinderVisitor class (internal) has been
 * created to find an object of the given name. The constructor accepts
 * the name of the object to search for, and visit() has been redefined
 * to perform recursive traversal, and to throw EndTraversalException
 * if it finds the given object. The class can be used like this:
 *
 * \code
 * cRecursiveObjectFinderVisitor v(objectName);
 * v.process(root);
 * cObject *result = v.getResult();
 * \endcode
 *
 * The above code will find the root object as well, if its name matches.
 * The second version (below) starts the traversal at the children,
 * so the root object is ignored.
 *
 * \code
 * cRecursiveObjectFinderVisitor v(objectName);
 * v.processChildrenOf(root);
 * cObject *result = v.getResult();
 * \endcode
 *
 * @ingroup SimSupport
 */
class SIM_API cVisitor
{
  protected:
    /**
     * @brief Can be thrown to get out in the middle of the traversal process.
     */
     class EndTraversalException { };

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
     * Similar to process(), except that it skips the object itself, i.e. it begins
     * with processing the children. Calls obj->forEachChild(this),
     * and catches EndTraversalException if one occurs. Return value is true if
     * traversal went through and false if EndTraversalException was caught.
     */
    virtual bool processChildrenOf(cObject *obj);

    /**
     * Method called from the forEachChild() methods of every class derived
     * from cObject, for each contained object. visit() should be
     * redefined by user to encapsulate the operation to be performed
     * on the object. If you want recursively traversal, call
     * obj->forEachChild(this) from here.
     *
     * This method should NOT be called to to initiate the traversal -- use
     * process() or processChildrenOf() for that.
     */
    virtual void visit(cObject *obj) = 0;
};

}  // namespace omnetpp


#endif


