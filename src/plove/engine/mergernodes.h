//=========================================================================
//  MERGERNODES.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef _MERGERNODES_H_
#define _MERGERNODES_H_

#include "commonnodes.h"

#include <vector>
#include "node.h"
#include "nodetype.h"


/**
 * Processing node which merges several input streams into one.
 */
class MergerNode : public Node
{
    public:
        typedef std::vector<Port> PortVector;
        Port out;
        PortVector ports;
    public:
        MergerNode() : out(this) {}
        virtual ~MergerNode() {}

        Port *addPort();

        virtual bool isReady() const;
        virtual void process();
        virtual bool finished() const;
};

class MergerNodeType : public NodeType
{
    public:
        virtual const char *name() const {return "merger";}
        virtual const char *category() const {return "merger";}
        virtual const char *description() const;
        virtual bool isHidden() const {return true;}
        virtual void getAttributes(StringMap& attrs) const;
        virtual Node *create(DataflowManager *, StringMap& attrs) const;
        virtual Port *getPort(Node *node, const char *portname) const;
};

#endif



