//=========================================================================
//  VECTORFILEWRITER.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_SCAVE_VECTORFILEWRITER_H
#define __OMNETPP_SCAVE_VECTORFILEWRITER_H

#include <vector>
#include "node.h"
#include "nodetype.h"
#include "resultfilemanager.h"

namespace omnetpp {
namespace scave {

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
        VectorFileWriterNode(const char *filename, const char *fileHeader=nullptr);
        virtual ~VectorFileWriterNode();

        Port *addVector(const VectorResult& vector);
        void setPrecision(int prec) {this->prec = prec;}

        virtual bool isReady() const override;
        virtual void process() override;
        virtual bool isFinished() const override;
};

class SCAVE_API VectorFileWriterNodeType : public NodeType
{
    public:
        virtual const char *getName() const override {return "vectorfilewriter";}
        virtual const char *getCategory() const override {return "multi-port sink";}
        virtual const char *getDescription() const override;
        virtual bool isHidden() const override {return true;}
        virtual void getAttributes(StringMap& attrs) const override;
        virtual Node *create(DataflowManager *mgr, StringMap& attrs) const override;
        virtual Port *getPort(Node *node, const char *portname) const override;
};

} // namespace scave
}  // namespace omnetpp


#endif


