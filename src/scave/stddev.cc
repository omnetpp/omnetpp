//=========================================================================
//  STDDEV.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Tamas Borbely
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cmath>
#include "scaveutils.h"
#include "channel.h"
#include "stddev.h"

namespace omnetpp {
namespace scave {


StddevNode::StddevNode()
    : numValues(0), minValue(NaN), maxValue(NaN), sumValues(0.0), sqrsumValues(0.0)
{
}

StddevNode::~StddevNode()
{
}

bool StddevNode::isReady() const
{
    return in()->length() > 0;
}

void StddevNode::process()
{
    int n = in()->length();
    for (int i = 0; i < n; i++) {
        Datum a;
        in()->read(&a, 1);
        collect(a.y);
    }
}

bool StddevNode::isFinished() const
{
    return in()->eof();
}

void StddevNode::collect(double val)
{
    if (++numValues <= 0)
        throw opp_runtime_error("StddevNode: Observation count overflow");

    sumValues += val;
    sqrsumValues += val*val;

    if (numValues > 1) {
        if (val < minValue)
            minValue = val;
        else if (val > maxValue)
            maxValue = val;
    }
    else {
        minValue = maxValue = val;
    }
}

double StddevNode::getVariance() const
{
    if (numValues <= 1)
        return 0.0;
    else {
        double devsqr = (sqrsumValues - sumValues*sumValues/numValues)/(numValues-1);
        if (devsqr <= 0)
            return 0.0;
        else
            return devsqr;
    }
}

double StddevNode::getStddev() const
{
    return sqrt(getVariance());
}

//------

const char *StddevNodeType::getDescription() const
{
    return "Collects basic statistics like min, max, mean, stddev.";
}

void StddevNodeType::getAttributes(StringMap& attrs) const
{
}

Node *StddevNodeType::create(DataflowManager *mgr, StringMap& attrs) const
{
    checkAttrNames(attrs);

    Node *node = new StddevNode();
    node->setNodeType(this);
    mgr->addNode(node);
    return node;
}

}  // namespace scave
}  // namespace omnetpp

