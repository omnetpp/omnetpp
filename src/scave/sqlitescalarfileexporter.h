//=========================================================================
//  SQLITESCALARFILEEXPORTER.H - part of
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

#ifndef __OMNETPP_SCAVE_SQLITESCALARFILEEXPORTER_H
#define __OMNETPP_SCAVE_SQLITESCALARFILEEXPORTER_H

#include "exporter.h"
#include "common/sqlitescalarfilewriter.h"

namespace omnetpp {
namespace scave {

class IDList;

using common::SqliteScalarFileWriter;

/**
 * Export data in SQLite Scalar File format.
 */
class SCAVE_API SqliteScalarFileExporter : public Exporter
{
    private:
        SqliteScalarFileWriter writer;
        int simtimeScaleExp = -12;

    public:
        SqliteScalarFileExporter() {}

        void setSimtimeScaleExp(int d) {simtimeScaleExp = d;}
        int getSimtimeScaleExp() const {return simtimeScaleExp;}

        virtual void setOption(const std::string& key, const std::string& value);
        virtual void saveResults(const std::string& fileName, ResultFileManager *manager, const IDList& idlist, IProgressMonitor *monitor=nullptr);

        static ExporterType *getDescription();
};

}  // namespace scave
}  // namespace omnetpp

#endif


