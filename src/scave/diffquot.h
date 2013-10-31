//=========================================================================
//  DIFFQUOT.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2005-2008 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef _DIFFQUOT_H_
#define _DIFFQUOT_H_

#include "commonnodes.h"

NAMESPACE_BEGIN


/**
 * Processing node which calculates difference quotient of every value
 * and the next one.
 */
class SCAVE_API DifferenceQuotientNode : public FilterNode
{
    private:
        bool firstRead;
        Datum l;
    public:
        DifferenceQuotientNode();
        virtual bool isReady() const;
        virtual void process();
};

class SCAVE_API DifferenceQuotientNodeType : public FilterNodeType
{
    public:
        virtual const char *getName() const {return "diffquot";}
        virtual const char *getDescription() const;
        virtual void getAttributes(StringMap& attrs) const;
        virtual void getAttrDefaults(StringMap& attrs) const;
        virtual Node *create(DataflowManager *mgr, StringMap& attrs) const;
        virtual void mapVectorAttributes(/*inout*/StringMap &attrs, /*out*/StringVector &warnings) const;
};

NAMESPACE_END


#endif


