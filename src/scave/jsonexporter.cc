//==========================================================================
//  JSONEXPORTER.CC - part of
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


#include "jsonexporter.h"

#include <cstdio>
#include <memory>
#include "common/stringutil.h"
#include "common/stringtokenizer.h"
#include "common/fileutil.h"
#include "common/stlutil.h"
#include "xyarray.h"
#include "resultfilemanager.h"
#include "exportutils.h"
#include "vectorutils.h"

using namespace std;
using namespace omnetpp::common;

namespace omnetpp {
namespace scave {

static const std::map<std::string,bool> BOOLS = {{"true", true}, {"false", false}};

class PythonExporterType : public ExporterType
{
    public:
        virtual std::string getFormatName() const {return "JSON";}
        virtual std::string getDisplayName() const {return "JSON";}
        virtual std::string getDescription() const {return "JSON format, optionally with Python flavour";}
        virtual int getSupportedResultTypes() {return ResultFileManager::SCALAR | ResultFileManager::PARAMETER | ResultFileManager::VECTOR | ResultFileManager::STATISTICS | ResultFileManager::HISTOGRAM;}
        virtual std::string getFileExtension() {return "json"; }
        virtual StringMap getSupportedOptions() const;
        virtual std::string getXswtForm() const;
        virtual Exporter *create() const {return new JsonExporter();}
};

string PythonExporterType::getXswtForm() const
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
            "         <button x:id='pythonFlavoured' text='Python-flavoured JSON' x:style='CHECK' selection='false'>\n"
            "           <layoutData x:class='GridData' horizontalSpan='2'/>\n"
            "         </button>\n"
            "         <button x:id='useNumpy' text='Use NumPy arrays in the output' x:style='CHECK' selection='true'>\n"
            "           <layoutData x:class='GridData' horizontalSpan='2' horizontalIndent='16'/>\n"
            "         </button>\n"
            "         <button x:id='skipResultAttributes' text='Skip result attributes' x:style='CHECK' selection='false'>\n"
            "           <layoutData x:class='GridData' horizontalSpan='2'/>\n"
            "         </button>\n"
            "         <label text='Data precision:'/>\n"
            "         <spinner x:id='precision' x:style='BORDER' minimum='1' maximum='16' textLimit='2' selection='14'>\n"
            "           <layoutData x:class='GridData' widthHint='100'/>\n"
            "         </spinner>\n"
            "         <label text='Indent size (spaces):'/>\n"
            "         <spinner x:id='indentSize' x:style='BORDER' minimum='0' maximum='16' textLimit='2' selection='4'>\n"
            "           <layoutData x:class='GridData' widthHint='100'/>\n"
            "         </spinner>\n"
            "      </x:children>\n"
            "    </group>\n"
            "  </x:children>\n"
            "</xswt>\n";
}

StringMap PythonExporterType::getSupportedOptions() const
{
    StringMap options {
        {"precision", "The number of significant digits for floating-point values (double). The maximum value is ~15."},
        {"pythonFlavoured", "Generate Python-flavoured JSON."},
        {"useNumpy", "Use NumPy arrays in the output."},
        {"indentSize", "Number of spaces to indent with. Set to 0 or 1 to reduce file size."},
        {"skipResultAttributes", "Do not export result attributes."},
    };
    return options;
}

//---

ExporterType *JsonExporter::getDescription()
{
    static PythonExporterType desc;
    return &desc;
}

void JsonExporter::setOption(const std::string& key, const std::string& value)
{
    checkOptionKey(getDescription(), key);
    if (key == "precision")
        setPrecision(opp_atol(value.c_str()));
    else if (key == "pythonFlavoured")
        setPythonFlavoured(translateOptionValue(BOOLS,value));
    else if (key == "useNumpy")
        setUseNumpy(translateOptionValue(BOOLS,value));
    else if (key == "indentSize")
        setIndentSize(opp_atol(value.c_str()));
    else if (key == "skipResultAttributes")
        setSkipResultAttributes(translateOptionValue(BOOLS,value));
    else
        throw opp_runtime_error("Exporter: unhandled option '%s'", key.c_str());
}

void JsonExporter::writeStringMap(const std::string& key, const StringMap& attrs)
{
    writer.openObject(key);
    for (auto& pair : attrs)
        writer.writeString(pair.first, pair.second);
    writer.closeObject();
}

void JsonExporter::writeOrderedKeyValueList(const std::string& key, const OrderedKeyValueList& list)
{
    writer.openArray(key);
    for (auto& pair : list) {
        writer.openObject(true);
        writer.writeString(pair.first, pair.second);
        writer.closeObject();
    }
    writer.closeArray();
}

void JsonExporter::writeStatisticsFields(const Statistics& stat)
{
    writer.writeInt("count", stat.getCount());
    writer.writeDouble("mean", stat.getMean());
    writer.writeDouble("stddev", stat.getStddev());
    writer.writeDouble("min", stat.getMin());
    writer.writeDouble("max", stat.getMax());
    if (!stat.isWeighted()) {
        writer.writeDouble("sum", stat.getSum());
        writer.writeDouble("sqrsum", stat.getSumSqr());
    }
    else {
        writer.writeDouble("weights", stat.getSumWeights());
        writer.writeDouble("weightedSum", stat.getWeightedSum());
        writer.writeDouble("sqrSumWeights", stat.getSumSquaredWeights());
        writer.writeDouble("weightedSqrSum", stat.getSumWeightedSquaredValues());
    }
}

void JsonExporter::writeVector(const std::vector<double>& v)
{
    std::ostream& out = writer.out();
    writeVectorProlog();
    bool first = true;
    int n = v.size();
    for (int i = 0; i < n; i++) {
        if (!first)
            out << ", ";
        first = false;
        if (i % 10 == 0 && i > 0)
            writer.doWriteNewLine(1);
        writer.doWriteDouble(v[i]);
    }
    writeVectorEpilog();
}

void JsonExporter::writeX(XYArray *array)
{
    std::ostream& out = writer.out();
    writeVectorProlog();
    bool first = true;
    int n = array->length();
    bool hasPreciseX = array->hasPreciseX();
    for (int i = 0; i < n; i++) {
        if (!first)
            out << ", ";
        first = false;
        if (i % 10 == 0 && i > 0)
            writer.doWriteNewLine(1);
        if (hasPreciseX)
            writer.doWriteBigDecimal(array->getPreciseX(i));
        else
            writer.doWriteDouble(array->getX(i));
    }
    writeVectorEpilog();
}

void JsonExporter::writeY(XYArray *array)
{
    std::ostream& out = writer.out();
    writeVectorProlog();
    bool first = true;
    int n = array->length();
    for (int i = 0; i < n; i++) {
        if (!first)
            out << ", ";
        first = false;
        if (i % 10 == 0 && i > 0)
            writer.doWriteNewLine(1);
        writer.doWriteDouble(array->getY(i));
    }
    writeVectorEpilog();
}

void JsonExporter::writeEventNumbers(XYArray *array)
{
    std::ostream& out = writer.out();
    writeVectorProlog();
    bool first = true;
    int n = array->length();
    for (int i = 0; i < n; i++) {
        if (!first)
            out << ", ";
        first = false;
        if (i % 10 == 0 && i > 0)
            writer.doWriteNewLine(1);
        writer.doWriteInt(array->getEventNumber(i));
    }
    writeVectorEpilog();
}

void JsonExporter::writeVectorProlog()
{
    std::ostream& out = writer.out();
    if (useNumpy)
        out << "np.array(";
    out << "[";
}

void JsonExporter::writeVectorEpilog()
{
    std::ostream& out = writer.out();
    out << "]";
    if (useNumpy)
        out << ",  dtype=np.float64)";
}

void JsonExporter::saveResults(const std::string& fileName, ResultFileManager *manager, const IDList& idlist, IProgressMonitor *monitor)
{
    //TODO progress reporting

    if (fileName == "-")
        writer.setOut(std::cout);
    else
        writer.open(fileName.c_str());

    if (!pythonFlavoured)
        useNumpy = false;

    if (pythonFlavoured) {
        // configure writer
        writer.setTrueString("True");
        writer.setFalseString("False");
        if (useNumpy) {
            writer.setNanString("np.nan");
            writer.setInfString("np.inf");
            writer.setNegInfString("-np.inf");
        }
        else {
            writer.setNanString("float('nan')");
            writer.setInfString("float('inf')");
            writer.setNegInfString("float('-inf')");
        }

        // write banner comment
        writer.out() << "# To load into Python, use: " << (useNumpy ? "import numpy as np; " : "") << "results = eval(open(filename).read())\n";
    }

    writer.openObject();

    RunList runList = manager->getUniqueRuns(idlist);

    for (Run *run : runList) {
        IDList idsInRun = idlist.filterByRun(run);

        writer.openObject(run->getRunName());

        // run metadata
        writeStringMap("attributes", run->getAttributes());
        writeStringMap("itervars", run->getIterationVariables());
        writeOrderedKeyValueList("config", run->getConfigEntries());

        // scalars
        IDList scalars = idsInRun.filterByTypes(ResultFileManager::SCALAR);
        if (!scalars.isEmpty()) {
            writer.openArray("scalars");
            for (ID id : scalars) {
                ScalarResult buffer;
                const ScalarResult *scalar = manager->getScalar(id, buffer);
                writer.openObject();
                writer.writeString("module", scalar->getModuleName());
                writer.writeString("name", scalar->getName());
                writer.writeDouble("value", scalar->getValue());
                if (!skipResultAttributes && !scalar->getAttributes().empty())
                    writeStringMap("attributes", scalar->getAttributes());
                writer.closeObject();
            }
            writer.closeArray();
        }

        // parameters
        IDList parameters = idsInRun.filterByTypes(ResultFileManager::PARAMETER);
        if (!parameters.isEmpty()) {
            writer.openArray("parameters");
            for (ID id : parameters) {
                const ParameterResult *scalar = manager->getParameter(id);
                writer.openObject();
                writer.writeString("module", scalar->getModuleName());
                writer.writeString("name", scalar->getName());
                writer.writeString("value", scalar->getValue());
                if (!skipResultAttributes && !scalar->getAttributes().empty())
                    writeStringMap("attributes", scalar->getAttributes());
                writer.closeObject();
            }
            writer.closeArray();
        }

        // statistics
        IDList statistics = idsInRun.filterByTypes(ResultFileManager::STATISTICS);
        if (!statistics.isEmpty()) {
            writer.openArray("statistics");
            for (ID id : statistics) {
                const StatisticsResult *stats = manager->getStatistics(id);
                writer.openObject();
                writer.writeString("module", stats->getModuleName());
                writer.writeString("name", stats->getName());
                if (!skipResultAttributes && !stats->getAttributes().empty())
                    writeStringMap("attributes", stats->getAttributes());
                writeStatisticsFields(stats->getStatistics());
                writer.closeObject();
            }
            writer.closeArray();
        }

        // histograms
        IDList histograms = idsInRun.filterByTypes(ResultFileManager::HISTOGRAM);
        if (!histograms.isEmpty()) {
            writer.openArray("histograms");
            for (ID id : histograms) {
                const HistogramResult *histogram = manager->getHistogram(id);
                writer.openObject();
                writer.writeString("module", histogram->getModuleName());
                writer.writeString("name", histogram->getName());
                if (!skipResultAttributes && !histogram->getAttributes().empty())
                    writeStringMap("attributes", histogram->getAttributes());
                writeStatisticsFields(histogram->getStatistics());

                const Histogram& bins = histogram->getHistogram();
                writer.writeDouble("underflows", bins.getUnderflows());
                writer.writeDouble("overflows", bins.getOverflows());
                writer.startRawValue("binedges"); writeVector(bins.getBinEdges());
                writer.startRawValue("binvalues"); writeVector(bins.getBinValues());
                writer.closeObject();
            }
            writer.closeArray();
        }

        // vectors
        IDList vectors = idsInRun.filterByTypes(ResultFileManager::VECTOR);
        if (!vectors.isEmpty()) {
            // compute vector data
            std::vector<XYArray *> xyArrays = readVectorsIntoArrays(manager, vectors, true, true, std::numeric_limits<size_t>::max(), vectorStartTime, vectorEndTime);
            Assert((int)xyArrays.size() == vectors.size());

            // export
            writer.openArray("vectors");
            for (int i = 0; i < (int)vectors.size(); i++) {
                ID id = vectors.get(i);
                const VectorResult *vector = manager->getVector(id);
                writer.openObject();
                writer.writeString("module", vector->getModuleName());
                writer.writeString("name", vector->getName());
                if (!skipResultAttributes && !vector->getAttributes().empty())
                    writeStringMap("attributes", vector->getAttributes());

                XYArray *array = xyArrays[i];
                writer.startRawValue("time"); writeX(array);
                writer.startRawValue("value"); writeY(array);
                if (array->hasEventNumbers()) {
                    writer.startRawValue("eventnumber"); writeEventNumbers(array);
                }

                writer.closeObject();
            }
            writer.closeArray();

            for (auto xyArray : xyArrays)
                delete xyArray;
        }

        writer.closeObject(); // close run
    }

    writer.closeObject();

    if (fileName != "-")
        writer.close();
}

}  // namespace scave
}  // namespace omnetpp

