//=========================================================================
//  VECTORFILEWRITER.H - part of
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

#ifndef _VECTORFILEWRITER_H_
#define _VECTORFILEWRITER_H_

#include <vector>
#include "node.h"
#include "nodetype.h"
#include "resultfilemanager.h"

NAMESPACE_BEGIN

/**
 * Consumer node which writes an output vector file.
 */
class SCAVE_API VectorFileWriterNode : public Node
{
    public:
        struct Pair {
            int id;
            std::string moduleName;
            std::string name;
            std::string columns;
            Port port;

            Pair(int id, const char *moduleName, const char *name, const char *columns, Node *owner) : port(owner)
                {this->id = id; this->moduleName = moduleName; this->name = name; this->columns = columns; }
        };
        typedef std::vector<Pair> PortVector;

    private:
        PortVector ports;
        std::string fileName;
        std::string fileHeader;
        FILE *f;
        int prec;

    public:
        VectorFileWriterNode(const char *filename, const char *fileHeader=NULL);
        virtual ~VectorFileWriterNode();

        Port *addVector(const VectorResult &vector);
        void setPrecision(int prec) {this->prec = prec;}

        virtual bool isReady() const;
        virtual void process();
        virtual bool isFinished() const;
};

class SCAVE_API VectorFileWriterNodeType : public NodeType
{
    public:
        virtual const char *getName() const {return "vectorfilewriter";}
        virtual const char *getCategory() const {return "multi-port sink";}
        virtual const char *getDescription() const;
        virtual bool isHidden() const {return true;}
        virtual void getAttributes(StringMap& attrs) const;
        virtual Node *create(DataflowManager *mgr, StringMap& attrs) const;
        virtual Port *getPort(Node *node, const char *portname) const;
};

NAMESPACE_END


#endif


