//=========================================================================
//  TEENODE.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef _TEENODE_H_
#define _TEENODE_H_

#include "commonnodes.h"

#include <vector>
#include "node.h"
#include "nodetype.h"

NAMESPACE_BEGIN


/**
 * Processing node splits an input stream into several output streams.
 */
class SCAVE_API TeeNode : public Node
{
    public:
        typedef std::vector<Port> PortVector;
        Port in;
        PortVector outs;
    public:
        TeeNode() : in(this) {}
        virtual ~TeeNode() {}

        Port *addPort();

        virtual bool isReady() const;
        virtual void process();
        virtual bool finished() const;
};

class SCAVE_API TeeNodeType : public NodeType
{
    public:
        virtual const char *name() const {return "tee";}
        virtual const char *category() const {return "tee";}
        virtual const char *description() const;
        virtual bool isHidden() const {return true;}
        virtual void getAttributes(StringMap& attrs) const;
        virtual Node *create(DataflowManager *mgr, StringMap& attrs) const;
        virtual Port *getPort(Node *node, const char *portname) const;
};

NAMESPACE_END


#endif



