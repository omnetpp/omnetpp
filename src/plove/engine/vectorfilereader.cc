//=========================================================================
//  VECTORFILEREADER.CC - part of
//                          OMNeT++
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2003 Andras Varga

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
    eofreached = false;
    buffer = new char[buffersize+100];  // +1 for EOS
    bufferused = 0;
}

VectorFileReaderNode::~VectorFileReaderNode()
{
    delete [] buffer;
}

Port *VectorFileReaderNode::addVector(int vectorId)
{
    PortVector& portvec = ports[vectorId];
    portvec.push_back(Port());
    Port& port = portvec.back();
    port.setNode(this);
    return &port;
}

bool VectorFileReaderNode::isReady() const
{
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
        if (!*s && !eofreached)  // if at eof, we have to process unterminated last line, too
            break;

        s = line;
        if (!*s && eofreached)
        {
            // end of file, no more work
            break;
        }
        else if (!*s || *s=='\r' || *s=='\n')
        {
            // blank line, ignore
        }
        else if (*s=='v' || *s=='#')
        {
            // ignore "vector..." and comment ("#...") lines
            while (*s && *s!='\r' && *s!='\n') s++;
        }
        else
        {
            // parse line
            // extract vector id
            char *e;
            int vectorId = (int) strtol(s,&e,10);
            if (s==e)
                throw new Exception("invalid vector file syntax: invalid vector id column");
            s = e;

            Portmap::iterator portvec = ports.find(vectorId);
            if (portvec==ports.end())
            {
                // skip rest of irrelevant line
                while (*s && *s!='\r' && *s!='\n') s++;
            }
            else
            {
                char old = *(s+50);
                char *olds = s;
                *(s+50) = 0;

                // time
                double time = strtod(s,&e);
                if (s==e)
                    throw new Exception("invalid vector file syntax: invalid time column");
                s = e;

                // value
                double value = strtod(s,&e);
                if (s==e)
                    throw new Exception("invalid vector file syntax: invalid value column");
                s = e;

                *(olds+50) = old;

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
        }

        // skip line termination
        if (*s && *s!='\r' && *s!='\n')
            throw new Exception("invalid vector file syntax: garbage at end of line");
        while (*s=='\r' || *s=='\n') s++;
        line = s;
    }

    if (eofreached)
    {
        // close all ports
        for (Portmap::iterator portvec=ports.begin(); portvec!=ports.end(); portvec++)
            for (PortVector::iterator p=portvec->second.begin(); p!=portvec->second.end(); ++p)
                p->channel()->close();
    }
    else
    {
        // copy the last (partial) line to beginning of buffer
        bufferused = endbuffer-line;
        memcpy(buffer, line, bufferused);
    }
    return;
}

bool VectorFileReaderNode::finished() const
{
    return eofreached;
}

//-----

const char *VectorFileReaderNodeType::description() const
{
    return "Reads output vector files. FIXME more";
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

