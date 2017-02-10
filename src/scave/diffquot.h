//=========================================================================
//  DIFFQUOT.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2005-2017 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_SCAVE_DIFFQUOT_H
#define __OMNETPP_SCAVE_DIFFQUOT_H

#include "commonnodes.h"

namespace omnetpp {
namespace scave {


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
        virtual bool isReady() const override;
        virtual void process() override;
};

class SCAVE_API DifferenceQuotientNodeType : public FilterNodeType
{
    public:
        virtual const char *getName() const override {return "diffquot";}
        virtual const char *getDescription() const override;
        virtual void getAttributes(StringMap& attrs) const override;
        virtual void getAttrDefaults(StringMap& attrs) const override;
        virtual Node *create(DataflowManager *mgr, StringMap& attrs) const override;
        virtual void mapVectorAttributes(/*inout*/StringMap& attrs, /*out*/StringVector& warnings) const override;
};

} // namespace scave
}  // namespace omnetpp


#endif


