//=========================================================================
//  CVISITOR.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//   Member functions of
//    cVisitor:  enables traversing the object tree
//
//  Author: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "cvisitor.h"
#include "cobject.h"


bool cVisitor::process(cObject *obj)
{
    try
    {
        visit(obj);
    }
    catch (EndTraversalException e)
    {
        return false;
    }
    return true;
}

bool cVisitor::processChildrenOf(cObject *obj)
{
    try
    {
        obj->forEachChild(this);
    }
    catch (EndTraversalException e)
    {
        return false;
    }
    return true;
}
