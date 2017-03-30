//==========================================================================
//  PYTHONEXPORTER.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
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
#include "common/fileutil.h"
#include "common/stlutil.h"
#include "xyarray.h"
#include "resultfilemanager.h"
#include "exportutils.h"
#include "pythonexporter.h"

using namespace std;
using namespace omnetpp::common;

namespace omnetpp {
namespace scave {

static const std::map<std::string,bool> BOOLS = {{"true", true}, {"false", false}};

class PythonExporterType : public ExporterType
{
    public:
        virtual std::string getFormatName() const {return "Python";}
        virtual std::string getDisplayName() const {return "Python Source File";}
        virtual std::string getDescription() const {return "Export results as Python source file containing a JSON-like data structure, optionally with NumPy arrays.";}
        virtual int getSupportedResultTypes() {return ResultFileManager::SCALAR | ResultFileManager::VECTOR | ResultFileManager::STATISTICS | ResultFileManager::HISTOGRAM;}
        virtual std::string getFileExtension() {return "py"; }
        virtual StringMap getSupportedOptions() const;
        virtual std::string getXswtForm() const;
        virtual Exporter *create() const {return new PythonExporter();}
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
            "         <button x:id='skipResultAttributes' text='Skip result attributes' x:style='CHECK' selection='false'>\n"
            "           <layoutData x:class='GridData' horizontalSpan='2'/>\n"
            "         </button>\n"
            "         <button x:id='useNumpy' text='Use NumPy arrays in generated code' x:style='CHECK' selection='true'>\n"
            "           <layoutData x:class='GridData' horizontalSpan='2'/>\n"
            "         </button>\n"
            "         <button x:id='nakedNan' text='Use nan and inf variables in generated code instead of float(&quot;nan&quot;) forms' x:style='CHECK' selection='true'>\n"
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
        {"useNumpy", "Generate code that relies on NumPy data structures."},
        {"nakedNan", "Generate code that defines 'nan' and 'inf' variables and uses them in the generated code instead of the longer 'float(\"nan\")' / 'float(\"inf\")' forms"},
        {"indentSize", "Number of spaces to indent with. Set to 0 or 1 to reduce file size."},
        {"skipResultAttributes", "Do not export result attributes."},
        {"vectorFilters", "A semicolon-separated list of operations to be applied to the vectors to be exported. See the 'operations' help page. Example value: 'winavg(10);mean'"},
    };
    return options;
}

//---

ExporterType *PythonExporter::getDescription()
{
    static PythonExporterType desc;
    return &desc;
}

void PythonExporter::setOption(const std::string& key, const std::string& value)
{
    checkOptionKey(getDescription(), key);
    if (key == "precision")
        setPrecision(opp_atol(value.c_str()));
    else if (key == "useNumpy")
        setUseNumpy(translateOptionValue(BOOLS,value));
    else if (key == "nakedNan")
        setNakedNan(translateOptionValue(BOOLS,value));
    else if (key == "indentSize")
        setIndentSize(opp_atol(value.c_str()));
    else if (key == "skipResultAttributes")
        setSkipResultAttributes(translateOptionValue(BOOLS,value));
    else if (key == "vectorFilters")
        setVectorFilters(StringTokenizer(value.c_str(), ";").asVector());
    else
        throw opp_runtime_error("Exporter: unhandled option '%s'", key.c_str());
}

void PythonExporter::writeStringMap(const std::string& key, const StringMap& attrs)
{
    writer.openObject(key);
    for (auto pair : attrs)
        writer.writeString(pair.first, pair.second);
    writer.closeObject();
}

void PythonExporter::writeStatisticsFields(const Statistics& stat)
{
    writer.writeDouble("count", stat.getCount());
    writer.writeDouble("mean", stat.getMean());
    writer.writeDouble("stddev", stat.getStddev());
    writer.writeDouble("sum", stat.getSum());
    writer.writeDouble("sqrsum", stat.getSumSqr());
    writer.writeDouble("min", stat.getMin());
    writer.writeDouble("max", stat.getMax());
    //TODO weighted?
}

void PythonExporter::writeVector(const std::vector<double>& v)
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

void PythonExporter::writeX(XYArray *array)
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

void PythonExporter::writeY(XYArray *array)
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

void PythonExporter::writeEventNumbers(XYArray *array)
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

void PythonExporter::writeVectorProlog()
{
    std::ostream& out = writer.out();
    if (useNumpy)
        out << "np.array(";
    out << "[";
}

void PythonExporter::writeVectorEpilog()
{
    std::ostream& out = writer.out();
    out << "]";
    if (useNumpy)
        out << ",  dtype=np.float64)";
}

static std::string filenameOf(const std::string path)
{
    std::string filename, dir;
    splitFileName(path.c_str(), dir, filename);
    return filename;
}

void PythonExporter::saveResults(const std::string& fileName, ResultFileManager *manager, const IDList& idlist, IProgressMonitor *monitor)
{
    //TODO progress reporting

    if (fileName == "-")
        writer.setOut(std::cout);
    else
        writer.open(fileName.c_str());

    writer.setUsePythonSyntax(true);

    std::string shortFilename = (fileName == "-") ? "exported.py" : filenameOf(fileName);
    writer.out() << "#\n";
    writer.out() << "# OMNeT++ simulation results exported as Python data structure.\n";
    writer.out() << "#\n";
    writer.out() << "# Hint: Use the following command:\n";
    writer.out() << "#   exec(open('" << shortFilename << "').read())\n";
    writer.out() << "# It will leave the data in the 'opp_results' local variable.\n";
    writer.out() << "#\n\n";
    if (useNumpy)
        writer.out() << "import numpy as np\n\n";
    if (writer.getNakedNan())
        writer.out() << "nan = float('nan')\ninf = float('inf')\n\n";
    writer.out() << "opp_results = ";
    writer.openObject();

    RunList *runList = manager->getUniqueRuns(idlist);
    std::unique_ptr<RunList> tmp(runList);

    for (Run *run : *runList) {
        writer.openObject(run->getRunName());

        // run metadata
        writeStringMap("attributes", run->getAttributes());
        writeStringMap("moduleparams", run->getModuleParams());

        // scalars
        IDList scalars = idlist.filterByTypes(ResultFileManager::SCALAR);
        if (!scalars.isEmpty()) {
            writer.openArray("scalars");
            for (ID id : scalars) {
                const ScalarResult& scalar = manager->getScalar(id);
                writer.openObject();
                writer.writeString("module", scalar.getModuleName());
                writer.writeString("name", scalar.getName());
                writer.writeDouble("value", scalar.getValue());
                if (!skipResultAttributes && !scalar.getAttributes().empty())
                    writeStringMap("attributes", scalar.getAttributes());
                writer.closeObject();
            }
            writer.closeArray();
        }

        // statistics
        IDList statistics = idlist.filterByTypes(ResultFileManager::STATISTICS);
        if (!statistics.isEmpty()) {
            writer.openArray("statistics");
            for (ID id : statistics) {
                const StatisticsResult& stats = manager->getStatistics(id);
                writer.openObject();
                writer.writeString("module", stats.getModuleName());
                writer.writeString("name", stats.getName());
                if (!skipResultAttributes && !stats.getAttributes().empty())
                    writeStringMap("attributes", stats.getAttributes());
                writeStatisticsFields(stats.getStatistics());
                writer.closeObject();
            }
            writer.closeArray();
        }

        // histograms
        IDList histograms = idlist.filterByTypes(ResultFileManager::HISTOGRAM);
        if (!histograms.isEmpty()) {
            writer.openArray("histograms");
            for (ID id : histograms) {
                const HistogramResult& histogram = manager->getHistogram(id);
                writer.openObject();
                writer.writeString("module", histogram.getModuleName());
                writer.writeString("name", histogram.getName());
                if (!skipResultAttributes && !histogram.getAttributes().empty())
                    writeStringMap("attributes", histogram.getAttributes());
                writeStatisticsFields(histogram.getStatistics());

                const Histogram& bins = histogram.getHistogram();
                writer.startRawValue("histbins"); writeVector(bins.getBinLowerBounds());
                writer.startRawValue("histcounts"); writeVector(bins.getBinValues());
                writer.closeObject();
            }
            writer.closeArray();
        }

        // vectors
        IDList vectors = idlist.filterByTypes(ResultFileManager::VECTOR);
        if (!vectors.isEmpty()) {
            // compute vector data
            std::vector<XYArray *> xyArrays = readVectorsIntoArrays(manager, vectors, vectorFilters); //TODO rather: set up vectorFileWriter as consumer in the dataflow network
            Assert((int)xyArrays.size() == vectors.size());

            // export
            writer.openArray("vectors");
            for (int i = 0; i < vectors.size(); i++) {
                ID id = vectors.get(i);
                const VectorResult& vector = manager->getVector(id);
                writer.openObject();
                writer.writeString("module", vector.getModuleName());
                writer.writeString("name", vector.getName());
                if (!skipResultAttributes && !vector.getAttributes().empty())
                    writeStringMap("attributes", vector.getAttributes());

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

