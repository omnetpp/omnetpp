//=========================================================================
//  SQLITEVECTORREADER.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Tamas Borbely, Zoltan Bojthe
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_SCAVE_SQLITEVECTORREADER_H
#define __OMNETPP_SCAVE_SQLITEVECTORREADER_H

#include <map>
#include <string>
//#include "common/filereader.h"
#include "common/linetokenizer.h"
#include "node.h"
#include "nodetype.h"
#include "commonnodes.h"
#include "indexfile.h"
#include "resultfilemanager.h"

#include "common/sqlite3.h"


namespace omnetpp {
namespace scave {

// read in 64K chunks (apparently it doesn't matter much if we use a bigger buffer)
#define VECFILEREADER_BUFSIZE  (64*1024)


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

    private:
        PortMap ports;
        sqlite3 *db;
        sqlite3_stmt *get_vector_data_stmt;
        std::string filename;
        size_t bufferSize;
        bool fFinished;

    public:
        SqliteVectorReaderNode(const char *filename, size_t bufferSize = VECFILEREADER_BUFSIZE);
        virtual ~SqliteVectorReaderNode();

        Port *addVector(const VectorResult &vector);

        virtual bool isReady() const override;
        virtual void process() override;
        virtual bool isFinished() const override;

        virtual int64_t getFileSize() override;
        virtual int64_t getNumReadBytes() override;

    private:
        bool readNextBlock(int vectorId, PortData& portData);
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
