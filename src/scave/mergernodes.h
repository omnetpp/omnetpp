//=========================================================================
//  MERGERNODES.H - part of
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

#ifndef __OMNETPP_SCAVE_MERGERNODES_H
#define __OMNETPP_SCAVE_MERGERNODES_H

#include "commonnodes.h"

#include <vector>
#include "node.h"
#include "nodetype.h"

namespace omnetpp {
namespace scave {


/**
 * Processing node which merges several input streams into one.
 */
class SCAVE_API MergerNode : public Node
{
    public:
        typedef std::vector<Port> PortVector;
        Port out;
        PortVector ports;
    public:
        MergerNode() : out(this) {}
        virtual ~MergerNode() {}

        Port *addPort();

        virtual bool isReady() const override;
        virtual void process() override;
        virtual bool isFinished() const override;
};

class SCAVE_API MergerNodeType : public NodeType
{
    public:
        virtual const char *getName() const override {return "merger";}
        virtual const char *getCategory() const override {return "merger";}
        virtual const char *getDescription() const override;
        virtual bool isHidden() const override {return true;}
        virtual void getAttributes(StringMap& attrs) const override;
        virtual Node *create(DataflowManager *mgr, StringMap& attrs) const override;
        virtual Port *getPort(Node *node, const char *portname) const override;
};

/**
 * Processing node which aggregates several input streams into one.
 *
 * It can compute the average/count/minimum/maximum of its inputs.
 *
 * TODO: compute all statistics at once.
 */
class SCAVE_API AggregatorNode : public Node
{
    private:
        enum Function
        {
            Sum, Average, Count, Minimum, Maximum
        };

        Function fn;
        double acc;
        int count;

        void init();
        void collect(double value);
        double result();
    public:
        typedef std::vector<Port> PortVector;
        Port out;
        PortVector ports;
    public:
        AggregatorNode(const std::string& function);
        virtual ~AggregatorNode() {}

        Port *addPort();

        virtual bool isReady() const override;
        virtual void process() override;
        virtual bool isFinished() const override;
};

class SCAVE_API AggregatorNodeType : public NodeType
{
    public:
        virtual const char *getName() const override {return "aggregator";}
        virtual const char *getCategory() const override {return "merger";}
        virtual const char *getDescription() const override;
        virtual bool isHidden() const override {return true;}
        virtual void getAttributes(StringMap& attrs) const override;
        virtual void getAttrDefaults(StringMap& attrs) const override;
        virtual void validateAttrValues(const StringMap& attrs) const override;
        virtual Node *create(DataflowManager *mgr, StringMap& attrs) const override;
        virtual Port *getPort(Node *node, const char *portname) const override;
};

} // namespace scave
}  // namespace omnetpp


#endif



