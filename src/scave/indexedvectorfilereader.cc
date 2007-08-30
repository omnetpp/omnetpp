//=========================================================================
//  VECTORFILEREADER.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include "channel.h"
#include "scaveutils.h"
#include "vectorfilereader.h"
#include "indexedvectorfilereader.h"

using namespace std;

IndexedVectorFileReaderNode::IndexedVectorFileReaderNode(const char *filename, size_t bufferSize) :
  filename(filename), reader(filename, bufferSize), tokenizer(), fFinished(false), index(NULL)
{
	
}

IndexedVectorFileReaderNode::~IndexedVectorFileReaderNode()
{
	if (index) {
		delete index;
		index = NULL;
	}
}

Port *IndexedVectorFileReaderNode::addVector(const VectorResult &vector)
{
    PortData& portdata = ports[vector.vectorId];
    portdata.ports.push_back(Port(this));
    Port& port = portdata.ports.back();
    return &port;
}

bool IndexedVectorFileReaderNode::isReady() const
{
    return true;
}

void IndexedVectorFileReaderNode::process()
{
	if (!index)
		readIndexFile();
	
	// read one block from each vector
	bool found = false;
	for (PortMap::iterator it = ports.begin(); it != ports.end(); ++it)
	{
		PortData &portData = it->second;
		found |= readNextBlock(portData);
	}
	

    if (!found)
        fFinished = true;
}

bool IndexedVectorFileReaderNode::finished() const
{
    return fFinished;
}

void IndexedVectorFileReaderNode::readIndexFile()
{
	const char *fn = filename.c_str();
	
	if (!IndexFile::isVectorFile(fn))
		throw opp_runtime_error("indexed vector file reader: not a vector file, file %s", fn);
	if (!IndexFile::isIndexFileUpToDate(fn))
		throw opp_runtime_error("indexed vector file reader: index file is not up to date, file %s", fn);
	
	string indexFileName = IndexFile::getIndexFileName(fn);
	IndexFileReader reader(indexFileName.c_str());
	index = reader.readAll();
	
	for (PortMap::iterator it = ports.begin(); it != ports.end(); ++it)
	{
		int vectorId = it->first;
		PortData &portData = it->second;
		
		portData.vector = index->getVectorById(vectorId);
		
		if (!portData.vector)
			throw opp_runtime_error("indexed vector file reader: vector %d not found, file %s",
										vectorId, indexFileName.c_str());
	}
}

bool IndexedVectorFileReaderNode::readNextBlock(PortData &portData)
{
	assert(portData.vector);
	
	VectorData *vector = portData.vector;
	if (portData.currentBlockIndex >= vector->blocks.size())
		return false;

	const char *file = filename.c_str();
	long offset;
#define CHECK(cond, msg) {if (!cond) throw opp_runtime_error(msg ", file %s, offset %ld", file, offset); }
	

	Block &block = vector->blocks[portData.currentBlockIndex++];
	long startOffset = block.startOffset;
	long count = block.count();
	
	reader.seekTo(startOffset);
	
	char *line;
	for (long k = 0; k < count && (line=reader.getNextLineBufferPointer())!=NULL; ++k)
	{
		offset = reader.getLastLineStartOffset();
        int length = reader.getLastLineLength();
        tokenizer.tokenize(line, length);

        int numtokens = tokenizer.numTokens();
        char **vec = tokenizer.tokens();
        int vectorId;
        
        // check vector id
        CHECK((numtokens >= 3) && isdigit(vec[0][0]), "vector file reader: data line too short");
        CHECK(parseInt(vec[0], vectorId), "invalid vector file syntax: invalid vector id column");
        CHECK(vectorId == vector->vectorId, "vector file reader: unexpected vector id");

        // parse columns
        Datum a = parseColumns(vec, numtokens, vector->columns, file, -1, offset);

        // write to port(s)
        for (PortVector::const_iterator port = portData.ports.begin(); port != portData.ports.end(); ++port)
            port->channel()->write(&a,1);
	}
	
	return true;
}

//-----

const char *IndexedVectorFileReaderNodeType::description() const
{
    return "Reads indexed output vector files.";
}

void IndexedVectorFileReaderNodeType::getAttributes(StringMap& attrs) const
{
    attrs["filename"] = "name of the output vector file (.vec)";
}

Node *IndexedVectorFileReaderNodeType::create(DataflowManager *mgr, StringMap& attrs) const
{
    checkAttrNames(attrs);

    const char *fname = attrs["filename"].c_str();

    Node *node = new IndexedVectorFileReaderNode(fname);
    node->setNodeType(this);
    mgr->addNode(node);
    return node;
}

Port *IndexedVectorFileReaderNodeType::getPort(Node *node, const char *portname) const
{
    // vector id is used as port name
    IndexedVectorFileReaderNode *node1 = dynamic_cast<IndexedVectorFileReaderNode *>(node);
    VectorResult vector;
    if (!parseInt(portname, vector.vectorId))
    	throw opp_runtime_error("indexed file reader node: port should be a vector id, received: %s", portname);
    return node1->addVector(vector);
}

