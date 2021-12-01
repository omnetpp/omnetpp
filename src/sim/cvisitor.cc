//=========================================================================
//  CVISITOR.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//   Member functions of
//    cVisitor:  enables traversing the object tree
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "omnetpp/cvisitor.h"
#include "omnetpp/cobject.h"

namespace omnetpp {

void cVisitor::process(cObject *obj)
{
    visit(obj);
}

void cVisitor::processChildrenOf(cObject *obj)
{
    obj->forEachChild(this);
}

}  // namespace omnetpp

