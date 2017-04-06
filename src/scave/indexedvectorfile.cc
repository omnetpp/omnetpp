//=========================================================================
//  INDEXEDVECTORFILE.CC - part of
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

#include <clocale>
#include <cstdlib>
#include "common/exception.h"
#include "common/linetokenizer.h"
#include "common/stringutil.h"
#include "omnetpp/platdep/platmisc.h"
#include "channel.h"
#include "indexedvectorfile.h"
#include "scaveutils.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace scave {

#define VECTOR_FILE_VERSION 2

//=========================================================================

IndexedVectorFileReader::IndexedVectorFileReader(const char *filename, int vectorId)
    : fname(filename), index(nullptr), vector(nullptr), currentBlock(nullptr)
{
    std::string ifname = IndexFile::getIndexFileName(filename);
    IndexFileReader indexReader(ifname.c_str());
    index = indexReader.readAll();  // XXX do not read whole index
    vector = index->getVectorById(vectorId);

    if (!vector)
        throw opp_runtime_error("Vector with vectorId %d not found in file '%s'",
                vectorId, filename);
}

IndexedVectorFileReader::~IndexedVectorFileReader()
{
    if (index != nullptr)
        delete index;
}

// see filemgrs.h
#define MIN_BUFFER_SIZE    512

#ifdef CHECK
#undef CHECK
#endif
#define CHECK(cond, msg, block, line) \
            if (!(cond))\
            {\
                throw opp_runtime_error("Invalid vector file syntax: %s, file %s, block offset %" PRId64 ", line in block %d", \
                                        msg, fname.c_str(), (int64_t)block.startOffset, line);\
            }

void IndexedVectorFileReader::loadBlock(const Block& block)
{
    if (currentBlock == &block)
        return;

    if (currentBlock != nullptr) {
        currentBlock = nullptr;
        currentEntries.clear();
    }

    size_t bufferSize = vector->blockSize;
    if (bufferSize < MIN_BUFFER_SIZE)
        bufferSize = MIN_BUFFER_SIZE;
    FileReader reader(fname.c_str(), bufferSize);

    long count = block.getCount();
    reader.seekTo(block.startOffset);
    currentEntries.resize(count);

    char *line, **tokens;
    int numTokens;
    LineTokenizer tokenizer;
    int id;

    std::string columns = vector->columns;
    int columnsNo = columns.size();

    for (int i = 0; i < count; ++i) {
        CHECK(line = reader.getNextLineBufferPointer(), "Unexpected end of file", block, i);
        int len = reader.getCurrentLineLength();

        tokenizer.tokenize(line, len);
        tokens = tokenizer.tokens();
        numTokens = tokenizer.numTokens();

        CHECK(numTokens >= (int)columns.size() + 1, "Line is too short", block, i);
        CHECK(parseInt(tokens[0], id) && id == vector->vectorId, "Missing or unexpected vector id", block, i);

        OutputVectorEntry& entry = currentEntries[i];
        entry.serial = block.startSerial+i;
        for (int j = 0; j < columnsNo; ++j) {
            switch (columns[j]) {
                case 'E': CHECK(parseInt64(tokens[j+1], entry.eventNumber), "Malformed event number", block, i); break;
                case 'T': CHECK(parseSimtime(tokens[j+1], entry.simtime), "Malformed simulation time", block, i); break;
                case 'V': CHECK(parseDouble(tokens[j+1], entry.value), "Malformed vector value", block, i); break;
                default: CHECK(false, "Unknown column", block, i); break;
            }
        }
    }

    currentBlock = &block;
}

OutputVectorEntry *IndexedVectorFileReader::getEntryBySerial(long serial)
{
    if (serial < 0 || serial >= vector->getCount())
        return nullptr;

    if (currentBlock == nullptr || !currentBlock->contains(serial)) {
        loadBlock(*(vector->getBlockBySerial(serial)));
    }

    return &currentEntries.at(serial - currentBlock->startSerial);
}

OutputVectorEntry *IndexedVectorFileReader::getEntryBySimtime(simultime_t simtime, bool after)
{
    const Block *block = vector->getBlockBySimtime(simtime, after);
    if (block) {
        loadBlock(*block);
        if (after) {
            for (Entries::iterator it = currentEntries.begin(); it != currentEntries.end(); ++it)  // FIXME: binary search
                if (it->simtime >= simtime)
                    return &(*it);

        }
        else {
            for (Entries::reverse_iterator it = currentEntries.rbegin(); it != currentEntries.rend(); ++it)  // FIXME: binary search
                if (it->simtime <= simtime)
                    return &(*it);

        }
    }
    return nullptr;
}

