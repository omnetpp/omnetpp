//=========================================================================
//  XYPLOTNODE.CC - part of
//                          OMNeT++
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2003 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include "channel.h"
#include "xyplotnode.h"


Port *XYPlotNode::addYPort()
{
    Port a;
    ports.push_back(a);
    return &(ports.back());
}

bool XYPlotNode::isReady() const
{
    // every input port must have data available (or already have reached EOF)
    for (PortVector::const_iterator it=ports.begin(); it!=ports.end(); it++)
        if ((*it)()->length()==0 && !(*it)()->closing())
            return false;
    return true;
}

void XYPlotNode::process()
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
    if (!minPort)
    {
        out()->close();
    }
    else
    {
        Datum d;
        (*minPort)()->read(&d,1);
        out()->write(&d,1);
    }
}

bool XYPlotNode::finished() const
{
    // only finished if all ports are at EOF
    for (PortVector::const_iterator it=ports.begin(); it!=ports.end(); it++)
        if (!(*it)()->closing() || (*it)()->length()>0)
            return false;
    return true;
}

//-------

const char *XYPlotNodeType::description() const
{
    return "Creates x-y plot by matching values that arrive on one x and several "
           "y ports by their timestamps. (One value on x and one on y create one "
           "output value when their timestamps are equal).";
}

void XYPlotNodeType::getAttributes(StringMap& attrs) const
{
}

Node *XYPlotNodeType::create(DataflowManager *, StringMap& attrs) const
{
    checkAttrNames(attrs);

    Node *node = new XYPlotNode();
    node->setNodeType(this);
    return node;
}

Port *XYPlotNodeType::getPort(Node *node, const char *portname) const
{
    XYPlotNode *node1 = dynamic_cast<XYPlotNode *>(node);
    if (!strcmp(portname,"x"))
        return &(node1->x);
    if (!strcmp(portname,"next-y"))
        return node1->addYPort();
    else if (!strcmp(portname,"out"))
        return &(node1->out);
    throw new Exception("no such port `%s'", portname);
}



