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
  reader(fileName, bufferSize), tokenizer(), fFinished(false)
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
    if (strstr(s,"INF") || strstr(s, "inf"))
    {
        dest = 1/zero;  // +INF or -INF
        if (*s=='-') dest = -dest;
        return true;
    }
    return false;
}

void VectorFileReaderNode::process()
{
    char *line;
    int length;

    for (int k=0; k<1000 && (line=reader.readNextLine())!=NULL; k++)
    {
        int length = reader.getLastLineLength();
        tokenizer.tokenize(line, length);

        int numtokens = tokenizer.numTokens();
        char **vec = tokenizer.tokens();
        if (numtokens>=3 && isdigit(vec[0][0]))  // silently ignore incomplete lines
        {
            // extract vector id
            char *e;
            int vectorId = (int) strtol(vec[0],&e,10);
            if (*e)
                throw opp_runtime_error("invalid vector file syntax: invalid vector id column, line %d", (int)reader.getNumReadLines());

            Portmap::iterator portvec = ports.find(vectorId);
            if (portvec!=ports.end())
            {
                // parse time and value
                Datum a;
                if (!parseDouble(vec[1],a.x) || !parseDouble(vec[2],a.y))
                    throw opp_runtime_error("invalid vector file syntax: invalid time or value column, line %d", (int)reader.getNumReadLines());

                // write to port(s)
                for (PortVector::iterator p=portvec->second.begin(); p!=portvec->second.end(); ++p)
                    p->channel()->write(&a,1);

                //DBG(("vectorfilereader: written id=%d (%lg,%lg)\n", vectorId, time, value));
            }
        }
    }

    if (line == NULL) {
        fFinished = true;
    }
}

bool VectorFileReaderNode::finished() const
{
    return fFinished;
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

Node *VectorFileReaderNodeType::create(DataflowManager *mgr, StringMap& attrs) const
{
    checkAttrNames(attrs);

    const char *fname = attrs["filename"].c_str();

    Node *node = new VectorFileReaderNode(fname);
    node->setNodeType(this);
    mgr->addNode(node);
    return node;
}

Port *VectorFileReaderNodeType::getPort(Node *node, const char *portname) const
{
    // vector id is used as port name
    VectorFileReaderNode *node1 = dynamic_cast<VectorFileReaderNode *>(node);
    int vectorId = atoi(portname);  // FIXME check it's numeric at all
    return node1->addVector(vectorId);
}

