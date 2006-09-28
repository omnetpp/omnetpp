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


/**
 * Calculates min, max, mean, stddev.
 */
class StddevNode : public SingleSinkNode
{
    private:
    public:
        StddevNode();
        virtual ~StddevNode();
        virtual bool isReady() const;
        virtual void process();
        virtual bool finished() const;
};


class StddevNodeType : public SingleSinkNodeType
{
    public:
        virtual const char *name() const {return "stddev";}
        virtual const char *description() const;
        virtual bool isHidden() const {return true;}
        virtual void getAttributes(StringMap& attrs) const;
        virtual Node *create(DataflowManager *mgr, StringMap& attrs) const;
};

#endif


