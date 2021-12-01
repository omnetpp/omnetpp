//=========================================================================
//  OMNETPPVECTORFILEEXPORTER.H - part of
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

#ifndef __OMNETPP_SCAVE_OMNETPPVECTORFILEEXPORTER_H
#define __OMNETPP_SCAVE_OMNETPPVECTORFILEEXPORTER_H

#include "exporter.h"
#include "common/omnetppvectorfilewriter.h"

namespace omnetpp {
namespace scave {

class IDList;

using common::OmnetppVectorFileWriter;

/**
 * Export data in OMNeT++ Vector File format.
 */
class SCAVE_API OmnetppVectorFileExporter : public Exporter
{
    private:
        OmnetppVectorFileWriter writer;
        bool skipSpecialValues = false;
        size_t perVectorMemoryLimit = 0;

    public:
        OmnetppVectorFileExporter() {}

        void setPrecision(int prec) {writer.setPrecision(prec);}
        int getPrecision() const {return writer.getPrecision();}
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


