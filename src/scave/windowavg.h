//=========================================================================
//  WINDOWAVG.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef _WINDOWAVG_H_
#define _WINDOWAVG_H_

#include "commonnodes.h"


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
        virtual const char *name() const {return "winavg";}
        virtual const char *description() const;
        virtual void getAttributes(StringMap& attrs) const;
        virtual void getAttrDefaults(StringMap& attrs) const;
        virtual Node *create(DataflowManager *mgr, StringMap& attrs) const;
        virtual void mapVectorAttributes(/*inout*/StringMap &attrs) const;
};

#endif


