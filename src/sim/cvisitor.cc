//=========================================================================
//
//  CVISITOR.CC - part of
//                          OMNeT++
//           Discrete System Simulation in C++
//
//   Member functions of
//    cVisitor:  enables traversing the object tree
//
//  Author: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2004 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "cvisitor.h"


bool cVisitor::do_foreach_child_call_visitor(cObject *obj, bool beg, cObject *_parent, cVisitor *_visitor)
{
    static cVisitor *visitor;
    static cObject *parent;
    if (!obj)
    {
         // setup
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

