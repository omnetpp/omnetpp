//=========================================================================
//  INDEXEDVECTORFILEREADER2.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Tamas Borbely
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef _INDEXEDVECTORFILEREADER2_H_
#define _INDEXEDVECTORFILEREADER2_H_

#include <map>
#include <string>
#include "node.h"
#include "nodetype.h"
#include "commonnodes.h"
#include "filereader.h"
#include "linetokenizer.h"
#include "indexfile.h"
#include "resultfilemanager.h"

NAMESPACE_BEGIN

// read in 64K chunks (apparently it doesn't matter much if we use a bigger buffer)
#define VECFILEREADER_BUFSIZE  (64*1024)


/**
 * Producer node which reads an output vector file.
 */
class SCAVE_API IndexedVectorFileReaderNode2 : public ReaderNode
{
    typedef std::vector<Port> PortVector;

    struct PortData {
        VectorData *vector;
        int currentBlockIndex;
        PortVector ports;

        PortData() : vector(NULL), currentBlockIndex(0) {}
    };

    typedef std::map<int,PortData> PortMap;

    private:
        PortMap ports;
        VectorFileIndex *index;
        LineTokenizer tokenizer;
        bool fFinished;

    public:
        IndexedVectorFileReaderNode2(const char *filename, size_t bufferSize = VECFILEREADER_BUFSIZE);
        virtual ~IndexedVectorFileReaderNode2();

        Port *addVector(const VectorResult &vector);

        virtual bool isReady() const;
        virtual void process();
        virtual bool isFinished() const;

    private:
        void readIndexFile();
        bool readNextBlock(PortData& portData);
};


class SCAVE_API IndexedVectorFileReaderNode2Type : public ReaderNodeType
{
    public:
        virtual const char *getName() const {return "indexedvectorfilereader2";}
        virtual const char *getDescription() const;
        virtual void getAttributes(StringMap& attrs) const;
        virtual Node *create(DataflowManager *mgr, StringMap& attrs) const;
        virtual Port *getPort(Node *node, const char *portname) const;
};


NAMESPACE_END


#endif


