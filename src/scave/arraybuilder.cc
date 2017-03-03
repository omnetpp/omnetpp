//=========================================================================
//  ARRAYBUILDER.CC - part of
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

#include <memory.h>
#include <cstring>
#include "channel.h"
#include "arraybuilder.h"

namespace omnetpp {
namespace scave {

ArrayBuilderNode::ArrayBuilderNode()
{
    vecCapacity = 0;
    vecLength = 0;
    xvec = nullptr;
    yvec = nullptr;
    xpvec = nullptr;
    evec = nullptr;
    collectEvec = false;
}

ArrayBuilderNode::~ArrayBuilderNode()
{
    delete[] xvec;
    delete[] yvec;
    delete[] xpvec;
    delete[] evec;
}

void ArrayBuilderNode::resize()
{
    size_t newsize = 3*vecCapacity/2;

    if (!newsize)
        newsize = 1024;

    double *newxvec = new double[newsize];
    memcpy(newxvec, xvec, vecLength*sizeof(double));
    delete[] xvec;
    xvec = newxvec;

    double *newyvec = new double[newsize];
    memcpy(newyvec, yvec, vecLength*sizeof(double));
    delete[] yvec;
    yvec = newyvec;

    if (xpvec) {
        BigDecimal *newxpvec = new BigDecimal[newsize];
        memcpy(newxpvec, xpvec, vecCapacity*sizeof(BigDecimal));
        delete[] xpvec;
        xpvec = newxpvec;
    }

    if (collectEvec) {
        eventnumber_t *newevec = new eventnumber_t[newsize];
        memcpy(newevec, evec, vecCapacity*sizeof(eventnumber_t));
        delete[] evec;
        evec = newevec;
    }

    vecCapacity = newsize;
}

bool ArrayBuilderNode::isReady() const
{
    return in()->length() > 0;
}

void ArrayBuilderNode::process()
{
    int n = in()->length();
    for (int i = 0; i < n; i++) {
        Datum a;
        in()->read(&a, 1);
        if (vecLength == vecCapacity)
            resize();
        Assert(xvec && yvec);
        xvec[vecLength] = a.x;
        yvec[vecLength] = a.y;
        if (!a.xp.isNil()) {
            if (!xpvec)
                xpvec = new BigDecimal[vecCapacity];

            xpvec[vecLength] = a.xp;
        }
        if (collectEvec)
            evec[vecLength] = a.eventNumber;

        vecLength++;
    }
}

bool ArrayBuilderNode::isFinished() const
{
    return in()->eof();
}

void ArrayBuilderNode::sort()
{
    // FIXME sort both arrays by "x"
    // std::sort();
}

#ifndef min
#define min(a, b)    (((a) < (b)) ? (a) : (b))
#endif

XYArray *ArrayBuilderNode::getArray()
{
    // transfer ownership to XYArray
    XYArray *array = new XYArray(vecLength, xvec, yvec, xpvec, evec);
    xvec = yvec = nullptr;
    xpvec = nullptr;
    evec = nullptr;
    vecLength = 0;
    return array;
}

//------

const char *ArrayBuilderNodeType::getDescription() const
{
    return "Collects data into an array. The arrays can be accessed as two double* arrays in the C++ API.";
}

void ArrayBuilderNodeType::getAttrDefaults(StringMap& attrs) const
{
    attrs["collecteventnumbers"] = "false";
}

void ArrayBuilderNodeType::getAttributes(StringMap& attrs) const
{
    attrs["collecteventnumbers"] = "If true event numbers will be present in the output vector";
}

Node *ArrayBuilderNodeType::create(DataflowManager *mgr, StringMap& attrs) const
{
    checkAttrNames(attrs);

    ArrayBuilderNode *node = new ArrayBuilderNode();
    if (attrs.find("collecteventnumbers") != attrs.end() && !strcmp("true", attrs["collecteventnumbers"].c_str()))
        node->collectEvec = true;
    node->setNodeType(this);
    mgr->addNode(node);
    return node;
}

}  // namespace scave
}  // namespace omnetpp

