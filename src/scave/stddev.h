//=========================================================================
//  STDDEV.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

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
        virtual bool finished() const;

        virtual long count() const     {return num_vals;}
        virtual double sum() const     {return sum_vals;}
        virtual double sqrSum() const  {return sqrsum_vals;}
        virtual double min() const     {return min_vals;}
        virtual double max() const     {return max_vals;}
        virtual double mean() const    {return num_vals ? sum_vals/num_vals : 0.0;}
        virtual double stddev() const;
        virtual double variance() const;
};


class SCAVE_API StddevNodeType : public SingleSinkNodeType
{
    public:
        virtual const char *name() const {return "stddev";}
        virtual const char *description() const;
        virtual bool isHidden() const {return true;}
        virtual void getAttributes(StringMap& attrs) const;
        virtual Node *create(DataflowManager *mgr, StringMap& attrs) const;
};

NAMESPACE_END


#endif


