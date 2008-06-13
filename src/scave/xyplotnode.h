//=========================================================================
//  XYPLOTNODE.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef _XYPLOTNODE_H_
#define _XYPLOTNODE_H_

#include "commonnodes.h"

#include <vector>
#include "node.h"
#include "nodetype.h"

NAMESPACE_BEGIN


/**
 * Creates x-y plot. Has one "x" and several "y" input ports. Values of these
 * ports are grouped by time -- when (t,x) and (t',y) arrive where t==t', it
 * outputs (x,y). (The t, t' times don't appear in the output).
 *
 * Note: the output will *not* be ordered by x, so one should be very cautious
 * with further processing. Connecting an ArrayBuilder is fine.
 */
class SCAVE_API XYPlotNode : public Node
{
    public:
        typedef std::vector<Port> PortVector;
        Port xin;
        PortVector yin;
        PortVector out;
    public:
        XYPlotNode() : xin(this) {}
        virtual ~XYPlotNode() {}

        Port *getPortY(int k);
        Port *getPortOut(int k);

        virtual bool isReady() const;
        virtual void process();
        virtual bool isFinished() const;
};

class SCAVE_API XYPlotNodeType : public NodeType
{
    public:
        virtual const char *getName() const {return "xyplot";}
        virtual const char *getCategory() const {return "merger";}
        virtual const char *getDescription() const;
        virtual bool isHidden() const {return true;}
        virtual void getAttributes(StringMap& attrs) const;
        virtual Node *create(DataflowManager *mgr, StringMap& attrs) const;
        virtual Port *getPort(Node *node, const char *portname) const;
};

NAMESPACE_END


#endif



