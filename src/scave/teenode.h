//=========================================================================
//  TEENODE.H - part of
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

#ifndef __OMNETPP_SCAVE_TEENODE_H
#define __OMNETPP_SCAVE_TEENODE_H

#include "commonnodes.h"

#include <vector>
#include "node.h"
#include "nodetype.h"

namespace omnetpp {
namespace scave {


/**
 * Processing node splits an input stream into several output streams.
 */
class SCAVE_API TeeNode : public Node
{
    public:
        typedef std::vector<Port> PortVector;
        Port in;
        PortVector outs;
    public:
        TeeNode() : in(this) {}
        virtual ~TeeNode() {}

        Port *addPort();

        virtual bool isReady() const override;
        virtual void process() override;
        virtual bool isFinished() const override;
};

class SCAVE_API TeeNodeType : public NodeType
{
    public:
        virtual const char *getName() const override {return "tee";}
        virtual const char *getCategory() const override {return "tee";}
        virtual const char *getDescription() const override;
        virtual bool isHidden() const override {return true;}
        virtual void getAttributes(StringMap& attrs) const override;
        virtual Node *create(DataflowManager *mgr, StringMap& attrs) const override;
        virtual Port *getPort(Node *node, const char *portname) const override;
};

} // namespace scave
}  // namespace omnetpp


#endif



