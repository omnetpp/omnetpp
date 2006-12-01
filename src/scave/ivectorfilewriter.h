//=========================================================================
//  VECTORFILEWRITER.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef _IVECTORFILEWRITER_H_
#define _IVECTORFILEWRITER_H_

#include <float.h>
#include <vector>
#include "node.h"
#include "nodetype.h"


/**
 * Consumer node which writes an indexed output vector file.
 */
class IndexedVectorFileWriterNode : public Node
{
    protected:
        struct Block {
          long offset;
          long numOfRecords;
          Block(long offset, long numOfRecords) : offset(offset), numOfRecords(numOfRecords) {}
        };

        class VectorInputPort : public Port {
            public:
            int id;
            std::string moduleName;
            std::string name;
            char *buffer;     // buffer holding recorded data
            int bufferSize;  // size of the allocated buffer
            char *bufferPtr; // pointer to the current position in the buffer
            int bufferNumOfRecords; //
            int numOfRecords; // number of records written into the buffer
            double min;
            double max;
            double sum;
            double sumSqr;
            std::vector<Block> blocks; // attributes of the blocks written into the file

            VectorInputPort(int id, std::string moduleName, std::string name, int bufferSize, Node *owner)
                : id(id), moduleName(moduleName), name(name), numOfRecords(0), min(DBL_MAX), max(DBL_MIN),
                  sum(0.0), sumSqr(0.0), Port(owner)
                { this->allocateBuffer(bufferSize); }
            ~VectorInputPort() { if (buffer) delete[] buffer; }

            void allocateBuffer(int size) { buffer=new char[size]; bufferSize=size; clearBuffer(); }
            void clearBuffer() { bufferPtr = buffer; bufferNumOfRecords=0; if (buffer) buffer[0] = '\0'; }
            bool hasBufferedData() const { return bufferPtr!=buffer; }
        };

        typedef std::vector<VectorInputPort*> PortVector;

    private:
        PortVector ports;
        std::string fileHeader;
        std::string fileName;
        std::string indexFileName;
        int blockSize;
        FILE *f;
        FILE *fi;
        int prec;

    public:
        IndexedVectorFileWriterNode(const char *fileName, const char* indexFileName, int blockSize, const char *fileHeader=NULL);
        virtual ~IndexedVectorFileWriterNode();

        Port *addVector(int vectorId, std::string moduleName, std::string name);
        void setPrecision(int prec) {this->prec = prec;}
        std::string filename() const {return fileName;}

        virtual bool isReady() const;
        virtual void process();
        virtual bool finished() const;
    protected:
        void writeRecordsToBuffer(VectorInputPort *port);
        void writeBufferToFile(VectorInputPort *port);
        void writeIndex(VectorInputPort *port);
};

class IndexedVectorFileWriterNodeType : public NodeType
{
    public:
        virtual const char *name() const {return "indexedvectorfilewriter";}
        virtual const char *category() const {return "multi-port sink";}
        virtual const char *description() const;
        virtual bool isHidden() const {return true;}
        virtual void getAttributes(StringMap& attrs) const;
        virtual Node *create(DataflowManager *mgr, StringMap& attrs) const;
        virtual Port *getPort(Node *node, const char *portname) const;
};

#endif


