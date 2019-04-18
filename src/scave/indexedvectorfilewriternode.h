//=========================================================================
//  INDEXEDVECTORFILEWRITERNODE.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Tamas Borbely
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_SCAVE_INDEXEDVECTORFILEWRITERNODE_H
#define __OMNETPP_SCAVE_INDEXEDVECTORFILEWRITERNODE_H


#include <cassert>
#include <cfloat>
#include <vector>
#include <map>
#include <cstdarg>
#include "common/filereader.h"
#include "scavedefs.h"
#include "indexfilewriter.h"
#include "node.h"
#include "nodetype.h"
#include "resultfilemanager.h"
#include "vectorfileindex.h"

namespace omnetpp {
namespace scave {


/**
 * Consumer node which writes an indexed output vector file.
 */
class SCAVE_API IndexedVectorFileWriterNode : public Node
{
    using VectorInfo = VectorFileIndex::VectorInfo;
    using Block = VectorFileIndex::Block;
    using RunData = VectorFileIndex::RunData;

    protected:
        class VectorInputPort : public Port {
            public:
            VectorInfo vector;
            char *buffer;     // buffer holding recorded data
            int bufferSize;  // size of the allocated buffer
            char *bufferPtr; // pointer to the current position in the buffer
            int bufferNumOfRecords; //
            bool finished;

            VectorInputPort(int id, std::string moduleName, std::string name, std::string columns, int bufferSize, Node *owner)
                : Port(owner), vector(id, moduleName, name, columns, bufferSize), finished(false)
                { this->allocateBuffer(bufferSize); vector.blocks.push_back(new Block()); } // TODO what is this? a sentinel object?
            ~VectorInputPort() { if (buffer) delete[] buffer; }

            void allocateBuffer(int size) { buffer=new char[size]; bufferSize=size; clearBuffer(); }
            void clearBuffer() { bufferPtr = buffer; bufferNumOfRecords=0; if (buffer) buffer[0] = '\0'; }
            bool hasBufferedData() const { return bufferPtr!=buffer; }
        };

        typedef std::vector<VectorInputPort*> PortVector;
        typedef std::map<std::string,std::string> StringMap;

    private:
        PortVector ports;
        std::string fileHeader;
        std::string fileName;
        std::string indexFileName;
        int blockSize;
        RunData run;
        FILE *f;
        IndexFileWriter *indexWriter;
        int prec;

    public:
        IndexedVectorFileWriterNode(const char *fileName, const char* indexFileName, int blockSize, const char *fileHeader=nullptr);
        virtual ~IndexedVectorFileWriterNode();

        Port *addVector(int vectorId, const char *module, const char *name, const char *columns);
        Port *addVector(const VectorResult& vector);
        void setPrecision(int prec) {this->prec = prec;}
        void setHeader(const std::string& header) { fileHeader = header; }
        void setRun(const char *runName, const StringMap& attributes, const OrderedKeyValueList& paramAssignments)
            { run.runName = runName; run.attributes = attributes; run.paramAssignments = paramAssignments; };
        std::string getFilename() const {return fileName;}

        virtual bool isReady() const override;
        virtual void process() override;
        virtual bool isFinished() const override;
    protected:
        void writeRecordsToBuffer(VectorInputPort *port);
        void writeBufferToFile(VectorInputPort *port);
        void writeIndex(VectorInputPort *port);
    private:
#ifndef SWIG
        // note: swig 1.3.33 cannot parse this (even though this class is ignored in scave.i)
        void bufferPrintf(VectorInputPort *port, const char *format...);
#endif
};

class SCAVE_API IndexedVectorFileWriterNodeType : public NodeType
{
    public:
        virtual const char *getName() const override {return "indexedvectorfilewriter";}
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
