//=========================================================================
//  VECTORFILEWRITER.CC - part of
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
#include "vectorfilewriter.h"
#include "stringutil.h"


#ifdef CHECK
#undef CHECK
#endif
#define CHECK(fprintf)    if (fprintf<0) throw Exception("Cannot write output vector file `%s'", fileName.c_str())

VectorFileWriterNode::VectorFileWriterNode(const char *fileName, const char *fileHeader)
{
    f = NULL;
    this->prec = DEFAULT_PRECISION;
    this->fileName = fileName;
    this->fileHeader = (fileHeader ? fileHeader : "");
}

VectorFileWriterNode::~VectorFileWriterNode()
{
}

Port *VectorFileWriterNode::addVector(int vectorId, const char *moduleName, const char *name)
{
    ports.push_back(Pair(vectorId, moduleName, name, this));
    return &(ports.back().port);
}

bool VectorFileWriterNode::isReady() const
{
    for (PortVector::const_iterator it=ports.begin(); it!=ports.end(); it++)
        if (it->port()->length()>0)
            return true;
    return false;
}

void VectorFileWriterNode::process()
{
    // open file if needed
    if (!f)
    {
        f = fopen(fileName.c_str(), "w");
        if (!f)
            throw Exception("cannot open `%s' for write", fileName.c_str());

        // print file header and vector declarations
        CHECK(fprintf(f,"%s\n\n", fileHeader.c_str()));
        for (PortVector::iterator it=ports.begin(); it!=ports.end(); it++)
            CHECK(fprintf(f, "vector %ld  %s  %s  %d\n", it->id,
                             QUOTE(it->moduleName.c_str()),
                             QUOTE(it->name.c_str()), 1));
    }

    for (PortVector::iterator it=ports.begin(); it!=ports.end(); it++)
    {
        Channel *chan = it->port();
        int n = chan->length();
        for (int i=0; i<n; i++)
        {
            Datum a;
            chan->read(&a,1);
            CHECK(fprintf(f,"%ld\t%.*g\t%.*g\n", it->id, prec, a.x, prec, a.y));
        }
    }

}

bool VectorFileWriterNode::finished() const
{
    for (PortVector::const_iterator it=ports.begin(); it!=ports.end(); it++)
        if (!it->port()->closing() || it->port()->length()>0)
            return false;
    return true;
}

//-------

const char *VectorFileWriterNodeType::description() const
{
    return "Writes the output (several streams) into an output vector file.";
}

void VectorFileWriterNodeType::getAttributes(StringMap& attrs) const
{
    attrs["filename"] = "name of the output vector file (.vec)";
}

Node *VectorFileWriterNodeType::create(DataflowManager *mgr, StringMap& attrs) const
{
    checkAttrNames(attrs);

    const char *fileName = attrs["filename"].c_str();

    Node *node = new VectorFileWriterNode(fileName);
    node->setNodeType(this);
    mgr->addNode(node);
    return node;
}

Port *VectorFileWriterNodeType::getPort(Node *node, const char *portname) const
{
    // vector id is used as port name
    VectorFileWriterNode *node1 = dynamic_cast<VectorFileWriterNode *>(node);
    int vectorId = atoi(portname);  // FIXME check it's numeric at all
    return node1->addVector(vectorId, "n/a", "n/a");
}

