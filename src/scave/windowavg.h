//=========================================================================
//  WINDOWAVG.H - part of
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

#ifndef _WINDOWAVG_H_
#define _WINDOWAVG_H_

#include "commonnodes.h"

NAMESPACE_BEGIN


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
        virtual bool isReady() const;
        virtual void process();
};

class SCAVE_API WindowAverageNodeType : public FilterNodeType
{
    public:
        virtual const char *getName() const {return "winavg";}
        virtual const char *getDescription() const;
        virtual void getAttributes(StringMap& attrs) const;
        virtual void getAttrDefaults(StringMap& attrs) const;
        virtual Node *create(DataflowManager *mgr, StringMap& attrs) const;
        virtual void mapVectorAttributes(/*inout*/StringMap &attrs, /*out*/StringVector &warnings) const;
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
        void collect(const Datum &d) { sum += d.y; count++; }
        void outputWindowAverage();
        bool inCurrentWindow(const Datum &d) const { return (!d.xp.isNil() && d.xp < win_end) || d.x < win_end.dbl(); }
        void moveWindow(const Datum &d);

    public:
        TimeWindowAverageNode(simultime_t windowSize);
        virtual ~TimeWindowAverageNode();
        virtual bool isFinished() const;
        virtual bool isReady() const;
        virtual void process();
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

inline void TimeWindowAverageNode::moveWindow(const Datum &d)
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
        virtual const char *getName() const {return "timewinavg";}
        virtual const char *getDescription() const;
        virtual void getAttributes(StringMap& attrs) const;
        virtual void getAttrDefaults(StringMap& attrs) const;
        virtual Node *create(DataflowManager *mgr, StringMap& attrs) const;
        virtual void mapVectorAttributes(/*inout*/StringMap &attrs, /*out*/StringVector &warnings) const;
};


NAMESPACE_END


#endif


