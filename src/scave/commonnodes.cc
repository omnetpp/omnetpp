//=========================================================================
//  COMMONNODES.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "commonnodes.h"
#include "channel.h"

USING_NAMESPACE


Port *SingleSourceNodeType::getPort(Node *node, const char *name) const
{
    SingleSourceNode *node1 = dynamic_cast<SingleSourceNode *>(node);
    if (!strcmp(name,"out"))
        return &(node1->out);
    throw opp_runtime_error("no such port `%s'", name);
}

Port *SingleSinkNodeType::getPort(Node *node, const char *name) const
{
    SingleSinkNode *node1 = dynamic_cast<SingleSinkNode *>(node);
    if (!strcmp(name,"in"))
        return &(node1->in);
    throw opp_runtime_error("no such port `%s'", name);
}

bool FilterNode::finished() const
{
    return in()->eof();
}

Port *FilterNodeType::getPort(Node *node, const char *name) const
{
    FilterNode *node1 = dynamic_cast<FilterNode *>(node);
    if (!strcmp(name,"in"))
        return &(node1->in);
    else if (!strcmp(name,"out"))
        return &(node1->out);
    throw opp_runtime_error("no such port `%s'", name);
}