OutputVectorEntry *IndexedVectorFileReader::getEntryByEventnum(eventnumber_t eventNum, bool after)
{
    const Block *block = vector->getBlockByEventnum(eventNum, after);
    if (block) {
        loadBlock(*block);
        if (after) {
            for (Entries::iterator it = currentEntries.begin(); it != currentEntries.end(); ++it)  // FIXME: binary search
                if (it->eventNumber >= eventNum)
                    return &(*it);

        }
        else {
            for (Entries::reverse_iterator it = currentEntries.rbegin(); it != currentEntries.rend(); ++it)  // FIXME: binary search
                if (it->eventNumber <= eventNum)
                    return &(*it);

        }
    }
    return nullptr;
}

long IndexedVectorFileReader::collectEntriesInSimtimeInterval(simultime_t startTime, simultime_t endTime, Entries& out)
{
    Blocks::size_type startIndex;
    Blocks::size_type endIndex;
    vector->getBlocksInSimtimeInterval(startTime, endTime,  /*out*/ startIndex,  /*out*/ endIndex);

    Entries::size_type count = 0;
    for (Blocks::size_type i = startIndex; i < endIndex; i++) {
        const Block& block = vector->blocks[i];
        loadBlock(block);
        for (long j = 0; j < block.getCount(); ++j) {
            OutputVectorEntry& entry = currentEntries[j];
            if (startTime <= entry.simtime && entry.simtime <= endTime) {
                out.push_back(entry);
                count++;
            }
            else if (entry.simtime > endTime)
                break;
        }
    }
    return count;
}

long IndexedVectorFileReader::collectEntriesInEventnumInterval(eventnumber_t startEventNum, eventnumber_t endEventNum, Entries& out)
{
    Blocks::size_type startIndex;
    Blocks::size_type endIndex;
    vector->getBlocksInEventnumInterval(startEventNum, endEventNum,  /*out*/ startIndex,  /*out*/ endIndex);

    Entries::size_type count = 0;
    for (Blocks::size_type i = startIndex; i < endIndex; i++) {
        const Block& block = vector->blocks[i];
        loadBlock(block);

        for (long j = 0; j < block.getCount(); ++j) {
            OutputVectorEntry& entry = currentEntries[j];
            if (startEventNum <= entry.eventNumber && entry.eventNumber <= endEventNum) {
                out.push_back(entry);
                count++;
            }
            else if (entry.eventNumber > endEventNum)
                break;
        }
    }
    return count;
}

//=========================================================================

#ifdef CHECK
#undef CHECK
#endif
#define CHECK(printf) if ((printf)<0) throw opp_runtime_error("Cannot write vector file '%s'", fileName.c_str());


static FILE *openFile(const std::string& fileName)
{
    FILE *f = fopen(fileName.c_str(), "w");
    if (f == nullptr)
        throw opp_runtime_error("Cannot open vector file '%s'", fileName.c_str());
    setlocale(LC_NUMERIC, "C");  // write '.' as decimal marker
    return f;
}

IndexedVectorFileWriterNode::IndexedVectorFileWriterNode(const char *fileName, const char *indexFileName, int blockSize, const char *fileHeader)
{
    f = nullptr;
    indexWriter = nullptr;
    this->prec = DEFAULT_RESULT_PRECISION;
    this->fileHeader = (fileHeader ? fileHeader : "");
    this->fileName = fileName;
    this->indexFileName = indexFileName;
    this->blockSize = blockSize;
}

IndexedVectorFileWriterNode::~IndexedVectorFileWriterNode()
{
    for (PortVector::iterator it = ports.begin(); it != ports.end(); ++it)
        delete *it;
}

Port *IndexedVectorFileWriterNode::addVector(int vectorId, const char *module, const char *name, const char *columns)
{
    VectorInputPort *inputport = new VectorInputPort(vectorId, module, name, columns, blockSize, this);
    ports.push_back(inputport);
    return inputport;
}

Port *IndexedVectorFileWriterNode::addVector(const VectorResult& vector)
{
    VectorInputPort *inputport = new VectorInputPort(vector.getVectorId(), vector.getModuleName().c_str(), vector.getName().c_str(),
                vector.getColumns().c_str(), blockSize, this);
    inputport->vector.attributes = vector.getAttributes();
    ports.push_back(inputport);
    return inputport;
}

