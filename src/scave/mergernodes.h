//=========================================================================
//  MERGERNODES.H - part of
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

#ifndef _MERGERNODES_H_
#define _MERGERNODES_H_

#include "commonnodes.h"

#include <vector>
#include "node.h"
#include "nodetype.h"

NAMESPACE_BEGIN


/**
 * Processing node which merges several input streams into one.
 */
class SCAVE_API MergerNode : public Node
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
        virtual bool isFinished() const;
};

class SCAVE_API MergerNodeType : public NodeType
{
    public:
        virtual const char *getName() const {return "merger";}
        virtual const char *getCategory() const {return "merger";}
        virtual const char *getDescription() const;
        virtual bool isHidden() const {return true;}
        virtual void getAttributes(StringMap& attrs) const;
        virtual Node *create(DataflowManager *mgr, StringMap& attrs) const;
        virtual Port *getPort(Node *node, const char *portname) const;
};

NAMESPACE_END


#endif



