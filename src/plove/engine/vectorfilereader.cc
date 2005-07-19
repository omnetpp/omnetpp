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
#include "vectorfilereader.h"


VectorFileReaderNode::VectorFileReaderNode(const char *fileName, size_t bufferSize) :
  ftok(fileName, bufferSize)

{
}

VectorFileReaderNode::~VectorFileReaderNode()
{
}

Port *VectorFileReaderNode::addVector(int vectorId)
{
    PortVector& portvec = ports[vectorId];
    portvec.push_back(Port(this));
    Port& port = portvec.back();
    return &port;
}

bool VectorFileReaderNode::isReady() const
{
    return true;
}

static double zero =0;

static bool parseDouble(char *s, double& dest)
{
    char *e;
    dest = strtod(s,&e);
    if (!*e)
    {
        return true;
    }
    if (*e=='#' && *(e+1)=='I' && *(e+2)=='N' && *(e+3)=='F')
    {
        dest = dest * 1/zero;  // +INF or -INF
        return true;
    }
    return false;
}

void VectorFileReaderNode::process()
{
    FileTokenizer::CharPVector vec;
    while (ftok.getLine(vec))
    {
        if (vec.size()>=3 && isdigit(vec[0][0]))  // silently ignore incomplete lines
        {
            // extract vector id
            char *e;
            int vectorId = (int) strtol(vec[0],&e,10);
            if (*e)
                throw new Exception("invalid vector file syntax: invalid vector id column, line %d", ftok.lineNum());

            Portmap::iterator portvec = ports.find(vectorId);
            if (portvec!=ports.end())
            {
                // parse time and value
                double time, value;
                if (!parseDouble(vec[1],time) || !parseDouble(vec[2],value))
                    throw new Exception("invalid vector file syntax: invalid time or value column, line %d", ftok.lineNum());

                // write to port(s)
                Datum a;
                a.x = time;
                a.y = value;
                for (PortVector::iterator p=portvec->second.begin(); p!=portvec->second.end(); ++p)
                    p->channel()->write(&a,1);

                //DBG(("vectorfilereader: written id=%d (%lg,%lg)\n", vectorId, time, value));
            }
        }
    }

    if (!ftok.eof())
        throw new Exception(ftok.errorMsg().c_str());
}

bool VectorFileReaderNode::finished() const
{
    return ftok.eof();
}

//-----

const char *VectorFileReaderNodeType::description() const
{
    return "Reads output vector files.";
}

void VectorFileReaderNodeType::getAttributes(StringMap& attrs) const
{
    attrs["filename"] = "name of the output vector file (.vec)";
}

Node *VectorFileReaderNodeType::create(DataflowManager *, StringMap& attrs) const
{
    checkAttrNames(attrs);

    const char *fname = attrs["filename"].c_str();

    Node *node = new VectorFileReaderNode(fname);
    node->setNodeType(this);
    return node;
}

Port *VectorFileReaderNodeType::getPort(Node *node, const char *portname) const
{
    // vector id is used as port name
    VectorFileReaderNode *node1 = dynamic_cast<VectorFileReaderNode *>(node);
    int vectorId = atoi(portname);  // FIXME check it's numeric at all
    return node1->addVector(vectorId);
}