bool IndexedVectorFileWriterNode::isReady() const
{
    for (PortVector::const_iterator it = ports.begin(); it != ports.end(); ++it) {
        VectorInputPort *port = *it;
        if (port->getChannel()->length() > 0 || (port->getChannel()->isClosing() && port->hasBufferedData()))
            return true;
    }
    return false;
}

void IndexedVectorFileWriterNode::process()
{
    // open file if needed
    if (!f) {
        f = openFile(fileName);
        // print file header
        CHECK(fprintf(f, "%s\n", fileHeader.c_str()));
        CHECK(fprintf(f, "version %d\n", VECTOR_FILE_VERSION));

        // print run attributes
        run.writeToFile(f, fileName.c_str());

        // print vector declarations and attributes
        for (PortVector::iterator it = ports.begin(); it != ports.end(); ++it) {
            const VectorData& vector = (*it)->vector;
            CHECK(fprintf(f, "vector %d  %s  %s  %s\n",
                            vector.vectorId, QUOTE(vector.moduleName.c_str()), QUOTE(vector.name.c_str()), vector.columns.c_str()));
            for (StringMap::const_iterator attr = vector.attributes.begin(); attr != vector.attributes.end(); attr++)
                CHECK(fprintf(f, "attr %s  %s\n", QUOTE(attr->first.c_str()), QUOTE(attr->second.c_str())));
        }
    }

    for (PortVector::iterator it = ports.begin(); it != ports.end(); ++it) {
        VectorInputPort *port = *it;
        if (!port->finished) {
            if (port->getChannel()->length() > 0)
                writeRecordsToBuffer(port);
            if (port->getChannel()->eof()) {
                if (port->hasBufferedData())
                    writeBufferToFile(port);
                writeIndex(port);
                port->finished = true;
            }
        }
    }
}

bool IndexedVectorFileWriterNode::isFinished() const
{
    for (PortVector::const_iterator it = ports.begin(); it != ports.end(); ++it) {
        VectorInputPort *port = *it;
        if (!port->finished)
            return false;
    }

    // close output vector and index files
    if (f != nullptr)
        fclose(f);
    if (indexWriter != nullptr) {
        indexWriter->writeFingerprint(fileName);
        delete indexWriter;
    }

    return true;
}

void IndexedVectorFileWriterNode::bufferPrintf(VectorInputPort *port, const char *format...)
{
    va_list va;
    va_start(va, format);
    int count = vsprintf(port->bufferPtr, format, va);
    va_end(va);
    if (count < 0)
        throw opp_runtime_error("Cannot write data to output buffer");
    port->bufferPtr += count;
}

void IndexedVectorFileWriterNode::writeRecordsToBuffer(VectorInputPort *port)
{
    assert(port->vector.blocks.size() > 0);

    int vectorId = port->vector.vectorId;
    Channel *chan = port->getChannel();
    int n = chan->length();
    std::string& columns = port->vector.columns;
    int colno = columns.size();
    Datum a;
    char buf[64];
    char *endp;

    if (colno == 2 && columns[0] == 'T' && columns[1] == 'V') {
        for (int i = 0; i < n; i++) {
            chan->read(&a, 1);
            if (port->bufferPtr - port->buffer >= port->bufferSize - 100)
                writeBufferToFile(port);
            if (a.xp.isNil())
                bufferPrintf(port, "%d\t%.*g\t%.*g\n", vectorId, prec, a.x, prec, a.y);
            else
                bufferPrintf(port, "%d\t%s\t%.*g\n", vectorId, BigDecimal::ttoa(buf, a.xp, endp), prec, a.y);
            port->bufferNumOfRecords++;
            port->vector.blocks.back().collect(-1, a.x, a.y);
        }
    }
    else if (colno == 3 && columns[0] == 'E' && columns[1] == 'T' && columns[2] == 'V') {
        for (int i = 0; i < n; i++) {
            chan->read(&a, 1);
            if (port->bufferPtr - port->buffer >= port->bufferSize - 100)
                writeBufferToFile(port);
            if (a.xp.isNil())
                bufferPrintf(port, "%d\t%" PRId64 "\t%.*g\t%.*g\n", vectorId, a.eventNumber, prec, a.x, prec, a.y);
            else
                bufferPrintf(port, "%d\t%" PRId64 "\t%s\t%.*g\n", vectorId, a.eventNumber, BigDecimal::ttoa(buf, a.xp, endp), prec, a.y);
            port->bufferNumOfRecords++;
            port->vector.blocks.back().collect(a.eventNumber, a.x, a.y);
        }
    }
    else {
        for (int i = 0; i < n; i++) {
            chan->read(&a, 1);
            bufferPrintf(port, "%d", vectorId);
            for (int j = 0; j < colno; ++j) {
                bufferPrintf(port, "\t");
                switch (columns[j]) {
                    case 'T':
                        if (a.xp.isNil())
                            bufferPrintf(port, "%.*g", prec, a.x);
                        else
                            bufferPrintf(port, "%s", BigDecimal::ttoa(buf, a.xp, endp));
                        break;

                    case 'V':
                        bufferPrintf(port, "%.*g", prec, a.y);
                        break;

                    case 'E':
                        bufferPrintf(port, "%" PRId64, a.eventNumber);
                        break;

                    default:
                        throw opp_runtime_error("Unknown column type: '%c'", columns[j]);
                }
            }
            bufferPrintf(port, "\n");
            port->bufferNumOfRecords++;
            port->vector.blocks.back().collect(a.eventNumber, a.x, a.y);
        }
    }
}

