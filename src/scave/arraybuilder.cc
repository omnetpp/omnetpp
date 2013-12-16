//=========================================================================
//  ARRAYBUILDER.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <memory.h>
#include <string.h>
#include "channel.h"
#include "arraybuilder.h"

NAMESPACE_BEGIN


ArrayBuilderNode::ArrayBuilderNode()
{
    veccapacity = 0;
    veclen = 0;
    xvec = NULL;
    yvec = NULL;
    xpvec = NULL;
    evec = NULL;
    collectEvec = false;
}

ArrayBuilderNode::~ArrayBuilderNode()
{
    delete [] xvec;
    delete [] yvec;
    delete [] xpvec;
    delete [] evec;
}

void ArrayBuilderNode::resize()
{
    size_t newsize = 3*veccapacity/2;

    if (!newsize)
        newsize = 1024;

    double *newxvec = new double[newsize];
    memcpy(newxvec, xvec, veclen*sizeof(double));
    delete [] xvec;
    xvec = newxvec;

    double *newyvec = new double[newsize];
    memcpy(newyvec, yvec, veclen*sizeof(double));
    delete [] yvec;
    yvec = newyvec;

    if (xpvec) {
        BigDecimal *newxpvec = new BigDecimal[newsize];
        memcpy(newxpvec, xpvec, veccapacity*sizeof(BigDecimal));
        delete[] xpvec;
        xpvec = newxpvec;
    }

    if (collectEvec) {
        eventnumber_t *newevec = new eventnumber_t[newsize];
        memcpy(newevec, evec, veccapacity*sizeof(eventnumber_t));
        delete [] evec;
        evec = newevec;
    }

    veccapacity = newsize;
}

bool ArrayBuilderNode::isReady() const
{
    return in()->length()>0;
}

void ArrayBuilderNode::process()
{
    int n = in()->length();
    for (int i=0; i<n; i++)
    {
        Datum a;
        in()->read(&a,1);
        if (veclen==veccapacity)
            resize();
        Assert(xvec && yvec);
        xvec[veclen] = a.x;
        yvec[veclen] = a.y;
        if (!a.xp.isNil()) {
            if (!xpvec)
                xpvec = new BigDecimal[veccapacity];

            xpvec[veclen] = a.xp;
        }
        if (collectEvec)
            evec[veclen] = a.eventNumber;

        veclen++;
    }
}

bool ArrayBuilderNode::isFinished() const
{
    return in()->eof();
}


void ArrayBuilderNode::sort()
{
    // FIXME sort both arrays by "x"
    //std::sort();
}

#ifndef min
#define min(a,b)    (((a) < (b)) ? (a) : (b))
#endif

XYArray *ArrayBuilderNode::getArray()
{
    // transfer ownership to XYArray
    XYArray *array = new XYArray(veclen, xvec, yvec, xpvec, evec);
    xvec = yvec = NULL;
    xpvec = NULL;
    evec = NULL;
    veclen = 0;
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

NAMESPACE_END

