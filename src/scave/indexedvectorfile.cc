//=========================================================================
//  INDEXEDVECTORFILE.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#include "exception.h"
#include "linetokenizer.h"
#include "channel.h"
#include "stringutil.h"
#include "indexedvectorfile.h"
#include "scaveutils.h"

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

USING_NAMESPACE

//=========================================================================

IndexedVectorFileReader::IndexedVectorFileReader(const char *filename, long vectorId)
    : fname(filename), index(NULL), vector(NULL), currentBlock(NULL)
{
    std::string ifname = IndexFile::getIndexFileName(filename);
    IndexFileReader indexReader(ifname.c_str());
    index = indexReader.readAll(); // XXX do not read whole index
    vector = index->getVectorById(vectorId);
    
    if (!vector)
    	throw opp_runtime_error("Vector with vectorId %ld not found in file '%s'",
    			vectorId, filename);
}

IndexedVectorFileReader::~IndexedVectorFileReader()
{
    if (index != NULL)
        delete index;
}

// see filemgrs.h
#define MIN_BUFFER_SIZE 512

#ifdef CHECK
#undef CHECK
#endif
#define CHECK(cond, msg, block, line) \
            if (!(cond))\
            {\
                throw opp_runtime_error("Invalid vector file syntax: %s, file %s, block offset %ld, line in block %d", \
                                        msg, fname.c_str(), block.startOffset, line);\
            }

