//==========================================================================
//  OMNETPPSCALARFILEEXPORTER.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#include <cstdio>
#include <memory>
#include "common/stringutil.h"
#include "common/stringtokenizer.h"
#include "common/stlutil.h"
#include "common/fileutil.h"
#include "xyarray.h"
#include "resultfilemanager.h"
#include "exportutils.h"
#include "omnetppscalarfileexporter.h"

using namespace std;
using namespace omnetpp::common;

namespace omnetpp {
namespace scave {

class OmnetppScalarFileExporterType : public ExporterType
{
    public:
        virtual std::string getFormatName() const {return "OmnetppScalarFile";}
        virtual std::string getDisplayName() const {return "OMNeT++ Scalar File";}
        virtual std::string getDescription() const {return "OMNeT++ scalar file (.sca) format";}
        virtual int getSupportedResultTypes() {return ResultFileManager::SCALAR | ResultFileManager::PARAMETER | ResultFileManager::STATISTICS | ResultFileManager::HISTOGRAM;}
        virtual std::string getFileExtension() {return "sca";}
        virtual StringMap getSupportedOptions() const;
        virtual std::string getXswtForm() const;
        virtual Exporter *create() const {return new OmnetppScalarFileExporter();}
};

string OmnetppScalarFileExporterType::getXswtForm() const
{
    return
            "<?xml version='1.0' encoding='UTF-8'?>\n"
            "<xswt xmlns:x='http://sweet_swt.sf.net/xswt'>\n"
            "  <import xmlns='http://sweet_swt.sf.net/xswt'>\n"
            "    <package name='java.lang'/>\n"
            "    <package name='org.eclipse.swt.widgets' />\n"
            "    <package name='org.eclipse.swt.graphics' />\n"
            "    <package name='org.eclipse.swt.layout' />\n"
            "    <package name='org.eclipse.swt.custom' />\n"
            "  </import>\n"
            "  <layout x:class='GridLayout' numColumns='2'/>\n"
            "  <x:children>\n"
            "    <group text='Options'>\n"
            "      <layoutData x:class='GridData' horizontalSpan='2' horizontalAlignment='FILL' grabExcessHorizontalSpace='true'/>\n"
            "      <layout x:class='GridLayout' numColumns='2'/>\n"
            "      <x:children>\n"
            "         <label text='Data precision:'/>\n"
            "         <spinner x:id='precision' x:style='BORDER' minimum='1' maximum='16' textLimit='2' selection='14'>\n"
            "           <layoutData x:class='GridData' widthHint='100'/>\n"
            "         </spinner>\n"
            "      </x:children>\n"
            "    </group>\n"
            "  </x:children>\n"
            "</xswt>\n";
}

StringMap OmnetppScalarFileExporterType::getSupportedOptions() const
{
    StringMap options {
        {"precision", "The number of significant digits for floating-point values (double). The maximum value is ~15."},
    };
    return options;
}

//---

ExporterType *OmnetppScalarFileExporter::getDescription()
{
    static OmnetppScalarFileExporterType desc;
    return &desc;
}

void OmnetppScalarFileExporter::setOption(const std::string& key, const std::string& value)
{
    checkOptionKey(getDescription(), key);
    if (key == "precision")
        setPrecision(opp_atol(value.c_str()));
    else
        throw opp_runtime_error("Exporter: unhandled option '%s'", key.c_str());
}

void OmnetppScalarFileExporter::saveResults(const std::string& fileName, ResultFileManager *manager, const IDList& idlist, IProgressMonitor *monitor)
{
    //TODO progress reporting
    checkItemTypes(idlist, ResultFileManager::SCALAR | ResultFileManager::STATISTICS | ResultFileManager::HISTOGRAM);

    removeFile(fileName.c_str(), "existing file"); // remove existing file, as open() appends
    writer.open(fileName.c_str());

    RunList runList = manager->getUniqueRuns(idlist);

    for (Run *run : runList) {
        writer.beginRecordingForRun(run->getRunName(), run->getAttributes(), run->getIterationVariables(), run->getConfigEntries());
        IDList filteredList = manager->filterIDList(idlist, run, nullptr, nullptr);
        ScalarResult buffer;
        for (ID id : filteredList) {
            if (ResultFileManager::getTypeOf(id) == ResultFileManager::SCALAR) {
                const ScalarResult *scalar = manager->getScalar(id, buffer);
                writer.recordScalar(scalar->getModuleName(), scalar->getName(), scalar->getValue(), scalar->getAttributes());
            }
            else if (ResultFileManager::getTypeOf(id) == ResultFileManager::PARAMETER) {
                const ParameterResult *parameter = manager->getParameter(id);
                writer.recordParameter(parameter->getModuleName(), parameter->getName(), parameter->getValue(), parameter->getAttributes());
            }
            else if (ResultFileManager::getTypeOf(id) == ResultFileManager::STATISTICS) {
                const StatisticsResult *statistics = manager->getStatistics(id);
                writer.recordStatistic(statistics->getModuleName(), statistics->getName(), statistics->getStatistics(), statistics->getAttributes());
            }
            else if (ResultFileManager::getTypeOf(id) == ResultFileManager::HISTOGRAM) {
                const HistogramResult *histogram = manager->getHistogram(id);
                writer.recordHistogram(histogram->getModuleName(), histogram->getName(), histogram->getStatistics(), histogram->getHistogram(), histogram->getAttributes());
            }
        }
        writer.endRecordingForRun();
    }
    writer.close();
}

}  // namespace scave
}  // namespace omnetpp

