//=========================================================================
//  STDDEV.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Tamas Borbely
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <math.h>
#include "scaveutils.h"
#include "channel.h"
#include "stddev.h"

NAMESPACE_BEGIN


StddevNode::StddevNode()
    : num_vals(0), min_vals(NaN), max_vals(NaN), sum_vals(0.0), sqrsum_vals(0.0)
{
}

StddevNode::~StddevNode()
{
}

bool StddevNode::isReady() const
{
    return in()->length()>0;
}

void StddevNode::process()
{
    int n = in()->length();
    for (int i=0; i<n; i++)
    {
        Datum a;
        in()->read(&a,1);
        collect(a.y);
    }
}

bool StddevNode::isFinished() const
{
    return in()->eof();
}

void StddevNode::collect(double val)
{
    if (++num_vals <= 0)
        throw opp_runtime_error("StddevNode: observation count overflow");

    sum_vals += val;
    sqrsum_vals += val*val;

    if (num_vals>1)
    {
        if (val<min_vals)
            min_vals = val;
        else if (val>max_vals)
            max_vals = val;
    }
    else
    {
        min_vals = max_vals = val;
    }
}

double StddevNode::getVariance() const
{
    if (num_vals<=1)
        return 0.0;
    else
    {
        double devsqr = (sqrsum_vals - sum_vals*sum_vals/num_vals)/(num_vals-1);
        if (devsqr<=0)
            return 0.0;
        else
            return devsqr;
    }
}

double StddevNode::getStddev() const
{
    return sqrt( getVariance() );
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

NAMESPACE_END

