//=========================================================================
//  INDEXEDVECTORFILEWRITER.CC - part of
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
#include "ivectorfilewriter.h"

#define CHECK(printf) if (printf<0) throw new Exception("Cannot write vector file '%s'", fileName.c_str());

#ifndef min
#define min(a,b)     ( (a)<(b) ? (a) : (b) )
#define max(a,b)     ( (a)>(b) ? (a) : (b) )
#endif

static FILE *openFile(const std::string fileName)
{
    FILE *f = fopen(fileName.c_str(),"w");
    if (f==NULL)
        throw new Exception("Cannot open index file `%s'", fileName.c_str());
    return f;
}

IndexedVectorFileWriterNode::IndexedVectorFileWriterNode(const char *fileName, const char *indexFileName, int blockSize, const char *fileHeader)
{
    f = NULL;
    fi = NULL;
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
    if (fi != NULL)
        fclose(fi);
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
        CHECK(fprintf(f,"%s\n\n", fileHeader.c_str()));
        for (PortVector::iterator it=ports.begin(); it!=ports.end(); it++) {
            VectorInputPort *port=*it;
            //FIXME use opp_quotestr_ifneeded() here
            CHECK(fprintf(f,"vector %ld  \"%s\"  \"%s\"  %d\n", port->id, port->moduleName.c_str(), port->name.c_str(), 1));
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
        int count = sprintf(port->bufferPtr, "%ld\t%.*g\t%.*g\n", port->id, prec, a.x, prec, a.y);
        if (count > 0)
        {
            port->bufferPtr+=count;
            port->bufferNumOfRecords++;
            port->numOfRecords++;
            port->min = min(port->min, a.y);
            port->max = max(port->max, a.y);
            port->sum += a.y;
            port->sumSqr += a.y*a.y;
        }
        else
            throw new Exception("Cannot write data to output buffer.");
    }
}

void IndexedVectorFileWriterNode::writeBufferToFile(VectorInputPort *port)
{
    assert(f!=NULL);
    long offset = ftell(f);
    CHECK(fputs(port->buffer, f));
    port->blocks.push_back(Block(offset, port->bufferNumOfRecords));
    port->clearBuffer();
}

#define CHECK_I(printf) if (printf<0) throw new Exception("Cannot write index file '%s'", indexFileName.c_str());

void IndexedVectorFileWriterNode::writeIndex(VectorInputPort *port)
{
    if (!fi)
    {
        fi = openFile(indexFileName);
    }

    int nBlocks = port->blocks.size();
    if (nBlocks > 0)
    {
        CHECK_I(fprintf(fi,"vector %ld  \"%s\"  \"%s\"  %d  %d  %d  %.*g  %.*g  %.*g  %.*g\n",
                      port->id, port->moduleName.c_str(), port->name.c_str(), 1/*tuple*/, port->bufferSize,
                      port->numOfRecords, prec, port->min, prec, port->max, prec, port->sum, prec, port->sumSqr));
        for (int i=0; i<nBlocks; i+=10)
        {
            CHECK_I(fprintf(fi, "%ld\t", port->id));
            for (int j = 0; j<10 && i+j < nBlocks; ++j)
            {
                Block &block=port->blocks[i+j];
                CHECK_I(fprintf(fi, "%ld:%ld ", block.offset, block.numOfRecords));
            }
            CHECK_I(fprintf(fi, "\n"));
        }
        port->blocks.clear();
    }
}


//-------

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

    Node *node = new IndexedVectorFileWriterNode(fileName, indexFileName, blockSize);
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

