//=========================================================================
//  WINDOWAVG.H - part of
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

#ifndef __OMNETPP_SCAVE_WINDOWAVG_H
#define __OMNETPP_SCAVE_WINDOWAVG_H

#include "commonnodes.h"

namespace omnetpp {
namespace scave {


/**
 * Processing node which performs window average. Replaces every batch
 * of values with their means.
 */
class SCAVE_API WindowAverageNode : public FilterNode
{
    protected:
        int winsize;
        Datum *array;

    public:
        WindowAverageNode(int windowSize);
        virtual ~WindowAverageNode();
        virtual bool isReady() const override;
        virtual void process() override;
};

class SCAVE_API WindowAverageNodeType : public FilterNodeType
{
    public:
        virtual const char *getName() const override {return "winavg";}
        virtual const char *getDescription() const override;
        virtual void getAttributes(StringMap& attrs) const override;
        virtual void getAttrDefaults(StringMap& attrs) const override;
        virtual Node *create(DataflowManager *mgr, StringMap& attrs) const override;
        virtual void mapVectorAttributes(/*inout*/StringMap& attrs, /*out*/StringVector& warnings) const override;
};

/**
 *
 */
class SCAVE_API TimeWindowAverageNode : public FilterNode
{
    protected:
        simultime_t winsize;
        simultime_t win_end;
        double sum;
        long count;
        void collect(const Datum& d) { sum += d.y; count++; }
        void outputWindowAverage();
        bool inCurrentWindow(const Datum& d) const { return (!d.xp.isNil() && d.xp < win_end) || d.x < win_end.dbl(); }
        void moveWindow(const Datum& d);

    public:
        TimeWindowAverageNode(simultime_t windowSize);
        virtual ~TimeWindowAverageNode();
        virtual bool isFinished() const override;
        virtual bool isReady() const override;
        virtual void process() override;
};

inline void TimeWindowAverageNode::outputWindowAverage()
{
    if (count > 0)
    {
        Datum d;
        d.x = win_end.dbl();
        d.xp = win_end;
        d.y = sum / count;
        out()->write(&d, 1);
        sum = 0.0;
        count = 0;
    }
}

inline void TimeWindowAverageNode::moveWindow(const Datum& d)
{
    BigDecimal x = d.xp.isNil() ? BigDecimal(d.x) : d.xp;
    if (x < win_end + winsize)
        win_end += winsize;
    else
    {
        BigDecimal k = floor(x / winsize);
        win_end = (k.dbl() * winsize) + winsize;
    }
}

class SCAVE_API TimeWindowAverageNodeType : public FilterNodeType
{
    public:
        virtual const char *getName() const override {return "timewinavg";}
        virtual const char *getDescription() const override;
        virtual void getAttributes(StringMap& attrs) const override;
        virtual void getAttrDefaults(StringMap& attrs) const override;
        virtual Node *create(DataflowManager *mgr, StringMap& attrs) const override;
        virtual void mapVectorAttributes(/*inout*/StringMap& attrs, /*out*/StringVector& warnings) const override;
};


} // namespace scave
}  // namespace omnetpp


#endif


