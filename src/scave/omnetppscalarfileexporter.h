//=========================================================================
//  OMNETPPSCALARFILEEXPORTER.H - part of
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

#ifndef __OMNETPP_SCAVE_OMNETPPSCALARFILEEXPORTER_H
#define __OMNETPP_SCAVE_OMNETPPSCALARFILEEXPORTER_H

#include "exporter.h"
#include "common/omnetppscalarfilewriter.h"

namespace omnetpp {
namespace scave {

class IDList;

using common::OmnetppScalarFileWriter;

/**
 * Export data in OMNeT++ Scalar File format.
 */
class SCAVE_API OmnetppScalarFileExporter : public Exporter
{
    private:
        OmnetppScalarFileWriter writer;

    public:
        OmnetppScalarFileExporter() {}

        void setPrecision(int prec) {writer.setPrecision(prec);}
        int getPrecision() const {return writer.getPrecision();}

        virtual void setOption(const std::string& key, const std::string& value);
        virtual void saveResults(const std::string& fileName, ResultFileManager *manager, const IDList& idlist, IProgressMonitor *monitor=nullptr);

        static ExporterType *getDescription();
};

}  // namespace scave
}  // namespace omnetpp

#endif


