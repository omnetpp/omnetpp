//=========================================================================
//  SLIDINGWINAVG.H - part of
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

#ifndef __OMNETPP_SCAVE_SLIDINGWINAVG_H
#define __OMNETPP_SCAVE_SLIDINGWINAVG_H

#include "commonnodes.h"

namespace omnetpp {
namespace scave {


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
        virtual bool isReady() const override;
        virtual void process() override;
};

class SCAVE_API SlidingWindowAverageNodeType : public FilterNodeType
{
    public:
        virtual const char *getName() const override {return "slidingwinavg";}
        virtual const char *getDescription() const override;
        virtual void getAttributes(StringMap& attrs) const override;
        virtual void getAttrDefaults(StringMap& attrs) const override;
        virtual Node *create(DataflowManager *mgr, StringMap& attrs) const override;
        virtual void mapVectorAttributes(/*inout*/StringMap& attrs, /*out*/StringVector& warnings) const override;
};

} // namespace scave
}  // namespace omnetpp


#endif


