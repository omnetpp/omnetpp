//=========================================================================
//  VECTORFILEWRITER.H - part of
//                          OMNeT++
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2003 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef _VECTORFILEWRITER_H_
#define _VECTORFILEWRITER_H_

#include <vector>
#include "node.h"
#include "nodetype.h"


/**
 * Consumer node which writes an output vector file.
 */
class VectorFileWriterNode : public Node
{
    public:
        struct Pair {
            int id;
            Port port;
        };
        typedef std::vector<Pair> PortVector;

    private:
        PortVector ports;
        std::string fname;
        FILE *f;

    public:
        VectorFileWriterNode(const char *filename);
        virtual ~VectorFileWriterNode();

        Port *addVector(int vectorId);

        virtual bool isReady() const;
        virtual void process();
        virtual bool finished() const;
};

class VectorFileWriterNodeType : public NodeType
{
    public:
        virtual const char *name() const {return "vectorfilewriter";}
        virtual const char *category() const {return "multi-port sink";}
        virtual const char *description() const;
        virtual bool isHidden() const {return true;}
        virtual void getAttributes(StringMap& attrs) const;
        virtual Node *create(DataflowManager *, StringMap& attrs) const;
        virtual Port *getPort(Node *node, const char *portname) const;
};

#endif


