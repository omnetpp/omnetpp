//=========================================================================
//  VECTORFILEREADER.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef _VECTORFILEREADER_H_
#define _VECTORFILEREADER_H_

#include <map>
#include <string>
#include "node.h"
#include "nodetype.h"
#include "commonnodes.h"
#include "filereader.h"
#include "linetokenizer.h"
#include "resultfilemanager.h"

NAMESPACE_BEGIN

// read in 64K chunks (apparently it doesn't matter much if we use a bigger buffer)
#define VECFILEREADER_BUFSIZE  (64*1024)

// shared with indexedvectorfilereadernode
Datum parseColumns(char **tokens, int numtokens, const std::string &columns, const char* file, int lineno, long offset);

/**
 * Producer node which reads an output vector file.
 */
class SCAVE_API VectorFileReaderNode : public ReaderNode
{
    public:
        typedef std::vector<Port> PortVector;
        typedef std::map<int,PortVector> Portmap;
        typedef std::string ColumnSpec;
        typedef std::map<int,ColumnSpec> ColumnMap;
    private:
        Portmap ports;
        ColumnMap columns;
        LineTokenizer tokenizer;
        bool fFinished;

    public:
        VectorFileReaderNode(const char *filename, size_t bufferSize = VECFILEREADER_BUFSIZE);
        virtual ~VectorFileReaderNode();

        Port *addVector(const VectorResult &vector);

        virtual bool isReady() const;
        virtual void process();
        virtual bool finished() const;
};


class SCAVE_API VectorFileReaderNodeType : public ReaderNodeType
{
    public:
        virtual const char *name() const {return "vectorfilereader";}
        virtual const char *description() const;
        virtual void getAttributes(StringMap& attrs) const;
        virtual Node *create(DataflowManager *mgr, StringMap& attrs) const;
        virtual Port *getPort(Node *node, const char *portname) const;
};


NAMESPACE_END


#endif


