//=========================================================================
//  XYPLOTNODE.H - part of
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

#ifndef __OMNETPP_SCAVE_XYPLOTNODE_H
#define __OMNETPP_SCAVE_XYPLOTNODE_H

#include "commonnodes.h"

#include <vector>
#include "node.h"
#include "nodetype.h"

namespace omnetpp {
namespace scave {


/**
 * Creates x-y plot. Has one "x" and several "y" input ports. Values of these
 * ports are grouped by time -- when (t,x) and (t',y) arrive where t==t', it
 * outputs (x,y). (The t, t' times do not appear in the output).
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

        virtual bool isReady() const override;
        virtual void process() override;
        virtual bool isFinished() const override;
};

class SCAVE_API XYPlotNodeType : public NodeType
{
    public:
        virtual const char *getName() const override {return "xyplot";}
        virtual const char *getCategory() const override {return "merger";}
        virtual const char *getDescription() const override;
        virtual bool isHidden() const override {return true;}
        virtual void getAttributes(StringMap& attrs) const override;
        virtual Node *create(DataflowManager *mgr, StringMap& attrs) const override;
        virtual Port *getPort(Node *node, const char *portname) const override;
};

} // namespace scave
}  // namespace omnetpp


#endif



