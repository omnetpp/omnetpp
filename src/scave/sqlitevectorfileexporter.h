//=========================================================================
//  SQLITEVECTORFILEEXPORTER.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_SCAVE_SQLITEVECTORFILEEXPORTER_H
#define __OMNETPP_SCAVE_SQLITEVECTORFILEEXPORTER_H

#include "exporter.h"
#include "common/sqlitevectorfilewriter.h"

namespace omnetpp {
namespace scave {

class IDList;

using common::SqliteVectorFileWriter;

/**
 * Export data in SQLite Scalar File format.
 */
class SCAVE_API SqliteVectorFileExporter : public Exporter
{
    private:
        SqliteVectorFileWriter writer;
        int simtimeScaleExp = -12;
        bool skipSpecialValues = false;
        size_t perVectorMemoryLimit = 0;

    public:
        SqliteVectorFileExporter() {}

        void setSimtimeScaleExp(int d) {simtimeScaleExp = d;}
        int getSimtimeScaleExp() const {return simtimeScaleExp;}
        void setSkipSpecialValues(bool b) {skipSpecialValues = b;}
        bool getSkipSpecialValues() const {return skipSpecialValues;}
        void setOverallMemoryLimit(size_t n) {writer.setOverallMemoryLimit(n);}
        size_t getOverallMemoryLimit() const {return writer.getOverallMemoryLimit();}
        void setPerVectorMemoryLimit(size_t n) {perVectorMemoryLimit = n;}
        size_t getPerVectorMemoryLimit() const {return perVectorMemoryLimit;}

        virtual void setOption(const std::string& key, const std::string& value);
        virtual void saveResults(const std::string& fileName, ResultFileManager *manager, const IDList& idlist, IProgressMonitor *monitor=nullptr);

        static ExporterType *getDescription();
};

}  // namespace scave
}  // namespace omnetpp

#endif


