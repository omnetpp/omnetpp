//=========================================================================
//  ARRAYBUILDER.H - part of
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

#ifndef __OMNETPP_SCAVE_ARRAYBUILDER_H
#define __OMNETPP_SCAVE_ARRAYBUILDER_H

#include "common/commonutil.h"
#include "commonnodes.h"
#include "xyarray.h"

namespace omnetpp {
namespace scave {

class ArrayBuilderNodeType;

/**
 * Stores all data in vector (two 'double' vectors in fact).
 */
class SCAVE_API ArrayBuilderNode : public SingleSinkNode
{
    friend class ArrayBuilderNodeType;
    private:
        double *xvec;
        double *yvec;
        size_t vecCapacity;
        size_t vecLength;
        BigDecimal *xpvec;
        bool collectEvec;
        eventnumber_t *evec; // event numbers
        void resize();
    public:
        ArrayBuilderNode();
        virtual ~ArrayBuilderNode();
        virtual bool isReady() const override;
        virtual void process() override;
        virtual bool isFinished() const override;

        void sort();
        size_t length() {return vecLength;}
        XYArray *getArray();
};


class SCAVE_API ArrayBuilderNodeType : public SingleSinkNodeType
{
    friend class ArrayBuilderNode;
    public:
        virtual const char *getName() const override {return "arraybuilder";}
        virtual const char *getDescription() const override;
        virtual bool isHidden() const override {return true;}
        virtual void getAttributes(StringMap& attrs) const override;
        virtual void getAttrDefaults(StringMap& attrs) const override;
        virtual Node *create(DataflowManager *mgr, StringMap& attrs) const override;
};

} // namespace scave
}  // namespace omnetpp


#endif


