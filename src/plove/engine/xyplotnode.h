//=========================================================================
//  XYPLOTNODE.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef _XYPLOTNODE_H_
#define _XYPLOTNODE_H_

#include "commonnodes.h"

#include <vector>
#include "node.h"
#include "nodetype.h"


/**
 * Creates x-y plot. Has one "x" and several "y" input ports. Values of these
 * ports are grouped by time -- when (t,x) and (t',y) arrive where t==t', it
 * outputs (x,y). (The t, t' times don't appear in the output).
 *
 * Note: the output will *not* be ordered by x, so one should be very cautious
 * with further processing. Connecting an ArrayBuilder is fine.
 */
class XYPlotNode : public Node
{
    public:
        typedef std::vector<Port> PortVector;
        Port xin;
        PortVector yin;
        PortVector out;
    public:
        XYPlotNode() : xin(this) {}
        virtual ~XYPlotNode() {}

        Port *portY(int k);
        Port *portOut(int k);

        virtual bool isReady() const;
        virtual void process();
        virtual bool finished() const;
};

class XYPlotNodeType : public NodeType
{
    public:
        virtual const char *name() const {return "xyplot";}
        virtual const char *category() const {return "merger";}
        virtual const char *description() const;
        virtual bool isHidden() const {return true;}
        virtual void getAttributes(StringMap& attrs) const;
        virtual Node *create(DataflowManager *, StringMap& attrs) const;
        virtual Port *getPort(Node *node, const char *portname) const;
};

#endif



