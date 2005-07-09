//=========================================================================
//  SLIDINGWINAVG.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include "channel.h"
#include "slidingwinavg.h"


SlidingWindowAverageNode::SlidingWindowAverageNode(int windowSize)
{
    array = NULL;
    if (windowSize<1 || windowSize>100000)
        throw new Exception("slidingwinavg: invalid window size %d", windowSize);
    winsize = windowSize;
    array = new Datum[winsize];
    count = pos = 0;
    sum = 0;
}

SlidingWindowAverageNode::~SlidingWindowAverageNode()
{
    delete array;
}

bool SlidingWindowAverageNode::isReady() const
{
    return in()->length()>0;
}

void SlidingWindowAverageNode::process()
{
    do {
        Datum& arrayElem = array[pos];
        if (count<winsize)
            count++; // just starting up, filling window [0..pos]
        else
            sum -= arrayElem.y;
        in()->read(&arrayElem,1);
        sum += arrayElem.y;
        Datum o;
        o.x = arrayElem.x;
        o.y = sum/count;
        out()->write(&o,1);
        pos = (pos+1) % winsize;
    }
    while (in()->length()>0);
}

//-----

const char *SlidingWindowAverageNodeType::description() const
{
    return "Replaces every value with the mean of values in the window:\n"
           "y[k] = SUM(y[i],i=k-winsize+1..k)/winsize";
}

void SlidingWindowAverageNodeType::getAttributes(StringMap& attrs) const
{
    attrs["windowSize"] = "window size";
}

void SlidingWindowAverageNodeType::getAttrDefaults(StringMap& attrs) const
{
    attrs["windowSize"] = "10";
}

Node *SlidingWindowAverageNodeType::create(DataflowManager *, StringMap& attrs) const
{
    checkAttrNames(attrs);

    const char *ws = attrs["windowSize"].c_str();
    int windowSize = atoi(ws);

    Node *node = new SlidingWindowAverageNode(windowSize);
    node->setNodeType(this);
    return node;
}

