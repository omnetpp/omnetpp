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

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include "commonnodes.h"
#include "channel.h"


Port *SingleSourceNodeType::getPort(Node *node, const char *name) const
{
    SingleSourceNode *node1 = dynamic_cast<SingleSourceNode *>(node);
    if (!strcmp(name,"out"))
        return &(node1->out);
    throw new Exception("no such port `%s'", name);
}

Port *SingleSinkNodeType::getPort(Node *node, const char *name) const
{
    SingleSinkNode *node1 = dynamic_cast<SingleSinkNode *>(node);
    if (!strcmp(name,"in"))
        return &(node1->in);
    throw new Exception("no such port `%s'", name);
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
    throw new Exception("no such port `%s'", name);
}




