//=========================================================================
//  VECTORFILEREADER.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_SCAVE_VECTORFILEREADER_H
#define __OMNETPP_SCAVE_VECTORFILEREADER_H

#include <map>
#include <string>
#include "common/filereader.h"
#include "common/linetokenizer.h"
#include "omnetpp/platdep/platmisc.h"
#include "node.h"
#include "nodetype.h"
#include "commonnodes.h"
#include "resultfilemanager.h"

namespace omnetpp {
namespace scave {

// read in 64K chunks (apparently it doesn't matter much if we use a bigger buffer)
#define VECFILEREADER_BUFSIZE  (64*1024)

// shared with indexedvectorfilereadernode
Datum parseColumns(char **tokens, int numtokens, const std::string& columns, const char* file, int64_t lineno, file_offset_t offset);

/**
 * Producer node which reads an output vector file.
 */
class SCAVE_API VectorFileReaderNode : public FileReaderNode
{
    public:
        typedef omnetpp::common::LineTokenizer LineTokenizer;
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

        Port *addVector(const VectorResult& vector);
        Port *addVector(int vectorId);

        virtual bool isReady() const override;
        virtual void process() override;
        virtual bool isFinished() const override;
};


class SCAVE_API VectorFileReaderNodeType : public ReaderNodeType
{
    public:
        virtual const char *getName() const override {return "vectorfilereader";}
        virtual const char *getDescription() const override;
        virtual void getAttributes(StringMap& attrs) const override;
        virtual Node *create(DataflowManager *mgr, StringMap& attrs) const override;
        virtual Port *getPort(Node *node, const char *portname) const override;
};


} // namespace scave
}  // namespace omnetpp


#endif


