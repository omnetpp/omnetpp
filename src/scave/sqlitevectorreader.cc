//=========================================================================
//  SQLITEVECTORREADER.CC - part of
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

#include "common/opp_ctype.h"
#include "omnetpp/platdep/platmisc.h"
#include "channel.h"
#include "scaveutils.h"
#include "vectorfilereader.h"
#include "sqlitevectorreader.h"

using namespace std;
using namespace omnetpp::common;

namespace omnetpp {
namespace scave {

#define LL    INT64_PRINTF_FORMAT

SqliteVectorReaderNode::SqliteVectorReaderNode(const char *filename, size_t bufferSize) :
    ReaderNode(),
    db(nullptr),
    get_vector_data_stmt(nullptr),
    filename(filename),
    bufferSize(bufferSize/16),          // bytes --> items
    fFinished(false)
{
}

SqliteVectorReaderNode::~SqliteVectorReaderNode()
{
    if(db) {
        sqlite3_finalize(get_vector_data_stmt);
        sqlite3_close(db);
    }
}

Port *SqliteVectorReaderNode::addVector(const VectorResult& vector)
{
    PortData& portData = ports[vector.vectorId];
    portData.ports.push_back(Port(this));
    Port& port = portData.ports.back();
    return &port;
}

bool SqliteVectorReaderNode::isReady() const
{
    return true;
}

void SqliteVectorReaderNode::process()
{
    if(!db) {
        if (sqlite3_open_v2(filename.c_str(), &db, SQLITE_OPEN_READONLY, 0) != SQLITE_OK)
            throw opp_runtime_error("Can't open sqlite database '%s': %s\n", filename.c_str(), sqlite3_errmsg(db));
        if (sqlite3_prepare_v2(db,
                "select vectordata.rowid, eventNumber, simtimeRaw, simtimeExp, value "
                "from vectordata "
                "inner join vector using (vectorId) "
                "inner join run using (runId) "
                "where vectorId = ? and vectordata.rowid > ? order by vectordata.rowid;",
                -1, &get_vector_data_stmt, nullptr) != SQLITE_OK) {
            throw opp_runtime_error("At %s:%d database error: %s\n", __FILE__, __LINE__, sqlite3_errmsg(db));
        }
    }

    // read one block from each vector
    bool found = false;
    for (PortMap::iterator it = ports.begin(); it != ports.end(); ++it) {
        PortData& portData = it->second;
        found |= readNextBlock(it->first, portData);
    }

    if (!found)
        fFinished = true;
}

bool SqliteVectorReaderNode::isFinished() const
{
    return fFinished;
}

int64_t SqliteVectorReaderNode::getFileSize()
{
    //FIXME implement
    return 1;
}

int64_t SqliteVectorReaderNode::getNumReadBytes()
{
    //FIXME implement
    return 0;
}

#define CHECK(cond, msg) {if (!(cond)) throw opp_runtime_error(msg ", file %s, offset %" LL "d", file, (int64_t)offset); }

bool SqliteVectorReaderNode::readNextBlock(int vectorId, PortData& portData)
{
    bool retval = true;
    sqlite3_reset(get_vector_data_stmt);
    sqlite3_bind_int64(get_vector_data_stmt, 1, vectorId);
    sqlite3_bind_int64(get_vector_data_stmt, 2, portData.currentRowId);

    for (size_t i=0; i < bufferSize; i++) {
        int s = sqlite3_step(get_vector_data_stmt);
        if (s == SQLITE_ROW) {
            Datum a;
            sqlite_int64 rowId = sqlite3_column_int64(get_vector_data_stmt, 0);
            a.eventNumber = sqlite3_column_int64(get_vector_data_stmt, 1);
            sqlite_int64 simtimeRaw = sqlite3_column_int64(get_vector_data_stmt, 2);
            int simtimeExp = sqlite3_column_int(get_vector_data_stmt, 3);
            a.xp = BigDecimal(simtimeRaw, simtimeExp);
            a.x = a.xp.dbl();
            a.y = sqlite3_column_double(get_vector_data_stmt, 4);
            portData.currentRowId  = rowId;

            //std::cout << "BZ: read vectordata id=" << vectorId << " rowid=" << rowId << " #" << a.eventNumber << " t=" << a.xp << " v=" << a.y << std::endl;
            // write to port(s)
            for (PortVector::const_iterator p = portData.ports.begin(); p != portData.ports.end(); ++p)
                p->getChannel()->write(&a, 1);
        }
        else if (s == SQLITE_DONE) {
            retval = false;
            break;
        }
        else {
            throw opp_runtime_error("At %s:%d database error: %s\n", __FILE__, __LINE__, sqlite3_errmsg(db));
        }
    }

    sqlite3_clear_bindings(get_vector_data_stmt);

    return retval;
}

//-----

const char *SqliteVectorReaderNodeType::getDescription() const
{
    return "Reads sqlite output vector files.";
}

void SqliteVectorReaderNodeType::getAttributes(StringMap& attrs) const
{
    attrs["filename"] = "name of the output vector file (.vec)";
}

Node *SqliteVectorReaderNodeType::create(DataflowManager *mgr, StringMap& attrs) const
{
    checkAttrNames(attrs);

    const char *fname = attrs["filename"].c_str();

    Node *node = new SqliteVectorReaderNode(fname);
    node->setNodeType(this);
    mgr->addNode(node);
    return node;
}

Port *SqliteVectorReaderNodeType::getPort(Node *node, const char *portname) const
{
    // vector id is used as port name
    SqliteVectorReaderNode *node1 = dynamic_cast<SqliteVectorReaderNode *>(node);
    if (node1 == nullptr)
        throw opp_runtime_error("node type should be 'SqliteVectorReaderNode'");
    VectorResult vector;
    if (!parseInt(portname, vector.vectorId))
        throw opp_runtime_error("indexed file reader node: port should be a vector id, received: %s", portname);
    return node1->addVector(vector);
}

}  // namespace scave
}  // namespace omnetpp
