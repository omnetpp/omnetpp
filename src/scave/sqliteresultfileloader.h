//=========================================================================
//  SQLITERESULTFILELOADER.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_SCAVE_SQLITERESULTFILELOADER_H
#define __OMNETPP_SCAVE_SQLITERESULTFILELOADER_H

#include <cassert>
#include <cmath>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <list>

#include "common/exception.h"
#include "common/commonutil.h"
#include "common/statistics.h"
#include "idlist.h"
#include "enumtype.h"
#include "scaveutils.h"
#include "enums.h"

#ifdef THREADED
#include "common/rwlock.h"
#endif

#include "common/sqlite3.h"

#include "resultfilemanager.h"

namespace omnetpp {
namespace scave {

using omnetpp::common::Statistics;

class SCAVE_API SqliteResultFileLoader : public IResultFileLoader
{
  protected:
    sqlite3 *db = nullptr;
    sqlite3_stmt *stmt = nullptr; // we only have one prepared statement active at a time
    ResultFile *fileRef = nullptr;
    std::map<sqlite3_int64, FileRun *> fileRunMap;
    bool verbose;
    InterruptedFlag *interrupted;

  protected:
    virtual void loadRuns();
    virtual void loadRunAttrs();
    virtual void loadRunItervars();
    virtual void loadRunConfigEntries();
    virtual void loadScalars();
    virtual void loadParameters();
    virtual void loadHistograms();
    virtual void loadVectors();
    virtual void cleanupDb();
    void prepareStatement(const char *sql);
    void finalizeStatement();
    static double sqlite3ColumnDouble(sqlite3_stmt *stmt, int fieldIdx);  // sqlite3_column_double(stmt, fieldIdx), but converts sql NULL value to NaN double value
    void checkOK(int sqlite3_result);
    void checkRow(int sqlite3_result);
    void error(const char *errmsg);
    void setBins(HistogramResult *histogram, std::vector<double>& binEdges, std::vector<double>& binValues);

  public:
    SqliteResultFileLoader(ResultFileManager* resultFileManagerPar, int flags, InterruptedFlag *interrupted);
    virtual ~SqliteResultFileLoader() {}
    virtual ResultFile *loadFile(const char *displayName, const char *fileSystemFileName) override;
};

}  // namespace scave
}  // namespace omnetpp


#endif
