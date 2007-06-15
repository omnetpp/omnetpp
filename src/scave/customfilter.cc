//=========================================================================
//  FILTERNODES.CC - part of
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

#include <math.h>
#include "channel.h"
#include "customfilter.h"


bool CustomFilterNode::isReady() const
{
    return in()->length()>0;
}

void CustomFilterNode::process()
{
    int n = in()->length();
    for (int i=0; i<n; i++)
    {
        Datum d;
        in()->read(&d,1);
        out()->write(&d,1);
    }
}

//--

const char *CustomFilterNodeType::description() const
{
    return "Evaluates an arbitrary expression";
}

void CustomFilterNodeType::getAttributes(StringMap& attrs) const
{
    attrs["expression"] = "The expression to evaluate. Use x for time, and y for value."; //FIXME use "t" and "x" instead?
}

Node *CustomFilterNodeType::create(DataflowManager *mgr, StringMap& attrs) const
{
    checkAttrNames(attrs);

    Node *node = new CustomFilterNode();
    node->setNodeType(this);
    mgr->addNode(node);
    return node;
}


