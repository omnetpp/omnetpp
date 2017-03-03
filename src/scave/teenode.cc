//=========================================================================
//  TEENODE.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cstring>
#include "channel.h"
#include "teenode.h"

namespace omnetpp {
namespace scave {

Port *TeeNode::addPort()
{
    outs.push_back(Port(this));
    return &(outs.back());
}

bool TeeNode::isReady() const
{
    return in()->length() > 0;
}

void TeeNode::process()
{
    Datum d;

    int n = in()->length();
    for (int i = 0; i < n; i++) {
        in()->read(&d, 1);
        for (PortVector::iterator it = outs.begin(); it != outs.end(); ++it)
            (*it)()->write(&d, 1);
    }
}

bool TeeNode::isFinished() const
{
    return in()->eof();
}

//-------

const char *TeeNodeType::getDescription() const
{
    return "Splits the input into several output by copying its data.\n";
}

void TeeNodeType::getAttributes(StringMap& attrs) const
{
}

Node *TeeNodeType::create(DataflowManager *mgr, StringMap& attrs) const
{
    checkAttrNames(attrs);

    Node *node = new TeeNode();
    node->setNodeType(this);
    mgr->addNode(node);
    return node;
}

Port *TeeNodeType::getPort(Node *node, const char *portname) const
{
    TeeNode *node1 = dynamic_cast<TeeNode *>(node);
    if (!strcmp(portname, "next-out"))
        return node1->addPort();
    else if (!strcmp(portname, "in"))
        return &(node1->in);
    throw opp_runtime_error("No such port '%s'", portname);
}

}  // namespace scave
}  // namespace omnetpp

