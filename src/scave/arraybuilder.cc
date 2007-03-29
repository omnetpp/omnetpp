//=========================================================================
//  ARRAYBUILDER.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include "channel.h"
#include "arraybuilder.h"


ArrayBuilderNode::ArrayBuilderNode()
{
    vecsize = 1024;
    veclen = 0;
    xvec = new double[vecsize];
    yvec = new double[vecsize];
    xpvecsize = 0;
    xpvec = NULL;
}

ArrayBuilderNode::~ArrayBuilderNode()
{
    delete [] xvec;
    delete [] yvec;
    if (xpvec) delete[] xpvec;
}

void ArrayBuilderNode::resize()
{
    size_t newsize = 3*vecsize/2;
    double *newxvec = new double[newsize];
    double *newyvec = new double[newsize];
    memcpy(newxvec, xvec, veclen*sizeof(double));
    memcpy(newyvec, yvec, veclen*sizeof(double));
    delete [] xvec;
    delete [] yvec;
    xvec = newxvec;
    yvec = newyvec;
    vecsize = newsize;
}

void ArrayBuilderNode::resizePrec()
{
    size_t newsize = vecsize;
    BigDecimal *newxpvec = new BigDecimal[newsize];
    memcpy(newxpvec, xpvec, xpvecsize*sizeof(BigDecimal));
    delete[] xpvec;
    xpvec = newxpvec;
    xpvecsize = newsize;
}

bool ArrayBuilderNode::isReady() const
{
    return in()->length()>0;
}

void ArrayBuilderNode::process()
{
    Assert(xvec && yvec);
    int n = in()->length();
    for (int i=0; i<n; i++)
    {
        Datum a;
        in()->read(&a,1);
        if (veclen==vecsize)
            resize();
        xvec[veclen] = a.x;
        yvec[veclen] = a.y;
        if (!a.xp.isNil())
        {
            if (veclen>=xpvecsize)
                resizePrec();
            xpvec[veclen] = a.xp;
        }
        veclen++;
    }
}

bool ArrayBuilderNode::finished() const
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

void ArrayBuilderNode::extractVector(double *&x, double *&y, size_t &len, BigDecimal *&xp, size_t &xplen)
{
    // transfer ownership to caller
    x = xvec;
    y = yvec;
    len = veclen;
    xp = xpvec;
    xplen = min(veclen, xpvecsize);
    xvec = NULL;
    yvec = NULL;
    veclen = 0;
    xpvec = NULL;
}

XYArray *ArrayBuilderNode::getArray()
{
    // transfer ownership to XYArray
    XYArray *array = new XYArray(xvec, yvec, veclen, xpvec, min(veclen, xpvecsize));
    xvec = yvec = NULL;
    xpvec = NULL;
    veclen = 0;
    return array;
}

//------

const char *ArrayBuilderNodeType::description() const
{
    return "Collects data into an array. The arrays can be accessed as BLT vectors or\n"
           "as two double* arrays in the C++ API.";
}

void ArrayBuilderNodeType::getAttributes(StringMap& attrs) const
{
}

Node *ArrayBuilderNodeType::create(DataflowManager *mgr, StringMap& attrs) const
{
    checkAttrNames(attrs);

    Node *node = new ArrayBuilderNode();
    node->setNodeType(this);
    mgr->addNode(node);
    return node;
}


