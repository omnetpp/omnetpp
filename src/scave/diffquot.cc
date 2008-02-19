//=========================================================================
//  DIFFQUOT.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "channel.h"
#include "diffquot.h"

USING_NAMESPACE


DifferenceQuotientNode::DifferenceQuotientNode()
{
    firstRead = false;
}

bool DifferenceQuotientNode::isReady() const
{
    return in()->length() > 0;
}

void DifferenceQuotientNode::process()
{
    Datum r, o;

    if (!firstRead)
    {
        in()->read(&l, 1);
        firstRead = true;
    }

    do {
        in()->read(&r, 1);
        // Make sure that the second values has a later time value
        if (r.x > l.x)
        {
            o.x = l.x;
            o.y = (r.y - l.y) / (r.x - l.x);
            out()->write(&o,1);
            l = r;
        }
    }
    while (in()->length() > 0);
}

//-----

const char *DifferenceQuotientNodeType::description() const
{
    return "Calculates the difference quotient of every value and the subsequent one:\n"
           "y[k] = (y[k+1]-y[k]) / (x[k+1]-x[k])";
}

void DifferenceQuotientNodeType::getAttributes(StringMap& attrs) const
{
}

void DifferenceQuotientNodeType::getAttrDefaults(StringMap& attrs) const
{
}

Node *DifferenceQuotientNodeType::create(DataflowManager *mgr, StringMap& attrs) const
{
    checkAttrNames(attrs);

    Node *node = new DifferenceQuotientNode();
    node->setNodeType(this);
    mgr->addNode(node);
    return node;
}

void DifferenceQuotientNodeType::mapVectorAttributes(/*inout*/StringMap &attrs) const
{
    attrs["type"] = "double";
}

