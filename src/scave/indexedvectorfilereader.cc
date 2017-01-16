//=========================================================================
//  INDEXEDVECTORFILEREADER.CC - part of
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

#include <algorithm>
#include "common/opp_ctype.h"
#include "omnetpp/platdep/platmisc.h"
#include "channel.h"
#include "scaveutils.h"
#include "vectorfilereader.h"
#include "indexedvectorfilereader.h"

using namespace std;
using namespace omnetpp::common;

namespace omnetpp {
namespace scave {

IndexedVectorFileReaderNode::IndexedVectorFileReaderNode(const char *filename, size_t bufferSize) :
    FileReaderNode(filename, bufferSize), index(nullptr), currentBlockIndex(0)
{
}

IndexedVectorFileReaderNode::~IndexedVectorFileReaderNode()
{
    if (index) {
        delete index;
        index = nullptr;
    }
}

Port *IndexedVectorFileReaderNode::addVector(const VectorResult& vector)
{
    return addVector(vector.getVectorId());
}

Port *IndexedVectorFileReaderNode::addVector(int vectorId)
{
    PortData& portdata = ports[vectorId];
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

    long bytesRead = 0;
    while (currentBlockIndex < blocksToRead.size() && bytesRead < 64 * 1024) {
        BlockAndPortData& blockAndPort = blocksToRead[currentBlockIndex++];
        bytesRead += readBlock(blockAndPort.blockPtr, blockAndPort.portDataPtr);
    }
}

bool IndexedVectorFileReaderNode::isFinished() const
{
    return index && currentBlockIndex >= blocksToRead.size();
}

void IndexedVectorFileReaderNode::readIndexFile()
{
    const char *fn = filename.c_str();

    if (!IndexFile::isExistingVectorFile(fn))
        throw opp_runtime_error("Indexed vector file reader: Not a vector file, file %s", fn);
    if (!IndexFile::isIndexFileUpToDate(fn))
        throw opp_runtime_error("Indexed vector file reader: Index file is not up to date, file %s", fn);

    string indexFileName = IndexFile::getIndexFileName(fn);
    IndexFileReader reader(indexFileName.c_str());
    index = reader.readAll();

    for (VectorIdToPortMap::iterator it = ports.begin(); it != ports.end(); ++it) {
        int vectorId = it->first;
        PortData& portData = it->second;

        portData.vector = index->getVectorById(vectorId);

        if (!portData.vector)
            throw opp_runtime_error("Indexed vector file reader: Vector %d not found, file %s",
                    vectorId, indexFileName.c_str());

        Blocks& blocks = portData.vector->blocks;
        for (Blocks::iterator it = blocks.begin(); it != blocks.end(); ++it)
            blocksToRead.push_back(BlockAndPortData(&(*it), &portData));
    }

    sort(blocksToRead.begin(), blocksToRead.end());
}

long IndexedVectorFileReaderNode::readBlock(const Block *blockPtr, const PortData *portDataPtr)
{
    assert(blockPtr);
    assert(portDataPtr->vector);

    const char *file = filename.c_str();
    file_offset_t offset;
#define CHECK(cond, msg) {if (!(cond)) throw opp_runtime_error(msg ", file %s, offset %" PRId64, file, (int64_t)offset); }

    VectorData *vector = portDataPtr->vector;
    file_offset_t startOffset = blockPtr->startOffset;
    long count = blockPtr->getCount();

    reader.seekTo(startOffset);

    char *line;
    for (long k = 0; k < count  /*&& (line=reader.getNextLineBufferPointer())!=nullptr*/; ++k) {
        line = reader.getNextLineBufferPointer();

        if (!line)
            break;

        offset = reader.getCurrentLineStartOffset();
        int length = reader.getCurrentLineLength();
        tokenizer.tokenize(line, length);

        int numtokens = tokenizer.numTokens();
        char **vec = tokenizer.tokens();
        int vectorId;

        // check vector id
        CHECK((numtokens >= 3) && opp_isdigit(vec[0][0]), "vector file reader: data line too short");
        CHECK(parseInt(vec[0], vectorId), "invalid vector file syntax: invalid vector id column");
        CHECK(vectorId == vector->vectorId, "vector file reader: unexpected vector id");

        // parse columns
        Datum a;
        a = parseColumns(vec, numtokens, vector->columns, file, -1, offset);

        // write to port(s)
        for (PortVector::const_iterator port = portDataPtr->ports.begin(); port != portDataPtr->ports.end(); ++port)
            port->getChannel()->write(&a, 1);
    }

    return blockPtr->size;
}

//-----

const char *IndexedVectorFileReaderNodeType::getDescription() const
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
    int vectorId;
    if (!parseInt(portname, vectorId))
        throw opp_runtime_error("Indexed file reader node: Port should be a vector id, received: %s", portname);
    return node1->addVector(vectorId);
}

}  // namespace scave
}  // namespace omnetpp

