//=========================================================================
//  INDEXEDVECTORFILE.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef _INDEXEDVECTORFILE_H_
#define _INDEXEDVECTORFILE_H_

#include <assert.h>
#include <float.h>
#include <vector>
#include "filereader.h"
#include "indexfile.h"
#include "node.h"
#include "nodetype.h"


struct OutputVectorEntry {
    long serial;
    double simtime;
    double value;

    OutputVectorEntry() {}
    OutputVectorEntry(long serial, double simtime, double value)
        : serial(serial), simtime(simtime), value(value) {}
};

struct BlockWithEntries {
    Block block;
    OutputVectorEntry *entries;

    BlockWithEntries(Block block)
        : block(block), entries(NULL) {}
    ~BlockWithEntries() { if(entries) { delete entries; entries=NULL; } }
    long numOfEntries() { return block.numOfEntries(); }
    bool contains(long serial) { return block.contains(serial); }
    OutputVectorEntry* getEntryBySerial(long serial) { assert(entries != NULL); return &entries[serial - block.startSerial]; }
};

/**
 * Vector file reader with random access.
 * Each instance reads one vector from a vector file.
 */
class IndexedVectorFileReader
{
    std::string fname;
    FileReader *reader;
    
    VectorFileIndex *index;
    VectorData *vector;
    BlockWithEntries *currentBlock;

    public:
        explicit IndexedVectorFileReader(const char* filename, long vectorId);
        ~IndexedVectorFileReader();
    protected:
        // reads a block from the vector file
        BlockWithEntries *loadBlock(Block &block);
    public:
        int getNumberOfEntries() { return vector->count; };
        OutputVectorEntry *getEntryBySerial(long serial);
};

/**
 * Consumer node which writes an indexed output vector file.
 */
class IndexedVectorFileWriterNode : public Node
{
    protected:
        class VectorInputPort : public Port {
            public:
            VectorData vector;
            char *buffer;     // buffer holding recorded data
            int bufferSize;  // size of the allocated buffer
            char *bufferPtr; // pointer to the current position in the buffer
            int bufferNumOfRecords; //

            VectorInputPort(int id, std::string moduleName, std::string name, int bufferSize, Node *owner)
                : vector(id, moduleName, name, bufferSize), Port(owner)
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
        IndexFileWriter *indexWriter;
        int prec;

    public:
        IndexedVectorFileWriterNode(const char *fileName, const char* indexFileName, int blockSize, const char *fileHeader=NULL);
        virtual ~IndexedVectorFileWriterNode();

        Port *addVector(int vectorId, std::string moduleName, std::string name);
        void setPrecision(int prec) {this->prec = prec;}
        void setHeader(const std::string &header) { fileHeader = header; }
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