void IndexedVectorFileWriterNode::writeBufferToFile(VectorInputPort *port)
{
    assert(f != nullptr);
    assert(port->vector.blocks.size() > 0);

    Block& currentBlock = port->vector.blocks.back();
    currentBlock.startOffset = opp_ftell(f);

    CHECK(fputs(port->buffer, f));
    currentBlock.size = (int64_t)(opp_ftell(f) - currentBlock.startOffset);
    port->vector.collect(currentBlock);
    port->clearBuffer();
    port->vector.blocks.push_back(Block());
}

void IndexedVectorFileWriterNode::writeIndex(VectorInputPort *port)
{
    if (indexWriter == nullptr) {
        indexWriter = new IndexFileWriter(indexFileName.c_str(), prec);
        indexWriter->writeRun(run);
    }

    indexWriter->writeVector(port->vector);
}

//=========================================================================

const char *IndexedVectorFileWriterNodeType::getDescription() const
{
    return "Writes the output (several streams) into an indexed output vector file.";
}

void IndexedVectorFileWriterNodeType::getAttributes(StringMap& attrs) const
{
    attrs["filename"] = "name of the output vector file (.vec)";
    attrs["indexfilename"] = "name of the output index file (.vci)";
    attrs["blocksize"] = "size of the blocks of each vector";
    attrs["fileheader"] = "header written into the output vector file";
}

Node *IndexedVectorFileWriterNodeType::create(DataflowManager *mgr, StringMap& attrs) const
{
    checkAttrNames(attrs);

    const char *fileName = attrs["filename"].c_str();
    const char *indexFileName = attrs["indexfilename"].c_str();
    int blockSize = atoi(attrs["blocksize"].c_str());
    std::string header = attrs["fileheader"];

    IndexedVectorFileWriterNode *node = new IndexedVectorFileWriterNode(fileName, indexFileName, blockSize);
    node->setHeader(header);
    node->setNodeType(this);
    mgr->addNode(node);
    return node;
}

Port *IndexedVectorFileWriterNodeType::getPort(Node *node, const char *portname) const
{
    // vector id is used as port name
    IndexedVectorFileWriterNode *node1 = dynamic_cast<IndexedVectorFileWriterNode *>(node);

    LineTokenizer tokenizer;
    int vectorId;
    int numTokens = tokenizer.tokenize(portname, strlen(portname));
    char **tokens = tokenizer.tokens();
    if (numTokens < 3 || numTokens > 4) {
        throw opp_runtime_error(
                "IndexedVectorFileWriterNodeType::getPort(): "
                "expected '<vectorId> <module> <name> [<columns>]', received '%s' ",
                portname);
    }
    if (!parseInt(tokens[0], vectorId))
        throw opp_runtime_error(
                "IndexedVectorFileWriterNodeType::getPort(): "
                "expected an integer as vectorId, received '%s'",
                tokens[0]);

    const char *moduleName = tokens[1];
    const char *name = tokens[2];
    const char *columns = (numTokens < 4 ? "TV" : tokens[3]);
    return node1->addVector(vectorId, moduleName, name, columns);
}

}  // namespace scave
}  // namespace omnetpp

