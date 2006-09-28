//=========================================================================
//  STDDEV.CC - part of
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
#include "stddev.h"


StddevNode::StddevNode()
{
}

StddevNode::~StddevNode()
{
}

bool StddevNode::isReady() const
{
    return in()->length()>0;
}

void StddevNode::process()
{
    int n = in()->length();
    for (int i=0; i<n; i++)
    {
        Datum a;
        in()->read(&a,1);
        //TODO collect a.x, a.y
    }
}

bool StddevNode::finished() const
{
    return in()->eof();
}

//------

const char *StddevNodeType::description() const
{
    return "Collects basic statistics like min, max, mean, stddev.";
}

void StddevNodeType::getAttributes(StringMap& attrs) const
{
}

Node *StddevNodeType::create(DataflowManager *mgr, StringMap& attrs) const
{
    checkAttrNames(attrs);

    Node *node = new StddevNode();
    node->setNodeType(this);
    mgr->addNode(node);
    return node;
}


