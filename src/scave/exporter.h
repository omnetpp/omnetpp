//=========================================================================
//  EXPORTER.H - part of
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

#ifndef __OMNETPP_SCAVE_EXPORTER_H
#define __OMNETPP_SCAVE_EXPORTER_H

#include <string>
#include <vector>
#include <set>
#include <fstream>
#include <utility>
#include "common/bigdecimal.h"
#include "common/progressmonitor.h"
#include "xyarray.h"
#include "resultfilemanager.h"

namespace omnetpp {
namespace scave {

using omnetpp::common::IProgressMonitor;

class Exporter;

struct SCAVE_API ExporterType
{
    public:
        virtual ~ExporterType() {}
        virtual std::string getFormatName() const = 0; // for ExporterFactory and scavetool -F
        virtual std::string getDisplayName() const = 0; // for the IDE
        virtual std::string getDescription() const = 0;
        virtual int getSupportedResultTypes() = 0;
        virtual std::string getFileExtension() = 0;
        virtual StringMap getSupportedOptions() const = 0;
        virtual std::string getXswtForm() const = 0;
        virtual Exporter *create() const = 0;
};

/**
 * Base class for result exporters.
 */
class SCAVE_API Exporter
{
    protected:
        double vectorStartTime = -INFINITY, vectorEndTime = INFINITY;
    protected:
        virtual void checkOptionKey(ExporterType *desc, const std::string& key);
        virtual void checkItemTypes(const IDList& idlist, int supportedTypes);
    public:
        Exporter() {}
        virtual ~Exporter() {}
        virtual void setOption(const std::string& key, const std::string& value) = 0;
        virtual void setOptions(const StringMap& options);
        virtual void setVectorStartTime(double startTime) {vectorStartTime = startTime;}
        virtual void setVectorEndTime(double endTime) {vectorEndTime = endTime;}
        virtual void saveResults(const std::string& fileName, ResultFileManager *manager, const IDList& idlist, IProgressMonitor *monitor=nullptr) = 0;
};

class SCAVE_API ExporterFactory
{
    private:
        static void ensureInitialized();
    public:
        static StringVector getSupportedFormats();
        static ExporterType *getByFormat(const std::string& format);
        static std::string getFormatFromOutputFileName(const std::string& fileName);

        static Exporter *createExporter(const std::string& format);
};

}  // namespace scave
}  // namespace omnetpp


#endif


