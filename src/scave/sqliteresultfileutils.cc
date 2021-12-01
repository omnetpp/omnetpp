//=========================================================================
//=========================================================================
//  SQLITERESULTFILEUTILS.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cstring>
#include "sqliteresultfileutils.h"

using namespace std;
using namespace omnetpp::common;

namespace omnetpp {
namespace scave {

static const char SQLITE_HEADER_BYTES[] = "SQLite format 3";

bool SqliteResultFileUtils::isSqliteFile(const char *fileName)
{
    bool retval = false;
    FILE *f = fopen(fileName, "r");
    if (f != nullptr) {
        char buff[100];
        size_t l = fread(buff, 100, 1, f);
        if (l == 1) {
            if (memcmp(buff, SQLITE_HEADER_BYTES, sizeof(SQLITE_HEADER_BYTES)) == 0)
                retval = true;
        }
        fclose(f);
    }
    return retval;
}

void SqliteResultFileUtils::addIndexToVectorData(const char *fileName)
{
    sqlite3 *db;
    if (sqlite3_open(fileName, &db) != SQLITE_OK) {
        sqlite3_close(db);
        throw opp_runtime_error("Cannot open '%s' for indexing: %s", fileName, sqlite3_errmsg(db));
    }

    const char *sql = "CREATE INDEX IF NOT EXISTS vectordata_idx ON vectordata (vectorId);";
    if (sqlite3_exec(db, sql, nullptr, nullptr, nullptr) != SQLITE_OK) {
        throw opp_runtime_error("Cannot create index on '%s': %s", fileName, sqlite3_errmsg(db));
        sqlite3_close(db);
    }

    sqlite3_close(db);
}

}  // namespace scave
}  // namespace omnetpp
