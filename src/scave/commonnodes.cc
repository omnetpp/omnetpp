//=========================================================================
//  COMMONNODES.CC - part of
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
#include "commonnodes.h"
#include "channel.h"

namespace omnetpp {
namespace scave {

Port *SingleSourceNodeType::getPort(Node *node, const char *name) const
{
    SingleSourceNode *node1 = dynamic_cast<SingleSourceNode *>(node);
    if (!strcmp(name, "out"))
        return &(node1->out);
    throw opp_runtime_error("No such port '%s'", name);
}

Port *SingleSinkNodeType::getPort(Node *node, const char *name) const
{
    SingleSinkNode *node1 = dynamic_cast<SingleSinkNode *>(node);
    if (!strcmp(name, "in"))
        return &(node1->in);
    throw opp_runtime_error("No such port '%s'", name);
}

bool FilterNode::isFinished() const
{
    return in()->eof();
}

Port *FilterNodeType::getPort(Node *node, const char *name) const
{
    FilterNode *node1 = dynamic_cast<FilterNode *>(node);
    if (!strcmp(name, "in"))
        return &(node1->in);
    else if (!strcmp(name, "out"))
        return &(node1->out);
    throw opp_runtime_error("No such port '%s'", name);
}

}  // namespace scave
}  // namespace omnetpp

