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

#include "opp_ctype.h"
#include "channel.h"
#include "scaveutils.h"
#include "scaveexception.h"
#include "vectorfilereader.h"

NAMESPACE_BEGIN

using namespace std;


VectorFileReaderNode::VectorFileReaderNode(const char *fileName, size_t bufferSize) :
  ReaderNode(fileName, bufferSize), fFinished(false)
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
    return &port;
}

bool VectorFileReaderNode::isReady() const
{
    return true;
}

/**
 * Parses columns of one line in the vector file.
 */
Datum parseColumns(char **tokens, int numtokens, const string &columns, const char *file, int lineno, long offset)
{
    Datum a;
    int colno = columns.size();

    if (colno > numtokens - 1)
        throw ResultFileFormatException("invalid vector file syntax: missing columns", file, lineno, offset);
    if (numtokens - 1 > colno)
        throw ResultFileFormatException("invalid vector file syntax: extra columns", file, lineno, offset);

    // optimization:
    //   first process the two most common case, then the general case

    if (colno == 2 && columns[0] == 'T' && columns[1] == 'V')
    {
        // parse time and value
        if (!parseSimtime(tokens[1],a.xp) || !parseDouble(tokens[2],a.y))
            throw ResultFileFormatException("invalid vector file syntax: invalid time or value column", file, lineno, offset);
        a.eventNumber = -1;
        a.x = a.xp.dbl();
    }
    else if (colno == 3 && columns[0] == 'E' && columns[1] == 'T' && columns[2] == 'V')
    {
        // parse event number, time and value
        if (!parseLong(tokens[1], a.eventNumber) || !parseSimtime(tokens[2],a.xp) || !parseDouble(tokens[3],a.y))
            throw ResultFileFormatException("invalid vector file syntax: invalid event number, time or value column", file, lineno, offset);
        a.x = a.xp.dbl();
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
                    throw ResultFileFormatException("invalid vector file syntax: invalid event number", file, lineno, offset);
                break;
            case 'T':
                if (!parseSimtime(tokens[i+1], a.xp))
                    throw ResultFileFormatException("invalid vector file syntax: invalid time", file, lineno, offset);
                a.x = a.xp.dbl();
                break;
            case 'V':
                if (!parseDouble(tokens[i+1], a.y))
                    throw ResultFileFormatException("invalid vector file syntax: invalid value", file, lineno, offset);
                break;
            default:
                throw ResultFileFormatException("invalid vector file syntax: unknown column type", file, lineno, offset);
            }
        }
    }

    return a;
}

void VectorFileReaderNode::process()
{
    const char *file = filename.c_str();
    char *line;
    for (int k=0; k<1000 && (line=reader.getNextLineBufferPointer())!=NULL; k++)
    {
        int lineNo = (int)reader.getNumReadLines();
        int length = reader.getCurrentLineLength();
        tokenizer.tokenize(line, length);

        int numtokens = tokenizer.numTokens();
        char **vec = tokenizer.tokens();

        if (vec[0][0] == 'v' && strcmp(vec[0], "vector") == 0)
        {
            if (numtokens < 4)
                throw ResultFileFormatException("vector file reader: broken vector declaration", file, lineNo);

            int vectorId;
            if (!parseInt(vec[1], vectorId))
                throw ResultFileFormatException("vector file reader: malformed vector in vector declaration", file, lineNo);
            if (ports.find(vectorId) != ports.end())
                columns[vectorId] = (numtokens < 5 || opp_isdigit(vec[4][0]) ? "TV" : vec[4]);
        }
        else if (numtokens>=3 && opp_isdigit(vec[0][0]))  // silently ignore incomplete lines
        {
            // extract vector id
            int vectorId;
            if (!parseInt(vec[0], vectorId))
                throw ResultFileFormatException("invalid vector file syntax: invalid vector id column", file, lineNo);

            Portmap::iterator portvec = ports.find(vectorId);
            if (portvec!=ports.end())
            {
                ColumnMap::iterator columnSpec = columns.find(vectorId);
                if (columnSpec == columns.end())
                    throw ResultFileFormatException("vector file reader: missing vector declaration", file, lineNo);

                // parse columns
                Datum a = parseColumns(vec, numtokens, columnSpec->second, file, lineNo, -1);

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
    return node1->addVector(vector);
}

NAMESPACE_END
