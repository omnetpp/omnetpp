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


VectorFileWriterNode::VectorFileWriterNode(const char *fileName)
{
    fname = fileName;
    f = NULL;
}

VectorFileWriterNode::~VectorFileWriterNode()
{
}

Port *VectorFileWriterNode::addVector(int vectorId)
{
    ports.push_back(Pair(vectorId, this));
    return &(ports.back().port);
}

bool VectorFileWriterNode::isReady() const
{
    for (PortVector::const_iterator it=ports.begin(); it!=ports.end(); it++)
        if ((*it).port()->length()>0)
            return true;
    return false;
}

void VectorFileWriterNode::process()
{
    // open file if needed
    if (!f)
    {
        f = fopen(fname.c_str(), "w");
        if (!f)
            throw new Exception("cannot open `%s' for write", fname.c_str());
    }

    for (PortVector::iterator it=ports.begin(); it!=ports.end(); it++)
    {
        Channel *chan = (*it).port();
        int n = chan->length();
        for (int i=0; i<n; i++)
        {
            Datum a;
            chan->read(&a,1);
            fprintf(f,"%d\t%lg\t%lg\n", (*it).id, a.x, a.y);  // FIXME precision!!!
        }
    }

}

bool VectorFileWriterNode::finished() const
{
    for (PortVector::const_iterator it=ports.begin(); it!=ports.end(); it++)
        if (!(*it).port()->closing() || (*it).port()->length()>0)
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

Node *VectorFileWriterNodeType::create(DataflowManager *, StringMap& attrs) const
{
    checkAttrNames(attrs);

    const char *fname = attrs["filename"].c_str();

    Node *node = new VectorFileWriterNode(fname);
    node->setNodeType(this);
    return node;
}

Port *VectorFileWriterNodeType::getPort(Node *node, const char *portname) const
{
    // vector id is used as port name
    VectorFileWriterNode *node1 = dynamic_cast<VectorFileWriterNode *>(node);
    int vectorId = atoi(portname);  // FIXME check it's numeric at all
    return node1->addVector(vectorId);
}

