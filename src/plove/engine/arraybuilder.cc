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
}

ArrayBuilderNode::~ArrayBuilderNode()
{
    delete [] xvec;
    delete [] yvec;
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

bool ArrayBuilderNode::isReady() const
{
    return in()->length()>0;
}

void ArrayBuilderNode::process()
{
    ASSERT(xvec && yvec);
    int n = in()->length();
    for (int i=0; i<n; i++)
    {
        Datum a;
        in()->read(&a,1);
        if (veclen==vecsize)
            resize();
        xvec[veclen] = a.x;
        yvec[veclen] = a.y;
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

void ArrayBuilderNode::extractVector(double *&x, double *&y, size_t& len)
{
    // transfer ownership to caller
    x = xvec;
    y = yvec;
    len = veclen;
    xvec = NULL;
    yvec = NULL;
    veclen = 0;
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

Node *ArrayBuilderNodeType::create(DataflowManager *, StringMap& attrs) const
{
    checkAttrNames(attrs);

    Node *node = new ArrayBuilderNode();
    node->setNodeType(this);
    return node;
}


