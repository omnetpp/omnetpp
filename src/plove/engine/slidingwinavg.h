//=========================================================================
//  SLIDINGWINAVG.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef _SLIDINGWINAVG_H_
#define _SLIDINGWINAVG_H_

#include "commonnodes.h"


/**
 * Processing node which performs sliding window average. Replaces every
 * value with the mean of values in the window.
 */
class SlidingWindowAverageNode : public FilterNode
{
    protected:
        int winsize;
        Datum *array;
        double sum;
        int count, pos;

    public:
        SlidingWindowAverageNode(int windowSize);
        virtual ~SlidingWindowAverageNode();
        virtual bool isReady() const;
        virtual void process();
};

class SlidingWindowAverageNodeType : public FilterNodeType
{
    public:
        virtual const char *name() const {return "slidingwinavg";}
        virtual const char *description() const;
        virtual void getAttributes(StringMap& attrs) const;
        virtual void getAttrDefaults(StringMap& attrs) const;
        virtual Node *create(DataflowManager *, StringMap& attrs) const;
};

#endif


