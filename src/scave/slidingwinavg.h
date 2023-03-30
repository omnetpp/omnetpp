//=========================================================================
//  SLIDINGWINAVG.H - part of
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

#ifndef _SLIDINGWINAVG_H_
#define _SLIDINGWINAVG_H_

#include "commonnodes.h"

NAMESPACE_BEGIN


/**
 * Processing node which performs sliding window average. Replaces every
 * value with the mean of values in the window.
 */
class SCAVE_API SlidingWindowAverageNode : public FilterNode
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

class SCAVE_API SlidingWindowAverageNodeType : public FilterNodeType
{
    public:
        virtual const char *getName() const {return "slidingwinavg";}
        virtual const char *getDescription() const;
        virtual void getAttributes(StringMap& attrs) const;
        virtual void getAttrDefaults(StringMap& attrs) const;
        virtual Node *create(DataflowManager *mgr, StringMap& attrs) const;
        virtual void mapVectorAttributes(/*inout*/StringMap &attrs, /*out*/StringVector &warnings) const;
};

NAMESPACE_END


#endif


