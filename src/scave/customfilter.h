//=========================================================================
//  CUSTOMFILTER.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef _CUSTOMFILTER_H_
#define _CUSTOMFILTER_H_

#include "commonnodes.h"
#include "expression.h"


/**
 * Processing node which evaluates an arbitrary expression entered
 * by the user.
 */
class SCAVE_API CustomFilterNode : public FilterNode
{
    public:
        CustomFilterNode()  {}
        virtual ~CustomFilterNode() {}
        virtual bool isReady() const;
        virtual void process();
};

class SCAVE_API CustomFilterNodeType : public FilterNodeType
{
    public:
        virtual const char *name() const {return "custom";}
        virtual const char *description() const;
        virtual void getAttributes(StringMap& attrs) const;
        virtual Node *create(DataflowManager *mgr, StringMap& attrs) const;
};

#endif


