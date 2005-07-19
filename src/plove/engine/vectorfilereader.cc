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


VectorFileReaderNode::VectorFileReaderNode(const char *fileName, size_t bufferSize)
{
    fname = fileName;
    buffersize = bufferSize;
    f = NULL;
    linenum = 0;
    eofreached = false;
    buffer = new char[buffersize+100];  // +1 for EOS, +100 for MSVC hack (see later)
    bufferused = 0;
}

VectorFileReaderNode::~VectorFileReaderNode()
{
    delete [] buffer;
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

static bool parseDouble(char *&s, double& dest)
{
    char *e;
    dest = strtod(s,&e);
    if (s==e)
    {
        return false;
    }
    if (*e && *e!=' ' && *e!='\t')
    {
        if (*e=='#' && *(e+1)=='I' && *(e+2)=='N' && *(e+3)=='F')
        {
            dest = dest * 1/zero;  // +INF or -INF
            e+=4;
            if (*e && *e!=' ' && *e!='\t')
                return false;
        }
        else
        {
            return false;
        }
    }
    s = e;
    return true;
}

void VectorFileReaderNode::process()
{
    // open file if needed
    if (!f)
    {
        f = fopen(fname.c_str(), "r");
        if (!f)
            throw new Exception("cannot open `%s' for read", fname.c_str());
    }

    // read enough bytes to fill the buffer
    int bytesread = fread(buffer+bufferused, 1, buffersize-bufferused, f);
    if (feof(f))
        eofreached = true;
    if (ferror(f))
        throw new Exception("error reading from `%s'", fname.c_str());
    buffer[bufferused+bytesread] = '\0';
    const char *endbuffer = buffer+bufferused+bytesread;

    char *line = buffer;

    // process all full lines
    while (1)
    {
        // do we have a full line?
        char *s = line;
        while (*s && *s!='\r' && *s!='\n') s++;
        char *endline = s;
        if (!*s && !eofreached)  // if at eof, we have to process unterminated last line, too
            break;

        linenum++;

        s = line;

        // skip leading white space
        while (*s==' ' || *s=='\t') s++;

        if (!*s && eofreached)
        {
            // end of file, no more work
            break;
        }
        else if (!*s || *s=='\r' || *s=='\n')
        {
            // blank line, ignore
        }
        else if (!isdigit(*s))
        {
            // ignore "vector..." and comment ("#...") lines
            while (*s && *s!='\r' && *s!='\n') s++;
        }
        else
        {
            // parse line
            char old = *endline;
            *endline = 0;

            // extract vector id
            char *e;
            int vectorId = (int) strtol(s,&e,10);
            if (s==e)
                throw new Exception("invalid vector file syntax: invalid vector id column, line %d", linenum);
            s = e;

            Portmap::iterator portvec = ports.find(vectorId);
            if (portvec==ports.end())
            {
                // skip rest of irrelevant line
                while (*s && *s!='\r' && *s!='\n') s++;
            }
            else
            {
                // parse time and value
                double time, value;
                if (!parseDouble(s,time) || !parseDouble(s,value))
                    throw new Exception("invalid vector file syntax: invalid time or value column, line %d", linenum);

                // skip trailing white space
                while (*s==' ' || *s=='\t') s++;

                // write to port(s)
                Datum a;
                a.x = time;
                a.y = value;
                for (PortVector::iterator p=portvec->second.begin(); p!=portvec->second.end(); ++p)
                    p->channel()->write(&a,1);

                //DBG(("vectorfilereader: written id=%d (%lg,%lg)\n", vectorId, time, value));
            }

            *endline = old;
        }

        // skip line termination
        if (*s && *s!='\r' && *s!='\n')
            throw new Exception("invalid vector file syntax: garbage at end of line ('%c'), line %d", *s, linenum);
        while (*s=='\r' || *s=='\n') s++;
        line = s;
    }

    // copy the last (partial) line to beginning of buffer
    bufferused = endbuffer-line;
    memcpy(buffer, line, bufferused);

    return;
}

bool VectorFileReaderNode::finished() const
{
    return eofreached;
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

