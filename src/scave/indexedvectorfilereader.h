//=========================================================================
//  INDEXEDVECTORFILEREADER.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_SCAVE_INDEXEDVECTORFILEREADER_H
#define __OMNETPP_SCAVE_INDEXEDVECTORFILEREADER_H

#include <cassert>
#include <cfloat>
#include <vector>
#include <set>
#include <map>
#include <functional>
#include <cstdarg>
#include "common/filereader.h"
#include "scavedefs.h"
#include "ivectordatareader.h"
#include "resultfilemanager.h"
#include "vectorfileindex.h"
#include "filefingerprint.h"

namespace omnetpp {
namespace scave {


/**
 * Vector file reader with random access.
 * Each instance reads one vector from a vector file.
 */
class SCAVE_API IndexedVectorFileReader : public IVectorDataReader
{
    using VectorInfo = VectorFileIndex::VectorInfo;
    using Block = VectorFileIndex::Block;

    private:
        AdapterLambdaType adapterLambda;

        std::string fname;  // file name of the vector file
        VectorFileIndex *index = nullptr; // index of the vector file, loaded fully into the memory
        bool includeEventNumbers;
        FileFingerprint expectedFingerprint; // vec file fingerprint; empty = unspecified

    protected:
        /** reads a block from the vector file */
        Entries loadBlock(const Block& block, std::function<bool(const VectorDatum&)> filter = nullptr);

    public:
        explicit IndexedVectorFileReader(const char* filename, bool includeEventNumbers, Adapter *adapter, const FileFingerprint& fingerprint=FileFingerprint()) :
            IndexedVectorFileReader(filename, includeEventNumbers, [adapter](int vectorId, const std::vector<VectorDatum>& data) { adapter->process(vectorId, data); }, fingerprint)
        { }

        explicit IndexedVectorFileReader(const char* filename, bool includeEventNumbers, AdapterLambdaType adapter, const FileFingerprint& fingerprint=FileFingerprint());
        ~IndexedVectorFileReader();

        int getNumberOfEntries(int vectorId) override { return index->getVectorById(vectorId)->getCount(); };

        VectorDatum *getEntryBySerial(int vectorId, int64_t serial) override;
        VectorDatum *getEntryBySimtime(int vectorId, simultime_t simtime, bool after) override;
        VectorDatum *getEntryByEventnum(int vectorId, eventnumber_t eventNum, bool after) override;

        void collectEntries(const std::set<int>& vectorIds) override;
        void collectEntriesInSimtimeInterval(const std::set<int>& vectorIds, simultime_t startTime, simultime_t endTime) override;
        void collectEntriesInEventnumInterval(const std::set<int>& vectorIds, eventnumber_t startEventNum, eventnumber_t endEventNum) override;
};


}  // namespace scave
}  // namespace omnetpp

#endif
