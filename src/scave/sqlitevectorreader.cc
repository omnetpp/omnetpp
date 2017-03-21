//=========================================================================
//  SQLITEVECTORREADER.CC - part of
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

#include "common/opp_ctype.h"
#include "omnetpp/platdep/platmisc.h"
#include "channel.h"
#include "scaveutils.h"
#include "resultfilemanager.h"
#include "sqlitevectorreader.h"

using namespace std;
using namespace omnetpp::common;

namespace omnetpp {
namespace scave {

#define DEBUG(arglist)
//#define DEBUG(arglist)  {printf arglist; fflush(stdout);}

SqliteVectorReaderNode::SqliteVectorReaderNode(const char *filename, bool allowIndexing, size_t bufferSize) :
    ReaderNode(),
    db(nullptr),
    get_vector_data_stmt(nullptr),
    filename(filename),
    allowIndexing(allowIndexing),
    bufferSize(bufferSize/16),          // bytes --> items
    fFinished(false)
{
}

SqliteVectorReaderNode::~SqliteVectorReaderNode()
{
    if (db) {
        sqlite3_finalize(get_vector_data_stmt);
        sqlite3_close(db);
    }
}

inline void SqliteVectorReaderNode::checkOK(int sqlite3_result)
{
    if (sqlite3_result != SQLITE_OK)
        throw opp_runtime_error("Cannot read SQLite result file '%s': %s", filename.c_str(), sqlite3_errmsg(db));
}

Port *SqliteVectorReaderNode::addVector(const VectorResult& vector)
{
    return addVector(vector.getVectorId());
}

Port *SqliteVectorReaderNode::addVector(int vectorId)
{
    DEBUG(("registering vector %d\n", (int)getVectorId()));
    PortData& portData = ports[vectorId];
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
    if (!db) {
        checkOK(sqlite3_open_v2(filename.c_str(), &db, SQLITE_OPEN_READWRITE, 0));
        if (allowIndexing) {
            DEBUG(("indexing started\n"));
            checkOK(sqlite3_exec(db, "CREATE INDEX IF NOT EXISTS vectordata_idx ON vectordata (vectorId);", nullptr, nullptr, nullptr));
            DEBUG(("indexing finished\n"));
        }
        checkOK(sqlite3_prepare_v2(db,
                "SELECT vectordata.rowid, eventNumber, simtimeRaw, simtimeExp, value "
                "FROM vectordata "
                "INNER JOIN vector USING (vectorId) "
                "INNER JOIN run USING (runId) "
                "WHERE vectorId = ? AND vectordata.rowid > ? ORDER BY vectordata.rowid;",
                -1, &get_vector_data_stmt, nullptr));
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

bool SqliteVectorReaderNode::readNextBlock(int vectorId, PortData& portData)
{
    bool retval = true;
    checkOK(sqlite3_reset(get_vector_data_stmt));
    checkOK(sqlite3_bind_int64(get_vector_data_stmt, 1, vectorId));
    checkOK(sqlite3_bind_int64(get_vector_data_stmt, 2, portData.currentRowId));
    DEBUG(("vectorId=%d, rowId=%ld\n", getVectorId(), (long)portData.currentRowId));

    for (size_t i=0; i < bufferSize; i++) {
        int resultCode = sqlite3_step(get_vector_data_stmt);
        if (resultCode == SQLITE_ROW) {
            Datum a;
            sqlite_int64 rowId = sqlite3_column_int64(get_vector_data_stmt, 0);
            a.eventNumber = sqlite3_column_int64(get_vector_data_stmt, 1);
            sqlite_int64 simtimeRaw = sqlite3_column_int64(get_vector_data_stmt, 2);
            int simtimeExp = sqlite3_column_int(get_vector_data_stmt, 3);
            a.xp = BigDecimal(simtimeRaw, simtimeExp);
            a.x = a.xp.dbl();
            a.y = sqlite3_column_double(get_vector_data_stmt, 4);
            portData.currentRowId  = rowId;

            // write to port(s)
            for (PortVector::const_iterator p = portData.ports.begin(); p != portData.ports.end(); ++p)
                p->getChannel()->write(&a, 1);
        }
        else if (resultCode == SQLITE_DONE) {
            retval = false;
            break;
        }
        else {
            checkOK(resultCode);  // throw
        }
    }

    sqlite3_clear_bindings(get_vector_data_stmt);

    return retval;
}

//-----

const char *SqliteVectorReaderNodeType::getDescription() const
{
    return "Reads SQLite output vector files.";
}

void SqliteVectorReaderNodeType::getAttributes(StringMap& attrs) const
{
    attrs["filename"] = "name of the SQLite output vector file (.vec)";
    attrs["allowindexing"] = "whether adding an index on the vectordata table is allowed (true/false)";
}

Node *SqliteVectorReaderNodeType::create(DataflowManager *mgr, StringMap& attrs) const
{
    checkAttrNames(attrs);

    const char *fname = attrs["filename"].c_str();
    bool allowIndexing = attrs["allowindexing"] == "true";

    Node *node = new SqliteVectorReaderNode(fname, allowIndexing);
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
    int vectorId;
    if (!parseInt(portname, vectorId))
        throw opp_runtime_error("port should be a vector id, received: %s", portname);
    return node1->addVector(vectorId);
}

}  // namespace scave
}  // namespace omnetpp