void IndexedVectorFileReader::loadBlock(const Block &block)
{
    if (currentBlock == &block)
        return;

    if (currentBlock != NULL) {
        currentBlock = NULL;
        currentEntries.clear();
    }

    size_t bufferSize = vector->blockSize;
    if (bufferSize < MIN_BUFFER_SIZE)
        bufferSize = MIN_BUFFER_SIZE;
    FileReader reader(fname.c_str(), bufferSize);

    long count=block.count();
    reader.seekTo(block.startOffset);
    currentEntries.resize(count);

    char *line, **tokens;
    int numTokens;
    LineTokenizer tokenizer;
    long id;

    std::string columns = vector->columns;
    int columnsNo = columns.size();

    for (int i=0; i<count; ++i)
    {
        CHECK(line=reader.getNextLineBufferPointer(), "Unexpected end of file", block, i);
        int len = reader.getCurrentLineLength();

        tokenizer.tokenize(line, len);
        tokens=tokenizer.tokens();
        numTokens = tokenizer.numTokens();

        CHECK(numTokens >= (int)columns.size() + 1, "Line is too short", block, i);
        CHECK(parseLong(tokens[0],id) && id==vector->vectorId, "Missing or unexpected vector id", block, i);

        OutputVectorEntry &entry = currentEntries[i];
        entry.serial = block.startSerial+i;
        for (int j = 0; j < columnsNo; ++j)
        {
            switch (columns[j])
            {
            case 'E': CHECK(parseLong(tokens[j+1], entry.eventNumber), "Malformed event number", block, i); break;
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
    if (serial<0 || serial>=vector->count())
        return NULL;

    if (currentBlock == NULL || !currentBlock->contains(serial))
    {
        loadBlock(*(vector->getBlockBySerial(serial)));
    }

    return &currentEntries.at(serial - currentBlock->startSerial);
}

OutputVectorEntry *IndexedVectorFileReader::getEntryBySimtime(simultime_t simtime, bool after)
{
    const Block *block = vector->getBlockBySimtime(simtime, after);
    if (block)
    {
        loadBlock(*block);
        if (after)
        {
            for (Entries::iterator it = currentEntries.begin(); it != currentEntries.end(); ++it) // FIXME: binary search
                if (it->simtime >= simtime)
                    return &(*it);
        }
        else
        {
            for (Entries::reverse_iterator it = currentEntries.rbegin(); it != currentEntries.rend(); ++it)  // FIXME: binary search
                if (it->simtime <= simtime)
                    return &(*it);
        }
    }
    return NULL;
}

OutputVectorEntry *IndexedVectorFileReader::getEntryByEventnum(long eventNum, bool after)
{
    const Block *block = vector->getBlockByEventnum(eventNum, after);
    if (block)
    {
        loadBlock(*block);
        if (after)
        {
            for (Entries::iterator it = currentEntries.begin(); it != currentEntries.end(); ++it) // FIXME: binary search
                if (it->eventNumber >= eventNum)
                    return &(*it);
        }
        else
        {
            for (Entries::reverse_iterator it = currentEntries.rbegin(); it != currentEntries.rend(); ++it) // FIXME: binary search
                if (it->eventNumber <= eventNum)
                    return &(*it);
        }
    }
    return NULL;
}

long IndexedVectorFileReader::collectEntriesInSimtimeInterval(simultime_t startTime, simultime_t endTime, Entries &out)
{
    Blocks::size_type startIndex;
    Blocks::size_type endIndex;
    vector->getBlocksInSimtimeInterval(startTime, endTime, /*out*/ startIndex, /*out*/ endIndex);

    Entries::size_type count = 0;
    for (Blocks::size_type i = startIndex; i < endIndex; i++)
    {
        const Block &block = vector->blocks[i];
        loadBlock(block);
        for (long j = 0; j < block.count(); ++j)
        {
            OutputVectorEntry &entry = currentEntries[j];
            if (startTime <= entry.simtime && entry.simtime <= endTime)
            {
                out.push_back(entry);
                count++;
            }
            else if (entry.simtime > endTime)
                break;
        }
    }
    return count;
}

long IndexedVectorFileReader::collectEntriesInEventnumInterval(long startEventNum, long endEventNum, Entries &out)
{
    Blocks::size_type startIndex;
    Blocks::size_type endIndex;
    vector->getBlocksInEventnumInterval(startEventNum, endEventNum, /*out*/ startIndex, /*out*/ endIndex);

    Entries::size_type count = 0;
    for (Blocks::size_type i = startIndex; i < endIndex; i++)
    {
        const Block &block = vector->blocks[i];
        loadBlock(block);

        for (long j = 0; j < block.count(); ++j)
        {
            OutputVectorEntry &entry = currentEntries[j];
            if (startEventNum <= entry.eventNumber && entry.eventNumber <= endEventNum)
            {
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
#define CHECK(printf) if (printf<0) throw opp_runtime_error("Cannot write vector file '%s'", fileName.c_str());


static FILE *openFile(const std::string fileName)
{
    FILE *f = fopen(fileName.c_str(),"w");
    if (f==NULL)
        throw opp_runtime_error("Cannot open vector file `%s'", fileName.c_str());
    return f;
}

IndexedVectorFileWriterNode::IndexedVectorFileWriterNode(const char *fileName, const char *indexFileName, int blockSize, const char *fileHeader)
{
    f = NULL;
    indexWriter = NULL;
    this->prec = DEFAULT_PRECISION;
    this->fileHeader = (fileHeader ? fileHeader : "");
    this->fileName = fileName;
    this->indexFileName = indexFileName;
    this->blockSize = blockSize;
}

IndexedVectorFileWriterNode::~IndexedVectorFileWriterNode()
{
    for (PortVector::iterator it=ports.begin(); it!=ports.end(); it++)
        delete *it;
}

Port *IndexedVectorFileWriterNode::addVector(int vectorId, const char *module, const char *name, const char *columns)
{
    VectorInputPort *inputport = new VectorInputPort(vectorId, module, name, columns, blockSize, this);
    ports.push_back(inputport);
    return inputport;
}

bool IndexedVectorFileWriterNode::isReady() const
{
    for (PortVector::const_iterator it=ports.begin(); it!=ports.end(); it++)
    {
        VectorInputPort *port=*it;
        if (port->channel()->length()>0 || port->channel()->closing() && port->hasBufferedData())
            return true;
    }
    return false;
}

void IndexedVectorFileWriterNode::process()
{
    // open file if needed
    if (!f)
    {
        f = openFile(fileName);
        // print file header and vector declarations
        CHECK(fprintf(f,"%s\n", fileHeader.c_str()));
        for (PortVector::iterator it=ports.begin(); it!=ports.end(); it++)
        {
            VectorInputPort *port = *it;
            CHECK(fprintf(f, "vector %d  %s  %s  %s\n", port->vector.vectorId,
                             QUOTE(port->vector.moduleName.c_str()),
                             QUOTE(port->vector.name.c_str()),
                             port->vector.columns.c_str()));
        }
    }

    for (PortVector::iterator it=ports.begin(); it!=ports.end(); it++)
    {
        VectorInputPort *port=*it;
        if (!port->finished)
        {
	        if (port->channel()->length()>0)
	            writeRecordsToBuffer(port);
	        if (port->channel()->eof()) {
	            if (port->hasBufferedData())
	                writeBufferToFile(port);
	            writeIndex(port);
	            port->finished = true;
	        }
        }
    }

}

bool IndexedVectorFileWriterNode::finished() const
{
    for (PortVector::const_iterator it=ports.begin(); it!=ports.end(); it++)
    {
        VectorInputPort *port=*it;
        if (!port->finished)
            return false;
    }

    // close output vector and index files
    if (f != NULL)
        fclose(f);
    if (indexWriter != NULL)
    {
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
    port->bufferPtr+=count;
}

void IndexedVectorFileWriterNode::writeRecordsToBuffer(VectorInputPort *port)
{
    assert(port->vector.blocks.size() > 0);

    int vectorId = port->vector.vectorId;
    Channel *chan = port->channel();
    int n = chan->length();
    std::string &columns = port->vector.columns;
    int colno = columns.size();
    Datum a;
    char buf[64];
    char *endp;

    if (colno == 2 && columns[0] == 'T' && columns[1] == 'V')
    {
        for (int i=0; i<n; i++)
        {
            chan->read(&a,1);
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
    else if (colno == 3 && columns[0] == 'E' && columns[1] == 'T' && columns[2] == 'V')
    {
        for (int i=0; i<n; i++)
        {
            chan->read(&a,1);
            if (port->bufferPtr - port->buffer >= port->bufferSize - 100)
                writeBufferToFile(port);
            if (a.xp.isNil())
                bufferPrintf(port, "%d\t%ld\t%.*g\t%.*g\n", vectorId, a.eventNumber, prec, a.x, prec, a.y);
            else
                bufferPrintf(port, "%d\t%ld\t%s\t%.*g\n", vectorId, a.eventNumber, BigDecimal::ttoa(buf, a.xp, endp), prec, a.y);
            port->bufferNumOfRecords++;
            port->vector.blocks.back().collect(a.eventNumber, a.x, a.y);
        }
    }
    else
    {
        for (int i=0; i<n; i++)
        {
            chan->read(&a,1);
            bufferPrintf(port,"%d", vectorId);
            for (int j=0; j<colno; ++j)
            {
                bufferPrintf(port, "\t");
                switch (columns[j])
                {
                case 'T':
                    if (a.xp.isNil())
                        bufferPrintf(port,"%.*g", prec, a.x);
                    else
                        bufferPrintf(port,"%s", BigDecimal::ttoa(buf, a.xp, endp)); break;
                case 'V': bufferPrintf(port,"%.*g", prec, a.y); break;
                case 'E': bufferPrintf(port,"%ld", a.eventNumber); break;
                default: throw opp_runtime_error("unknown column type: '%c'", columns[j]);
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
    assert(f!=NULL);
    assert(port->vector.blocks.size() > 0);

    Block &currentBlock = port->vector.blocks.back();
    currentBlock.startOffset = ftell(f);

    CHECK(fputs(port->buffer, f));
    currentBlock.size = (long)(ftell(f) - currentBlock.startOffset);
    port->vector.collect(currentBlock);
    port->clearBuffer();
    port->vector.blocks.push_back(Block());
}

void IndexedVectorFileWriterNode::writeIndex(VectorInputPort *port)
{
    if (indexWriter == NULL)
    {
        indexWriter = new IndexFileWriter(indexFileName.c_str(), prec);
        indexWriter->writeRun(run);
    }

    indexWriter->writeVector(port->vector);
}


//=========================================================================

const char *IndexedVectorFileWriterNodeType::description() const
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
    if (numTokens < 3 || numTokens > 4)
    {
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

    const char* moduleName = tokens[1];
    const char* name = tokens[2];
    const char* columns = (numTokens < 4 ? "TV" : tokens[3]);
    return node1->addVector(vectorId, moduleName, name, columns);
}


