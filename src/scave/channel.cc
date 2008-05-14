//=========================================================================
//  CHANNEL.CC - part of
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

#include "channel.h"

USING_NAMESPACE


Channel::Channel()
{
    consumernode = producernode = NULL;
    consumerfinished = producerfinished = false;
}

const Datum *Channel::peek() const
{
    if (buffer.size()==0)
        return NULL;
    return &(buffer.front());
}


int Channel::read(Datum *a, int max)
{
    Assert(!consumerfinished);
    int n = buffer.size();
    if (n>max)
        n = max;
    for (int i=0; i<n; i++)
    {
        a[i] = buffer.front();
        buffer.pop_front();
    }
    return n;
}

void Channel::write(Datum *a, int n)
{
    Assert(!producerfinished);
    if (consumerfinished)
        return;  // discard data if consumer finished
    for (int i=0; i<n; i++)
        buffer.push_back(a[i]);
}


