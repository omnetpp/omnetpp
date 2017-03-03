//=========================================================================
//  DIFFQUOT.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2005-2017 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "channel.h"
#include "diffquot.h"

namespace omnetpp {
namespace scave {

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

    if (!firstRead) {
        in()->read(&l, 1);
        firstRead = true;
    }

    do {
        in()->read(&r, 1);
        // Make sure that the second values has a later time value
        if (r.x > l.x) {
            o.x = l.x;
            o.y = (r.y - l.y) / (r.x - l.x);
            out()->write(&o, 1);
            l = r;
        }
    } while (in()->length() > 0);
}

//-----

const char *DifferenceQuotientNodeType::getDescription() const
{
    return "Calculates the difference quotient of every value and the subsequent one:\n"
           "yout[k] = (y[k+1]-y[k]) / (t[k+1]-t[k])";
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

void DifferenceQuotientNodeType::mapVectorAttributes(  /*inout*/ StringMap& attrs,  /*out*/ StringVector& warnings) const
{
    attrs["type"] = "double";
}

}  // namespace scave
}  // namespace omnetpp

