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

#include <sys/stat.h>
#include "channel.h"
#include "ivectorfilewriter.h"

#define CHECK(printf,filename) if (printf<0) throw new Exception("Cannot write file '%s'", filename.c_str());

static std::string createIndexFileName(const std::string fileName)
{
    std::string indexFileName = fileName;
    std::string::size_type index = indexFileName.rfind('.');
    if (index != std::string::npos)
        indexFileName.replace(index, std::string::npos, ".vci");
    else
        indexFileName.append(".vci");
    return indexFileName;
}

static inline bool existsFile(const std::string fileName)
{
    struct stat s;
    return stat(fileName.c_str(), &s)==0;
}

static FILE *openTempFile(const std::string baseFileName, std::string &tmpFileName/*out*/)
{
    std::string prefix = baseFileName;
    prefix.append(".temp");
    tmpFileName = prefix;
    int serial = 0;
    char buffer[11];
    while (existsFile(tmpFileName)) 
        tmpFileName = prefix+itoa(serial++, buffer, 10);
    
    FILE *f = fopen(tmpFileName.c_str(), "w");
    if (!f)
        throw new Exception("Cannot open '%s' for writing.", tmpFileName.c_str());

    return f;
}

static FILE *openFile(const std::string fileName)
{
    FILE *f = fopen(fileName.c_str(),"a");
    if (f==NULL)
        throw new Exception("Cannot open index file `%s'", fileName.c_str());
    return f;
}

static void renameFile(const std::string oldName, const std::string newName, const char *desc)
{
    int rc = rename(oldName.c_str(), newName.c_str());
    if (rc)
        throw new Exception("Cannot rename %s from '%s' to '%s': %s", desc, oldName.c_str(), newName.c_str(), strerror(errno));
}

static void removeFile(const std::string fileName, const char *descr)
{
    if (unlink(fileName.c_str())!=0 && errno!=ENOENT)
        throw new Exception("Cannot remove %s `%s': %s", descr, fileName.c_str(), strerror(errno));
}


IndexedVectorFileWriterNode::IndexedVectorFileWriterNode(const char *fileName, int blockSize, const char *fileHeader)
{
    f = NULL;
    fi = NULL;
    this->prec = DEFAULT_PRECISION;
    this->fileHeader = (fileHeader ? fileHeader : "");
    this->fileName = fileName;
    this->indexFileName = createIndexFileName(fileName);
    this->tmpFileName = tmpnam(NULL);
    this->tmpIndexFileName = tmpnam(NULL);
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

    removeFile(fileName, "old input file");
    renameFile(tmpFileName, fileName, "output file");
    removeFile(indexFileName, "old index file");
    renameFile(tmpIndexFileName, indexFileName, "index file");
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
        f = openTempFile(fileName, tmpFileName);
        // print file header and vector declarations
        CHECK(fprintf(f,"%s\n\n", fileHeader.c_str()), tmpFileName);
        for (PortVector::iterator it=ports.begin(); it!=ports.end(); it++) {
            VectorInputPort *port=*it;
            CHECK(fprintf(f,"vector %ld  \"%s\"  \"%s\"  %d\n", port->id, port->moduleName.c_str(), port->name.c_str(), 1), tmpFileName);
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
            ++port->numOfRecords;
        }
        else
            throw new Exception("Cannot write data to output buffer.");
    }
}

void IndexedVectorFileWriterNode::writeBufferToFile(VectorInputPort *port)
{
    assert(f!=NULL);
    long offset = ftell(f);
    CHECK(fputs(port->buffer, f), tmpFileName);
    port->blocks.push_back(Block(offset, port->numOfRecords));
    port->clearBuffer();
}

void IndexedVectorFileWriterNode::writeIndex(VectorInputPort *port)
{
    if (!fi)
    {
        fi = openTempFile(indexFileName, tmpIndexFileName);
    }

    int nBlocks = port->blocks.size();
    if (nBlocks > 0)
    {
        CHECK(fprintf(fi,"vector %ld  \"%s\"  \"%s\"  %d  %d\n",
                      port->id, port->moduleName.c_str(), port->name.c_str(), 1/*tuple*/, port->bufferSize), tmpIndexFileName);
        for (int i=0; i<nBlocks; i+=10)
        {
            CHECK(fprintf(fi, "%ld\t", port->id), tmpIndexFileName);
            for (int j = 0; j<10 && i+j < nBlocks; ++j)
            {
                Block &block=port->blocks[i+j];
                CHECK(fprintf(fi, "%ld:%ld ", block.offset, block.numOfRecords), tmpIndexFileName);
            }
            CHECK(fprintf(fi, "\n"), tmpIndexFileName);
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
    attrs["blocksize"] = "size of the blocks of each vector";
    attrs["fileheader"] = "header written into the output vector file";
}

Node *IndexedVectorFileWriterNodeType::create(DataflowManager *mgr, StringMap& attrs) const
{
    checkAttrNames(attrs);

    const char *fileName = attrs["filename"].c_str();
    int blockSize = atoi(attrs["blocksize"].c_str());

    Node *node = new IndexedVectorFileWriterNode(fileName, blockSize);
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

