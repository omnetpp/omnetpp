//=========================================================================
//  WINDOWAVG.CC - part of
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
#include "windowavg.h"


WindowAverageNode::WindowAverageNode(int windowSize)
{
    array = NULL;
    if (windowSize<1 || windowSize>100000)
        throw new Exception("winavg: invalid window size %d", windowSize);
    winsize = windowSize;
    array = new Datum[winsize];
}

WindowAverageNode::~WindowAverageNode()
{
    delete array;
}

bool WindowAverageNode::isReady() const
{
    return in()->length()>=winsize || in()->closing();
}

void WindowAverageNode::process()
{
    // do as many winsize-sized chunks as possible, but in the end
    // we'll have to do the remaining ones (n<winsize)
    do {
        int n = in()->read(array,winsize);
        double sumy = 0;
        for (int i=0; i<n; i++)
            sumy += array[i].y;
        Datum o;
        o.x = array[0].x;
        o.y = sumy/n;
        out()->write(&o,1);
    }
    while (in()->length()>=winsize || (in()->closing() && in()->length()>0));
}

//-----

const char *WindowAverageNodeType::description() const
{
    return "Calculates batched average: replaces every `winsize' input values\n"
           "with their mean. Time is the time of the first value in the batch.";
}

void WindowAverageNodeType::getAttributes(StringMap& attrs) const
{
    attrs["windowSize"] = "size of batch as integer";
}

void WindowAverageNodeType::getAttrDefaults(StringMap& attrs) const
{
    attrs["windowSize"] = "10";
}

Node *WindowAverageNodeType::create(DataflowManager *, StringMap& attrs) const
{
    checkAttrNames(attrs);

    const char *ws = attrs["windowSize"].c_str();
    int windowSize = atoi(ws);

    Node *node = new WindowAverageNode(windowSize);
    node->setNodeType(this);
    return node;
}

