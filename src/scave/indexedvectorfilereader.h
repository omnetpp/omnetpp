//=========================================================================
//  INDEXEDVECTORFILEREADER.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef _INDEXEDVECTORFILEREADER_H_
#define _INDEXEDVECTORFILEREADER_H_

#include <map>
#include <string>
#include "node.h"
#include "nodetype.h"
#include "commonnodes.h"
#include "filereader.h"
#include "linetokenizer.h"
#include "indexfile.h"
#include "resultfilemanager.h"

NAMESPACE_BEGIN

// read in 64K chunks (apparently it doesn't matter much if we use a bigger buffer)
#define VECFILEREADER_BUFSIZE  (64*1024)


/**
 * Producer node which reads an output vector file.
 */
class SCAVE_API IndexedVectorFileReaderNode : public ReaderNode
{
    typedef std::vector<Port> PortVector;

    struct PortData
    {
		VectorData *vector;
		PortVector ports;
		
		PortData() : vector(NULL) {}
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

        Port *addVector(const VectorResult &vector);

        virtual bool isReady() const;
        virtual void process();
        virtual bool finished() const;

    private:
        void readIndexFile();
        long readBlock(const Block *blockPtr, const PortData *portDataPtr);
};


class SCAVE_API IndexedVectorFileReaderNodeType : public ReaderNodeType
{
    public:
        virtual const char *name() const {return "indexedvectorfilereader";}
        virtual const char *description() const;
        virtual void getAttributes(StringMap& attrs) const;
        virtual Node *create(DataflowManager *mgr, StringMap& attrs) const;
        virtual Port *getPort(Node *node, const char *portname) const;
};


NAMESPACE_END


#endif


