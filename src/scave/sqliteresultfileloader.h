//=========================================================================
//  RESULTFILEMANAGER.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga, Tamas Borbely, Zoltan Bojthe
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

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
#include "idlist.h"
#include "enumtype.h"
#include "statistics.h"
#include "scaveutils.h"
#include "enums.h"

#ifdef THREADED
#include "common/rwlock.h"
#endif

#include "common/sqlite3.h"

#include "resultfilemanager.h"

namespace omnetpp {
namespace scave {

class SCAVE_API SqliteResultFileLoader : public IResultFileLoader
{
    sqlite3 *db;
    ResultFile *fileRef;
    std::map<sqlite3_int64, FileRun *> fileRunMap;
    bool hasScalar;
    bool hasVector;
  protected:
    void loadDbInfo();
    void loadRuns();
    void loadRunAttrs();
    void loadScalars();
    void loadHistograms();
    void loadVectors();
    static double sqlite3ColumnDouble(sqlite3_stmt *stmt, int fieldIdx);        // sqlite3_column_double(stmt, fieldIdx), but converts sql NULL value to NaN double value

  public:
    SqliteResultFileLoader(ResultFileManager *resultFileManagerPar) : IResultFileLoader(resultFileManagerPar), db(nullptr), fileRef(nullptr), hasScalar(false), hasVector(false) {}
    virtual ResultFile *loadFile(const char *fileName, const char *fileSystemFileName=nullptr, bool reload=false) override;
};

} // namespace scave
}  // namespace omnetpp


#endif
