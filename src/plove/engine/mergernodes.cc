//=========================================================================
//  MERGERNODES.CC - part of
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

#include "channel.h"
#include "mergernodes.h"


Port *MergerNode::addPort()
{
    ports.push_back(Port(this));
    return &(ports.back());
}

bool MergerNode::isReady() const
{
    // every input port must have data available (or already have reached EOF)
    for (PortVector::const_iterator it=ports.begin(); it!=ports.end(); it++)
        if ((*it)()->length()==0 && !(*it)()->closing())
            return false;
    return true;
}

void MergerNode::process()
{
    // must maintain increasing time order in output, so:
    // always read from the port with smallest time value coming --
    // if port has reached EOF, skip it
    Port *minPort = NULL;
    const Datum *minDatum;
    for (PortVector::iterator it=ports.begin(); it!=ports.end(); it++)
    {
        Channel *chan = (*it)();
        const Datum *dp = chan->peek();
        if (dp && (!minPort || dp->x < minDatum->x))
        {
            minPort = &(*it);
            minDatum = dp;
        }
    }

    // if we couldn't get any data, all input ports must be at EOF (see isReady())
    if (minPort)
    {
        Datum d;
        (*minPort)()->read(&d,1);
        out()->write(&d,1);
    }
}

bool MergerNode::finished() const
{
    // only finished if all ports are at EOF
    for (PortVector::const_iterator it=ports.begin(); it!=ports.end(); it++)
        if (!(*it)()->eof())
            return false;
    return true;
}

//-------

const char *MergerNodeType::description() const
{
    return "Merges several series into a single one, maintaining increasing\n"
           "time order in the output.";
}

void MergerNodeType::getAttributes(StringMap& attrs) const
{
}

Node *MergerNodeType::create(DataflowManager *, StringMap& attrs) const
{
    checkAttrNames(attrs);

    Node *node = new MergerNode();
    node->setNodeType(this);
    return node;
}

Port *MergerNodeType::getPort(Node *node, const char *portname) const
{
    MergerNode *node1 = dynamic_cast<MergerNode *>(node);
    if (!strcmp(portname,"next-in"))
        return node1->addPort();
    else if (!strcmp(portname,"out"))
        return &(node1->out);
    throw new Exception("no such port `%s'", portname);
}



