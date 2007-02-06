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
#include "scaveutils.h"
#include "vectorfilereader.h"


VectorFileReaderNode::VectorFileReaderNode(const char *fileName, size_t bufferSize) :
  reader(fileName, bufferSize), tokenizer(), fFinished(false)
{
}

VectorFileReaderNode::~VectorFileReaderNode()
{
}

Port *VectorFileReaderNode::addVector(const VectorResult &vector)
{
    PortVector& portvec = ports[vector.vectorId];
    portvec.push_back(Port(this));
    Port& port = portvec.back();
    columns[vector.vectorId] = vector.columns;
    return &port;
}

bool VectorFileReaderNode::isReady() const
{
    return true;
}

/**
 * Parses columns of one line in the vector file.
 */
static Datum parseColumns(char **tokens, int numtokens, const VectorFileReaderNode::ColumnSpec &columns, int lineno)
{
    Datum a;
    int colno = columns.size();

    if (colno > numtokens - 1)
        throw opp_runtime_error("invalid vector file syntax: missing columns, line %d", lineno);
    if (numtokens - 1 > colno)
        throw opp_runtime_error("invalid vector file syntax: extra columns, line %d", lineno);

    // optimization:
    //   first process the two most common case, then the general case

    if (colno == 2 && columns[0] == 'T' && columns[1] == 'V')
    {
        // parse time and value
        if (!parseDouble(tokens[1],a.x) || !parseDouble(tokens[2],a.y))
            throw opp_runtime_error("invalid vector file syntax: invalid time or value column, line %d", lineno);
        a.eventNumber = -1;
    }
    else if (colno == 3 && columns[0] == 'E' && columns[1] == 'T' && columns[2] == 'V')
    {
        // parse event number, time and value
        if (!parseLong(tokens[1], a.eventNumber) || !parseDouble(tokens[2],a.x) || !parseDouble(tokens[3],a.y))
            throw opp_runtime_error("invalid vector file syntax: invalid event number, time or value column, line %d", lineno);
    }
    else // interpret general case
    {
        a.eventNumber = -1;
        for (int i = 0; i < (int)columns.size(); ++i)
        {
            switch (columns[i])
            {
            case 'E':
                if (!parseLong(tokens[i+1], a.eventNumber))
                    throw opp_runtime_error("invalid vector file syntax: invalid event number, line %d", lineno);
                break;
            case 'T':
                if (!parseDouble(tokens[i+1], a.x))
                    throw opp_runtime_error("invalid vector file syntax: invalid time, line %d", lineno);
                break;
            case 'V':
                if (!parseDouble(tokens[i+1], a.y))
                    throw opp_runtime_error("invalid vector file syntax: invalid value, line %d", lineno);
                break;
            default:
                throw opp_runtime_error("invalid vector file syntax: unknown column type: '%c', line %d", columns[i], lineno);
            }
        }
    }

    return a;
}

void VectorFileReaderNode::process()
{
    char *line;
    for (int k=0; k<1000 && (line=reader.getNextLineBufferPointer())!=NULL; k++)
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
                ColumnMap::iterator columnSpec = columns.find(vectorId);
                assert(columnSpec != columns.end());

                // parse columns
                Datum a = parseColumns(vec, numtokens, columnSpec->second, (int)reader.getNumReadLines());

                // write to port(s)
                for (PortVector::iterator p=portvec->second.begin(); p!=portvec->second.end(); ++p)
                    p->channel()->write(&a,1);

                //DBG(("vectorfilereader: written id=%d (%ld,%lg,%lg)\n", vectorId, a.eventNumber, a.x, a.y));
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
    VectorResult vector;
    vector.vectorId = atoi(portname);  // FIXME check it's numeric at all
    vector.columns = "TV";             // support old vector file format only
    return node1->addVector(vector);
}

