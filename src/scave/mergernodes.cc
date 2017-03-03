//=========================================================================
//  MERGERNODES.CC - part of
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

#include <cstring>
#include <algorithm>
#include "channel.h"
#include "scaveutils.h"
#include "mergernodes.h"

namespace omnetpp {
namespace scave {

Port *MergerNode::addPort()
{
    ports.push_back(Port(this));
    return &(ports.back());
}

bool MergerNode::isReady() const
{
    // every input port must have data available (or already have reached EOF)
    for (PortVector::const_iterator it = ports.begin(); it != ports.end(); ++it)
        if ((*it)()->length() == 0 && !(*it)()->isClosing())
            return false;

    return true;
}

void MergerNode::process()
{
    // must maintain increasing time order in output, so:
    // always read from the port with smallest time value coming --
    // if port has reached EOF, skip it
    Port *minPort = nullptr;
    const Datum *minDatum;
    for (PortVector::iterator it = ports.begin(); it != ports.end(); ++it) {
        Channel *chan = (*it)();
        const Datum *dp = chan->peek();
        if (dp && (!minPort || dp->x < minDatum->x)) {
            minPort = &(*it);
            minDatum = dp;
        }
    }

    // if we couldn't get any data, all input ports must be at EOF (see isReady())
    if (minPort) {
        Datum d;
        (*minPort)()->read(&d, 1);
        out()->write(&d, 1);
    }
}

bool MergerNode::isFinished() const
{
    // only finished if all ports are at EOF
    for (PortVector::const_iterator it = ports.begin(); it != ports.end(); ++it)
        if (!(*it)()->eof())
            return false;

    return true;
}

//-------

const char *MergerNodeType::getDescription() const
{
    return "Merges several series into a single one, maintaining increasing\n"
           "time order in the output.";
}

void MergerNodeType::getAttributes(StringMap& attrs) const
{
}

Node *MergerNodeType::create(DataflowManager *mgr, StringMap& attrs) const
{
    checkAttrNames(attrs);

    Node *node = new MergerNode();
    node->setNodeType(this);
    mgr->addNode(node);
    return node;
}

Port *MergerNodeType::getPort(Node *node, const char *portname) const
{
    MergerNode *node1 = dynamic_cast<MergerNode *>(node);
    if (!strcmp(portname, "next-in"))
        return node1->addPort();
    else if (!strcmp(portname, "out"))
        return &(node1->out);
    throw opp_runtime_error("No such port '%s'", portname);
}

//---- Aggregator ------

AggregatorNode::AggregatorNode(const std::string &function)
    : out(this)
{
    if (function == "sum") fn = Sum;
    else if (function == "average") fn = Average;
    else if (function == "count") fn = Count;
    else if (function == "minimum") fn = Minimum;
    else if (function == "maximum") fn = Maximum;
    else fn = Average; //TODO why not error?
}

void AggregatorNode::init()
{
    switch (fn) {
        case Sum: acc = 0.0; break;
        case Average: acc = 0.0; count = 0; break;
        case Count:   count = 0; break;
        case Minimum: acc = POSITIVE_INFINITY; break;
        case Maximum: acc = NEGATIVE_INFINITY; break;
        default: Assert(false);
    }
}

void AggregatorNode::collect(double value)
{
    switch (fn) {
        case Sum: acc += value; break;
        case Average: acc += value; count++; break;
        case Count:   count++; break;
        case Minimum: acc = std::min(value, acc); break;
        case Maximum: acc = std::max(value, acc); break;
        default: Assert(false);
    }
}

double AggregatorNode::result()
{
    switch (fn) {
        case Sum: return acc;
        case Average: return acc/count;
        case Count:   return count;
        case Minimum:
        case Maximum: return acc;
        default: Assert(false); return NaN;
    }
}

Port *AggregatorNode::addPort()
{
    ports.push_back(Port(this));
    return &(ports.back());
}

bool AggregatorNode::isReady() const
{
    // every input port must have data available (or already have reached EOF)
    for (PortVector::const_iterator it = ports.begin(); it != ports.end(); ++it)
        if ((*it)()->length() == 0 && !(*it)()->isClosing())
            return false;

    return true;
}

void AggregatorNode::process()
{
    const Datum *minDatum = nullptr;
    for (PortVector::iterator it = ports.begin(); it != ports.end(); ++it) {
        Channel *chan = (*it)();
        const Datum *dp = chan->peek();
        if (dp && (!minDatum || dp->x < minDatum->x))
            minDatum = dp;
    }

    // if we couldn't get any data, all input ports must be at EOF (see isReady())
    if (minDatum) {
        Datum d;
        d.x = minDatum->x;

        init();
        for (PortVector::iterator it = ports.begin(); it != ports.end(); ++it) {
            Channel *chan = (*it)();
            const Datum *dp = chan->peek();
            if (dp && dp->x == d.x) {
                Datum d2;
                chan->read(&d2, 1);
                collect(d2.y);
            }
        }

        d.y = result();
        out()->write(&d, 1);
    }
}

bool AggregatorNode::isFinished() const
{
    // only finished if all ports are at EOF
    for (PortVector::const_iterator it = ports.begin(); it != ports.end(); ++it)
        if (!(*it)()->eof())
            return false;

    return true;
}

//-------

const char *AggregatorNodeType::getDescription() const
{
    return "Aggregates several vectors into a single one, aggregating the\n"
           "y values at the same time coordinate with the specified function.";
}

void AggregatorNodeType::getAttributes(StringMap& attrs) const
{
    attrs["function"] = "the aggregator function; one of sum,average,count,maximum,minimum";
}

void AggregatorNodeType::getAttrDefaults(StringMap& attrs) const
{
    attrs["function"] = "average";
}

void AggregatorNodeType::validateAttrValues(const StringMap& attrs) const
{
    StringMap::const_iterator it = attrs.find("function");
    if (it != attrs.end()) {
        const std::string& fn = it->second;
        if (fn != "average" && fn != "count" && fn != "minimum" && fn != "maximum")
            throw opp_runtime_error("Unknown aggregator function: %s.", fn.c_str());
    }
}

Node *AggregatorNodeType::create(DataflowManager *mgr, StringMap& attrs) const
{
    checkAttrNames(attrs);

    StringMap::iterator it = attrs.find("function");
    std::string functionName = it == attrs.end() ? "average" : it->second;

    Node *node = new AggregatorNode(functionName);
    node->setNodeType(this);
    mgr->addNode(node);
    return node;
}

Port *AggregatorNodeType::getPort(Node *node, const char *portname) const
{
    AggregatorNode *node1 = dynamic_cast<AggregatorNode *>(node);
    if (!strcmp(portname, "next-in"))
        return node1->addPort();
    else if (!strcmp(portname, "out"))
        return &(node1->out);
    throw opp_runtime_error("No such port '%s'", portname);
}

}  // namespace scave
}  // namespace omnetpp

