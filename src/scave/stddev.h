//=========================================================================
//  STDDEV.H - part of
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

#ifndef _STDDEV_H_
#define _STDDEV_H_

#include "commonnodes.h"
#include "xyarray.h"

NAMESPACE_BEGIN


/**
 * Calculates min, max, mean, stddev.
 */
class SCAVE_API StddevNode : public SingleSinkNode
{
    protected:
         long num_vals;
         double min_vals, max_vals;
         double sum_vals, sqrsum_vals;

    protected:
        virtual void collect(double val);

    public:
        StddevNode();
        virtual ~StddevNode();
        virtual bool isReady() const;
        virtual void process();
        virtual bool isFinished() const;

        virtual long getCount() const     {return num_vals;}
        virtual double getSum() const     {return sum_vals;}
        virtual double getSqrSum() const  {return sqrsum_vals;}
        virtual double getMin() const     {return min_vals;}
        virtual double getMax() const     {return max_vals;}
        virtual double getMean() const    {return num_vals ? sum_vals/num_vals : 0.0;}
        virtual double getStddev() const;
        virtual double getVariance() const;
};


class SCAVE_API StddevNodeType : public SingleSinkNodeType
{
    public:
        virtual const char *getName() const {return "stddev";}
        virtual const char *getDescription() const;
        virtual bool isHidden() const {return true;}
        virtual void getAttributes(StringMap& attrs) const;
        virtual Node *create(DataflowManager *mgr, StringMap& attrs) const;
};

NAMESPACE_END


#endif


