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
    yin.push_back(a);
    return &(yin.back());
}

bool XYPlotNode::isReady() const
{
    // all input ports must have something (except those at EOF)
    if (xin()->length()==0 && !xin()->closing())
        return false;
    for (PortVector::const_iterator it=yin.begin(); it!=yin.end(); it++)
        if ((*it)()->length()==0 && !(*it)()->closing())
            return false;
    return true;
}

void XYPlotNode::process()
{
    // read one value from "x" port, then read "y" ports until their peek()'s
    // return ty>tx values (where tx is timestamp of the "x" value). Meanwhile,
    // if we find ty==tx values, output the (x,y) pair. If a port has reached
    // EOF, skip it.
    if (xin()->length()==0)
    {
         // "x" at EOF, discard all "y" values
         ASSERT(xin()->eof()); // isReady() guarantees this
         for (PortVector::const_iterator it=yin.begin(); it!=yin.end(); it++)
             (*it)()->flush();
         if (finished())
             out()->close();
         return;
    }

    Datum xd;
    xin()->read(&xd,1);

    for (PortVector::iterator it=yin.begin(); it!=yin.end(); it++)
    {
        Channel *ychan = (*it)();
        const Datum *yp;
        Datum d;
        while ((yp=ychan->peek())!=NULL && yp->x < xd.x)
        {
            ychan->read(&d,1);
        }
        while ((yp=ychan->peek())!=NULL && yp->x == xd.x)
        {
            ychan->read(&d,1);
            d.x = xd.y;
            out()->write(&d,1);
        }
    }

    if (finished())
        out()->close();
}

bool XYPlotNode::finished() const
{
    // only finished if all ports are at EOF
    if (!xin()->eof())
        return false;
    for (PortVector::const_iterator it=yin.begin(); it!=yin.end(); it++)
        if (!(*it)()->eof())
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
        return &(node1->xin);
    if (!strcmp(portname,"next-y"))
        return node1->addYPort();
    else if (!strcmp(portname,"out"))
        return &(node1->out);
    throw new Exception("no such port `%s'", portname);
}



