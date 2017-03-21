//=========================================================================
//  SQLITEVECTORREADER.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Tamas Borbely, Zoltan Bojthe
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_SCAVE_SQLITEVECTORREADER_H
#define __OMNETPP_SCAVE_SQLITEVECTORREADER_H

#include <map>
#include <string>
#include "node.h"
#include "nodetype.h"
#include "commonnodes.h"
#include "common/sqlite3.h"


namespace omnetpp {
namespace scave {

#define SQLITEVECFILEREADER_BUFSIZE  (64*1024)

/**
 * Producer node which reads an output vector file.
 */
class SCAVE_API SqliteVectorReaderNode : public ReaderNode
{
    typedef std::vector<Port> PortVector;

    struct PortData {
        sqlite_int64 currentRowId;
        PortVector ports;

        PortData() : currentRowId(-1) {}
    };

    typedef std::map<int,PortData> PortMap;

    protected:
        PortMap ports;
        sqlite3 *db;
        sqlite3_stmt *get_vector_data_stmt;
        std::string filename;
        bool allowIndexing;
        size_t bufferSize;
        bool fFinished;

    protected:
        bool readNextBlock(int vectorId, PortData& portData);
        void checkOK(int sqlite3_result);

    public:
        SqliteVectorReaderNode(const char *filename, bool allowIndexing, size_t bufferSize = SQLITEVECFILEREADER_BUFSIZE);
        virtual ~SqliteVectorReaderNode();

        Port *addVector(const VectorResult& vector);
        Port *addVector(int vectorId);

        virtual bool isReady() const override;
        virtual void process() override;
        virtual bool isFinished() const override;

        virtual int64_t getFileSize() override;
        virtual int64_t getNumReadBytes() override;
};


class SCAVE_API SqliteVectorReaderNodeType : public ReaderNodeType
{
    public:
        virtual const char *getName() const override {return "sqlitevectorreader";}
        virtual const char *getDescription() const override;
        virtual void getAttributes(StringMap& attrs) const override;
        virtual Node *create(DataflowManager *mgr, StringMap& attrs) const override;
        virtual Port *getPort(Node *node, const char *portname) const override;
};


} // namespace scave
}  // namespace omnetpp


#endif
