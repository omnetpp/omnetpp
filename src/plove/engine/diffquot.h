//=========================================================================
//  DIFFQUOT.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef _DIFFQUOT_H_
#define _DIFFQUOT_H_

#include "commonnodes.h"


/**
 * Processing node which calculates difference quotient of every value 
 * and the next one.
 */
class DifferenceQuotientNode : public FilterNode
{
    private:
        bool firstRead;
        Datum l;
    public:
        DifferenceQuotientNode();
        virtual bool isReady() const;
        virtual void process();
};

class DifferenceQuotientNodeType : public FilterNodeType
{
    public:
        virtual const char *name() const {return "diffquot";}
        virtual const char *description() const;
        virtual void getAttributes(StringMap& attrs) const;
        virtual void getAttrDefaults(StringMap& attrs) const;
        virtual Node *create(DataflowManager *, StringMap& attrs) const;
};

#endif


