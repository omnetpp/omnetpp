//=========================================================================
//  INDEXEDVECTORFILEREADER.H - part of
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

#ifndef __OMNETPP_SCAVE_INDEXEDVECTORFILEREADER_H
#define __OMNETPP_SCAVE_INDEXEDVECTORFILEREADER_H

#include <map>
#include <string>
#include "common/filereader.h"
#include "common/linetokenizer.h"
#include "node.h"
#include "nodetype.h"
#include "commonnodes.h"
#include "indexfile.h"
#include "resultfilemanager.h"

namespace omnetpp {
namespace scave {

// read in 64K chunks (apparently it doesn't matter much if we use a bigger buffer)
#define VECFILEREADER_BUFSIZE  (64*1024)


/**
 * Producer node which reads an output vector file.
 */
class SCAVE_API IndexedVectorFileReaderNode : public FileReaderNode
{
    typedef omnetpp::common::LineTokenizer LineTokenizer;
    typedef std::vector<Port> PortVector;

    struct PortData
    {
        VectorData *vector;
        PortVector ports;

        PortData() : vector(nullptr) {}
    };

    struct BlockAndPortData
    {
        Block *blockPtr;
        PortData *portDataPtr;

        BlockAndPortData(Block *blockPtr, PortData *portDataPtr)
            : blockPtr(blockPtr), portDataPtr(portDataPtr) {}

        bool operator<(const BlockAndPortData& other) const
        {
            return this->blockPtr->startOffset < other.blockPtr->startOffset;
        }
    };

    typedef std::map<int,PortData> VectorIdToPortMap;

    private:
        VectorIdToPortMap ports;
        VectorFileIndex *index;
        std::vector<BlockAndPortData> blocksToRead;
        unsigned int currentBlockIndex;
        LineTokenizer tokenizer;

    public:
        IndexedVectorFileReaderNode(const char *filename, size_t bufferSize = VECFILEREADER_BUFSIZE);
        virtual ~IndexedVectorFileReaderNode();

        Port *addVector(const VectorResult& vector);
        Port *addVector(int vectorId);

        virtual bool isReady() const override;
        virtual void process() override;
        virtual bool isFinished() const override;

    private:
        void readIndexFile();
        long readBlock(const Block *blockPtr, const PortData *portDataPtr);
};


class SCAVE_API IndexedVectorFileReaderNodeType : public ReaderNodeType
{
    public:
        virtual const char *getName() const override {return "indexedvectorfilereader";}
        virtual const char *getDescription() const override;
        virtual void getAttributes(StringMap& attrs) const override;
        virtual Node *create(DataflowManager *mgr, StringMap& attrs) const override;
        virtual Port *getPort(Node *node, const char *portname) const override;
};


} // namespace scave
}  // namespace omnetpp


#endif


