//=========================================================================
//  VECTORFILEREADER.H - part of
//                          OMNeT++
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2003 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef _VECTORFILEREADER_H_
#define _VECTORFILEREADER_H_

#include <map>
#include <string>
#include "node.h"
#include "nodetype.h"

// read in 64K chunks (apparently it doesn't matter much if we use a bigger buffer)
#define VECFILEREADER_BUFSIZE  (64*1024)


/**
 * Producer node which reads an output vector file.
 */
class VectorFileReaderNode : public Node
{
    public:
        typedef std::vector<Port> PortVector;
        typedef std::map<int,PortVector> Portmap;
    private:
        Portmap ports;
        std::string fname;
        FILE *f;
        bool eofreached;
        size_t buffersize;
        char *buffer;
        int bufferused;

    public:
        VectorFileReaderNode(const char *filename, size_t bufferSize = VECFILEREADER_BUFSIZE);
        virtual ~VectorFileReaderNode();

        Port *addVector(int vectorId);

        virtual bool isReady() const;
        virtual void process();
        virtual bool finished() const;
};


class VectorFileReaderNodeType : public NodeType
{
    public:
        virtual const char *name() const {return "vectorfilereader";}
        virtual const char *category() const {return "multi-port source";}
        virtual const char *description() const;
        virtual bool isHidden() const {return true;}
        virtual void getAttributes(StringMap& attrs) const;
        virtual Node *create(DataflowManager *, StringMap& attrs) const;
        virtual Port *getPort(Node *node, const char *portname) const;
};


#endif


