//==========================================================================
//  OMNETPPVECTORFILEEXPORTER.CC - part of
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
#include "vectorutils.h"
#include "omnetppvectorfileexporter.h"

using namespace std;
using namespace omnetpp::common;

namespace omnetpp {
namespace scave {

static const std::map<std::string,bool> BOOLS = {{"true", true}, {"false", false}};


class OmnetppVectorFileExporterType : public ExporterType
{
    public:
        virtual std::string getFormatName() const {return "OmnetppVectorFile";}
        virtual std::string getDisplayName() const {return "OMNeT++ Vector File";}
        virtual std::string getDescription() const {return "OMNeT++ vector file (.vec) format";}
        virtual int getSupportedResultTypes() {return ResultFileManager::VECTOR;}
        virtual std::string getFileExtension() {return "vec";}
        virtual StringMap getSupportedOptions() const;
        virtual std::string getXswtForm() const;
        virtual Exporter *create() const {return new OmnetppVectorFileExporter();}
};

string OmnetppVectorFileExporterType::getXswtForm() const
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
            "         <button x:id='skipSpecialValues' text='Skip special values (NaN, +/-Inf)' x:style='CHECK' selection='false'>\n"
            "           <layoutData x:class='GridData' horizontalSpan='2'/>\n"
            "         </button>\n"
            "         <label text='Data precision:'/>\n"
            "         <spinner x:id='precision' x:style='BORDER' minimum='1' maximum='16' textLimit='2' selection='14'>\n"
            "           <layoutData x:class='GridData' widthHint='100'/>\n"
            "         </spinner>\n"
            "      </x:children>\n"
            "    </group>\n"
            "  </x:children>\n"
            "</xswt>\n";
}

StringMap OmnetppVectorFileExporterType::getSupportedOptions() const
{
    StringMap options {
        {"skipSpecialValues", "Allow and skip NaN and +/-Inf values as simulation time in vectors."},
        {"precision", "The number of significant digits for floating-point values (double). The maximum value is ~15."},
        {"overallMemoryLimitMB", "Maximum amount of memory allowed to use, in megabytes. Use zero for no limit."},
        {"perVectorMemoryLimitKB", "Maximum amount of memory allowed to use per vector by the writer for output buffering, in kilobytes. Use zero for no limit."},
    };
    return options;
}

//---

ExporterType *OmnetppVectorFileExporter::getDescription()
{
    static OmnetppVectorFileExporterType desc;
    return &desc;
}

void OmnetppVectorFileExporter::setOption(const std::string& key, const std::string& value)
{
    checkOptionKey(getDescription(), key);
    if (key == "precision")
        setPrecision(opp_atol(value.c_str()));
    else if (key == "skipSpecialValues")
        setSkipSpecialValues(translateOptionValue(BOOLS,value));
    else if (key == "overallMemoryLimitMB")
        setOverallMemoryLimit(opp_atol(value.c_str()) * 1024*1024);
    else if (key == "perVectorMemoryLimitKB")
        setPerVectorMemoryLimit(opp_atol(value.c_str()) * 1024);
    else
        throw opp_runtime_error("Exporter: unhandled option '%s'", key.c_str());
}

//TODO caller should remove file in case of exception!!!
void OmnetppVectorFileExporter::saveResults(const std::string& fileName, ResultFileManager *manager, const IDList& idlist, IProgressMonitor *monitor)
{
    //TODO progress reporting
    checkItemTypes(idlist, ResultFileManager::VECTOR);

    RunList runList = manager->getUniqueRuns(idlist);

    if (runList.size() > 1)
        throw opp_runtime_error("Exporter: OMNeT++ vec files currently do not support multiple runs per file"); //TODO revise later

    removeFile(fileName.c_str(), "existing file"); // remove existing file, just in case
    writer.open(fileName.c_str());

    for (Run *run : runList) {
        writer.beginRecordingForRun(run->getRunName(), run->getAttributes(), run->getIterationVariables(), run->getConfigEntries());
        IDList filteredList = manager->filterIDList(idlist, run, nullptr, nullptr);

        // register all vectors
        std::vector<void*> vectorHandles(filteredList.size());
        for (int i = 0; i < filteredList.size(); i++) {
            ID id = filteredList.get(i);
            const VectorResult *vector = manager->getVector(id);
            bool hasEventNumbers = vector->getColumns()=="ETV";
            vectorHandles[i] = writer.registerVector(vector->getModuleName(), vector->getName(), vector->getAttributes(), perVectorMemoryLimit, hasEventNumbers);
        }

        // write data for all vectors
        std::vector<XYArray *> xyArrays = readVectorsIntoArrays(manager, filteredList, true, true, std::numeric_limits<size_t>::max(), vectorStartTime, vectorEndTime);
        Assert((int)xyArrays.size() == filteredList.size());

        for (int i = 0; i < filteredList.size(); i++) {
            ID id = filteredList.get(i);
            const VectorResult *vector = manager->getVector(id);
            void *vectorHandle = vectorHandles[i];
            XYArray *array = xyArrays[i];
            int length = array->length();
            bool hasPreciseX = array->hasPreciseX();
            for (int j = 0; j < length; j++) {
                const BigDecimal time = hasPreciseX ? array->getPreciseX(j) : BigDecimal(array->getX(j));
                if (!time.isSpecial())
                    writer.recordInVector(vectorHandle, array->getEventNumber(j), time.getIntValue(), time.getScale(), array->getY(j));
                else if (!skipSpecialValues) {
                    std::string vectorName = vector->getModuleName() + "." + vector->getName();
                    throw opp_runtime_error("Illegal value (NaN of Inf) encountered as time while exporting vector %s; "
                            "use skipSpecialValues=true to turn off this error message", vectorName.c_str());
                }
            }
        }

        for (auto xyArray : xyArrays)
            delete xyArray;

        writer.endRecordingForRun();
    }
    writer.close();
}

}  // namespace scave
}  // namespace omnetpp

