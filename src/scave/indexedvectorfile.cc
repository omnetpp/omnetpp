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

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

//=========================================================================

IndexedVectorFileReader::IndexedVectorFileReader(const char *filename, long vectorId)
    : fname(filename), reader(NULL), index(NULL), vector(NULL), currentBlock(NULL)
{
    std::string ifname = IndexFile::getIndexFileName(filename);
    IndexFileReader indexReader(ifname.c_str());
    index = indexReader.readAll(); // XXX do not read whole index
    vector = index->getVector(vectorId);
}

IndexedVectorFileReader::~IndexedVectorFileReader()
{
    if (reader!=NULL)
        delete reader;
    if (index != NULL)
        delete index;
    if (currentBlock != NULL)
        delete currentBlock;
}

//=========================================================================

// see filemgrs.h
#define MIN_BUFFER_SIZE 512


static double zero =0;

static bool parseDouble(char *s, double& dest)
{
    char *e;
    dest = strtod(s,&e);
    if (!*e)
    {
        return true;
    }
    if (strstr(s,"INF") || strstr(s, "inf"))
    {
        dest = 1/zero;  // +INF or -INF
        if (*s=='-') dest = -dest;
        return true;
    }
    return false;
}

BlockWithEntries *IndexedVectorFileReader::loadBlock(Block &block)
{
    if (reader==NULL)
        reader=new FileReader(fname.c_str(), vector->blockSize);

    BlockWithEntries *result = new BlockWithEntries(block);

    long count=block.endSerial-block.startSerial;
    reader->seekTo(block.startOffset);
    result->entries=new OutputVectorEntry[count];

    char *line, **tokens, *end;
    int numTokens;
    LineTokenizer tokenizer;
    long id;
    double t, val;

    for (int i=0; i<count; ++i)
    {
        if ((line=reader->readNextLine())==NULL)
            throw opp_runtime_error("Unexpected end of file in '%s'", fname.c_str());
        int len = reader->getLastLineLength();

        tokenizer.tokenize(line, len);
        tokens=tokenizer.tokens();
        numTokens = tokenizer.numTokens();
        if (numTokens < 3)
            throw opp_runtime_error("Line to short: %.*s", len, line);

        id = strtol(tokens[0], &end, 10);
        if (*end || id!=vector->vectorId)
            throw opp_runtime_error("Missing or unexpected vector id: %.*s", len, line);

        if (!parseDouble(tokens[1], t) || !parseDouble(tokens[2], val))
            throw opp_runtime_error("Malformed line: %.*s", len, line);

        result->entries[i] = OutputVectorEntry(block.startSerial+i, t, val);
    }

    return result;
}

OutputVectorEntry *IndexedVectorFileReader::getEntryBySerial(long serial)
{
    if (serial<0 || serial>=vector->count)
        return NULL;

    if (currentBlock == NULL || !currentBlock->contains(serial))
    {
        if (currentBlock != NULL) {
            delete currentBlock;
            currentBlock = NULL;
        }
        currentBlock = loadBlock(*(vector->getBlockForEntry(serial)));
    }

    return currentBlock->getEntryBySerial(serial);
}

//=========================================================================


#define CHECK(printf) if (printf<0) throw opp_runtime_error("Cannot write vector file '%s'", fileName.c_str());


static FILE *openFile(const std::string fileName)
{
    FILE *f = fopen(fileName.c_str(),"w");
    if (f==NULL)
        throw opp_runtime_error("Cannot open index file `%s'", fileName.c_str());
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

    if (f != NULL)
        fclose(f);
    if (indexWriter != NULL)
    {
        indexWriter->writeFingerprint(fileName);
        delete indexWriter;
    }
}

Port *IndexedVectorFileWriterNode::addVector(int vectorId, std::string moduleName, std::string name)
{
    VectorInputPort *inputport = new VectorInputPort(vectorId, moduleName, name, blockSize, this);
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
            CHECK(fprintf(f, "vector %d  %s  %s  %d\n", port->vector.vectorId,
                             QUOTE(port->vector.moduleName.c_str()),
                             QUOTE(port->vector.name.c_str()), 1));
        }
    }

    for (PortVector::iterator it=ports.begin(); it!=ports.end(); it++)
    {
        VectorInputPort *port=*it;
        if (port->channel()->length()>0)
            writeRecordsToBuffer(port);
        if (port->channel()->closing()) {
            if (port->hasBufferedData())
                writeBufferToFile(port);
            writeIndex(port);
        }
    }

}

bool IndexedVectorFileWriterNode::finished() const
{
    for (PortVector::const_iterator it=ports.begin(); it!=ports.end(); it++)
    {
        VectorInputPort *port=*it;
        if (!port->channel()->closing() || port->channel()->length() > 0 || port->hasBufferedData())
            return false;
    }
    return true;
}

void IndexedVectorFileWriterNode::writeRecordsToBuffer(VectorInputPort *port)
{
    Channel *chan = port->channel();
    int n = chan->length();
    for (int i=0; i<n; i++)
    {
        Datum a;
        chan->read(&a,1);
        if (port->bufferPtr - port->buffer >= port->bufferSize - 100)
            writeBufferToFile(port);
        int count = sprintf(port->bufferPtr, "%d\t%.*g\t%.*g\n", port->vector.vectorId, prec, a.x, prec, a.y);
        if (count > 0)
        {
            port->bufferPtr+=count;
            port->bufferNumOfRecords++;
            port->vector.collect(a.y);
        }
        else
            throw opp_runtime_error("Cannot write data to output buffer.");
    }
}

void IndexedVectorFileWriterNode::writeBufferToFile(VectorInputPort *port)
{
    assert(f!=NULL);
    long offset = ftell(f);
    CHECK(fputs(port->buffer, f));
    port->vector.addBlock(offset, port->bufferNumOfRecords);
    port->clearBuffer();
}

void IndexedVectorFileWriterNode::writeIndex(VectorInputPort *port)
{
    if (indexWriter == NULL)
        indexWriter = new IndexFileWriter(indexFileName.c_str(), prec);

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
    int vectorId = atoi(portname);  // FIXME check it's numeric at all
    return node1->addVector(vectorId, "n/a", "n/a");
}


