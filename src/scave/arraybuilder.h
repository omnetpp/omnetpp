//=========================================================================
//  ARRAYBUILDER.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef _ARRAYBUILDER_H_
#define _ARRAYBUILDER_H_

#include "commonutil.h"
#include "commonnodes.h"
#include "xyarray.h"

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
        size_t veccapacity;
        size_t veclen;
        BigDecimal *xpvec;
        bool collectEvec;
        long *evec; // event numbers
        void resize();
    public:
        ArrayBuilderNode();
        virtual ~ArrayBuilderNode();
        virtual bool isReady() const;
        virtual void process();
        virtual bool finished() const;

        void sort();
        size_t length() {return veclen;}
        XYArray *getArray();
};


class SCAVE_API ArrayBuilderNodeType : public SingleSinkNodeType
{
    friend class ArrayBuilderNode;
    public:
        virtual const char *name() const {return "arraybuilder";}
        virtual const char *description() const;
        virtual bool isHidden() const {return true;}
        virtual void getAttributes(StringMap& attrs) const;
        virtual void getAttrDefaults(StringMap& attrs) const;
        virtual Node *create(DataflowManager *mgr, StringMap& attrs) const;
};

#endif


