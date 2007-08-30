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
#include "filereader.h"
#include "linetokenizer.h"
#include "indexfile.h"
#include "resultfilemanager.h"

// read in 64K chunks (apparently it doesn't matter much if we use a bigger buffer)
#define VECFILEREADER_BUFSIZE  (64*1024)


/**
 * Producer node which reads an output vector file.
 */
class SCAVE_API IndexedVectorFileReaderNode : public Node
{
    typedef std::vector<Port> PortVector;

    struct PortData {
		VectorData *vector;
		int currentBlockIndex;
		PortVector ports;
		
		PortData() : vector(NULL), currentBlockIndex(0) {}
	};

    typedef std::map<int,PortData> PortMap;

	private:
    	std::string filename;
        PortMap ports;
        VectorFileIndex *index;
        FileReader reader;
        LineTokenizer tokenizer;
        bool fFinished;

    public:
        IndexedVectorFileReaderNode(const char *filename, size_t bufferSize = VECFILEREADER_BUFSIZE);
        virtual ~IndexedVectorFileReaderNode();

        Port *addVector(const VectorResult &vector);

        virtual bool isReady() const;
        virtual void process();
        virtual bool finished() const;

    private:
        void readIndexFile();
        bool readNextBlock(PortData& portData);
};


class SCAVE_API IndexedVectorFileReaderNodeType : public NodeType
{
    public:
        virtual const char *name() const {return "indexedvectorfilereader";}
        virtual const char *category() const {return "multi-port source";}
        virtual const char *description() const;
        virtual bool isHidden() const {return true;}
        virtual void getAttributes(StringMap& attrs) const;
        virtual Node *create(DataflowManager *mgr, StringMap& attrs) const;
        virtual Port *getPort(Node *node, const char *portname) const;
};


#endif


