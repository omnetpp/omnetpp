//=========================================================================
//  SQLITEVECTORDATAREADER.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_SCAVE_SQLITEVECTORDATAREADER_H
#define __OMNETPP_SCAVE_SQLITEVECTORDATAREADER_H

#include <map>
#include <set>
#include <string>
#include "ivectordatareader.h"
#include "filefingerprint.h"
#include "common/sqlite3.h"

namespace omnetpp {
namespace scave {

class VectorResult;

class SCAVE_API SqliteVectorDataReader : public IVectorDataReader
{
    protected:
        sqlite3 *db;
        sqlite3_stmt *stmt; // we only have one prepared statement active at a time
        std::string filename;
        bool includeEventNumbers;
        size_t batchSize = 64;
        AdapterLambdaType adapterLambda;
        FileFingerprint expectedFingerprint;
        std::map<int, int> simtimeExpForVectorId;

    protected:
        void ensureDbOpen();
        int getSimtimeExp(int vectorId);
        std::map<int, std::set<int>> groupVectorIdsBySimtimeExp(const std::set<int>& vectorIds);
        int getSerialForRowId(int64_t rowId);
        static double sqlite3ColumnDouble(sqlite3_stmt *stmt, int fieldIdx);
        inline void checkOK(int sqlite3_result);
        inline void checkRow(int sqlite3_result);
        void error(const char *errmsg);
        void prepareStatement(const char *sql);
        void finalizeStatement();

        VectorDatum *getSingleEntry(int simtimeExp);
        void processStatementRows();

    public:
        explicit SqliteVectorDataReader(const char *filename, bool includeEventNumbers, Adapter *adapter, const FileFingerprint& fingerprint = FileFingerprint()) :
            SqliteVectorDataReader(filename, includeEventNumbers, [adapter](int vectorId, const std::vector<VectorDatum>& data) { adapter->process(vectorId, data); }, fingerprint)
        { }

        explicit SqliteVectorDataReader(const char *filename, bool includeEventNumbers, AdapterLambdaType adapter, const FileFingerprint& fingerprint = FileFingerprint());

        virtual ~SqliteVectorDataReader();

        void setBatchSize(int size) {batchSize = size;}

        virtual int getNumberOfEntries(int vectorId) override;

        virtual VectorDatum *getEntryBySerial(int vectorId, int64_t serial) override;
        virtual VectorDatum *getEntryBySimtime(int vectorId, simultime_t simtime, bool after) override;
        virtual VectorDatum *getEntryByEventnum(int vectorId, eventnumber_t eventNum, bool after) override;

        virtual void collectEntries(const std::set<int>& vectorIds) override;
        virtual void collectEntriesInSimtimeInterval(const std::set<int>& vectorIds, simultime_t startTime, simultime_t endTime) override;
        virtual void collectEntriesInEventnumInterval(const std::set<int>& vectorIds, eventnumber_t startEventNum, eventnumber_t endEventNum) override;
};

}  // namespace scave
}  // namespace omnetpp


#endif
