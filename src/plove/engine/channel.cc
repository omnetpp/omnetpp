//=========================================================================
//  CHANNEL.CC - part of
//
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
    ASSERT(!consumerfinished);
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
    ASSERT(!producerfinished);
    if (consumerfinished)
        return;  // discard data if consumer finished
    for (int i=0; i<n; i++)
        buffer.push_back(a[i]);
}


