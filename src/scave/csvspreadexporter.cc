//==========================================================================
//  CSVSPREADEXPORTER.CC - part of
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
#include "common/stringutil.h"
#include "common/stringtokenizer.h"
#include "common/stlutil.h"
#include "common/fileutil.h"
#include "csvspreadexporter.h"
#include "xyarray.h"
#include "resultfilemanager.h"
#include "exportutils.h"
#include "vectorutils.h"
#include "fields.h"

using namespace std;
using namespace omnetpp::common;

namespace omnetpp {
namespace scave {

static const std::map<std::string,CsvWriter::QuoteEscapingMethod> QUOTEMETHODS = {{"doubling", CsvWriter::DOUBLING}, {"backslash", CsvWriter::BACKSLASH}};
static const std::map<std::string,char> SEPARATORS = {{"tab", '\t'}, {"comma", ','}, {"semicolon", ';'}, {"colon", ':'}};
static const std::map<std::string,char> QUOTECHARS = {{"doublequote", '"'}, {"singlequote", '\''}};
static const std::map<std::string,bool> BOOLS = {{"true", true}, {"false", false}};
static const std::map<std::string,CsvForSpreadsheetExporter::VectorLayout> VECTORLAYOUTS = {{"horizontal", CsvForSpreadsheetExporter::HORIZONTAL}, {"vertical", CsvForSpreadsheetExporter::VERTICAL}};

class CsvForSpreadsheetExporterType : public ExporterType
{
    public:
        virtual std::string getFormatName() const {return "CSV-S";}
        virtual std::string getDisplayName() const {return "CSV for Spreadsheets";}
        virtual std::string getDescription() const {return "CSV format best suited for processing in spreadsheets";}
        virtual int getSupportedResultTypes() {return ResultFileManager::SCALAR | ResultFileManager::PARAMETER | ResultFileManager::VECTOR | ResultFileManager::STATISTICS | ResultFileManager::HISTOGRAM;}
        virtual std::string getFileExtension() {return "csv"; }
        virtual StringMap getSupportedOptions() const;
        virtual std::string getXswtForm() const;
        virtual Exporter *create() const {return new CsvForSpreadsheetExporter();}
};

string CsvForSpreadsheetExporterType::getXswtForm() const
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
            "      <layoutData x:class='GridData' horizontalAlignment='FILL' verticalAlignment='FILL' grabExcessHorizontalSpace='true'/>\n"
            "      <layout x:class='GridLayout' numColumns='2'/>\n"
            "      <x:children>\n"
            "         <button x:id='columnNames' text='Add header row' x:style='CHECK' selection='true'>\n"
            "           <layoutData x:class='GridData' horizontalSpan='2'/>\n"
            "         </button>\n"
            "         <button x:id='allowMixed' text='Allow mixed content' x:style='CHECK' selection='true'>\n"
            "           <layoutData x:class='GridData' horizontalSpan='2'/>\n"
            "         </button>\n"
            "         <label text='Vector arrangement:'/>\n"
            "         <combo x:id='vectorLayout' x:style='BORDER|READ_ONLY'>\n"
            "           <add x:p0='horizontal'/>\n"
            "           <add x:p0='vertical'/>\n"
            "           <text x:p0='vertical'/>\n"
            "         </combo>\n"
            "         <label text='Data precision:'/>\n"
            "         <spinner x:id='precision' x:style='BORDER' minimum='1' maximum='16' textLimit='2' selection='14'>\n"
            "           <layoutData x:class='GridData' widthHint='100'/>\n"
            "         </spinner>\n"
            "      </x:children>\n"
            "    </group>\n"
            "    <group text='CSV'>\n"
            "      <layoutData x:class='GridData' horizontalAlignment='FILL' verticalAlignment='FILL' grabExcessHorizontalSpace='true'/>\n"
            "      <layout x:class='GridLayout' numColumns='2'/>\n"
            "      <x:children>\n"
            "         <label text='Separator:'/>\n"
            "         <combo x:id='separator' x:style='BORDER|READ_ONLY'>\n"
            "           <add x:p0='tab'/>\n"
            "           <add x:p0='comma'/>\n"
            "           <add x:p0='semicolon'/>\n"
            "           <add x:p0='colon'/>\n"
            "           <text x:p0='comma'/>\n"
            "         </combo>\n"
            "         <label text='Quoting:'/>\n"
            "         <combo x:id='quoteChar' x:style='BORDER|READ_ONLY'>\n"
            "           <add x:p0='doublequote'/>\n"
            "           <add x:p0='singlequote'/>\n"
            "           <text x:p0='doublequote'/>\n"
            "         </combo>\n"
            "         <label text='Quote escaping:'/>\n"
            "         <combo x:id='quoteEscaping' x:style='BORDER|READ_ONLY'>\n"
            "           <add x:p0='doubling'/>\n"
            "           <add x:p0='backslash'/>\n"
            "           <text x:p0='doubling'/>\n"
            "         </combo>\n"
            "      </x:children>\n"
            "    </group>\n"
            "  </x:children>\n"
            "</xswt>\n";
}

StringMap CsvForSpreadsheetExporterType::getSupportedOptions() const
{
    StringMap options {
        {"precision", "The number of significant digits for floating-point values (double). The maximum value is ~15."},
        {"columnNames", "Whether to print a row containing column names. Values: 'true', 'false'"},
        {"allowMixed", "Whether to allow mixed content (e.g. vectors and scalars in the same file). Values: 'true', 'false'"},
        {"separator", "Separator character. Values: 'tab', 'comma', 'semicolon', 'colon'"},
        {"quoteChar", "Quote character. Values: 'doublequote', 'singlequote'"},
        {"quoteEscaping", "How to escape the quote character within quoted values. Values: 'doubling', 'backslash'"},
        {"vectorLayout", "The layout (format) in which vectors are exported. Values: 'horizontal','vertical'"},
    };
    return options;
}

//---

ExporterType *CsvForSpreadsheetExporter::getDescription()
{
    static CsvForSpreadsheetExporterType desc;
    return &desc;
}

void CsvForSpreadsheetExporter::setOption(const std::string& key, const std::string& value)
{
    checkOptionKey(getDescription(), key);
    if (key == "precision")
        setPrecision(opp_atol(value.c_str()));
    else if (key == "separator")
        setSeparator(translateOptionValue(SEPARATORS, value));
    else if (key == "quoteChar")
        setQuoteChar(translateOptionValue(QUOTECHARS, value));
    else if (key == "quoteEscaping")
        setQuoteEscapingMethod(translateOptionValue(QUOTEMETHODS, value));
    else if (key == "columnNames")
        setColumnNames(translateOptionValue(BOOLS,value));
    else if (key == "allowMixed")
        setMixedContentAllowed(translateOptionValue(BOOLS,value));
    else if (key == "vectorLayout")
        setVectorLayout(translateOptionValue(VECTORLAYOUTS,value));
    else
        throw opp_runtime_error("Exporter: unhandled option '%s'", key.c_str());
}

void CsvForSpreadsheetExporter::saveResults(const std::string& fileName, ResultFileManager *manager, const IDList& idlist, IProgressMonitor *monitor)
{
    int itemTypes = idlist.getItemTypes();
    bool mixedContent = itemTypes != 0 &&
            itemTypes != ResultFileManager::SCALAR &&
            itemTypes != ResultFileManager::PARAMETER &&
            itemTypes != ResultFileManager::VECTOR &&
            itemTypes != ResultFileManager::STATISTICS &&
            itemTypes != ResultFileManager::HISTOGRAM;

    if (mixedContent && !allowMixedContent)
        throw opp_runtime_error("CSV exporter: homogeneous results expected (all scalars, all vectors, "
                "all statistics, all histograms or all parameters); set allowMixed=true to allow mixed content");

    if (fileName == "-")
        csv.setOut(std::cout);
    else
        csv.open(fileName.c_str());

    if ((itemTypes & ResultFileManager::SCALAR) != 0) {
        if (mixedContent) {csv.writeNewLine(); csv.writeString("**** SCALARS ****"); csv.writeNewLine();}
        saveScalars(manager, idlist.filterByTypes(ResultFileManager::SCALAR), monitor);
    }
    if ((itemTypes & ResultFileManager::PARAMETER) != 0) {
        if (mixedContent) {csv.writeNewLine(); csv.writeString("**** PARAMETERS ****"); csv.writeNewLine();}
        saveParameters(manager, idlist.filterByTypes(ResultFileManager::PARAMETER), monitor);
    }
    if ((itemTypes & ResultFileManager::VECTOR) != 0) {
        if (mixedContent) {csv.writeNewLine(); csv.writeString("**** VECTORS ****"); csv.writeNewLine();}
        saveVectors(manager, idlist.filterByTypes(ResultFileManager::VECTOR), monitor);
    }
    if ((itemTypes & ResultFileManager::STATISTICS) != 0) {
        if (mixedContent) {csv.writeNewLine(); csv.writeString("**** STATISTICS ****"); csv.writeNewLine();}
        saveStatistics(manager, idlist.filterByTypes(ResultFileManager::STATISTICS), monitor);
    }
    if ((itemTypes & ResultFileManager::HISTOGRAM) != 0) {
        if (mixedContent) {csv.writeNewLine(); csv.writeString("**** HISTOGRAMS ****"); csv.writeNewLine();}
        saveHistograms(manager, idlist.filterByTypes(ResultFileManager::HISTOGRAM), monitor);
    }

    if (fileName != "-")
        csv.close();
}

void CsvForSpreadsheetExporter::collectItervars(ResultFileManager *manager, const IDList& idlist)
{
    itervarNames.clear();
    RunList runs = manager->getUniqueRuns(idlist);
    StringSet names = manager->getUniqueIterationVariableNames(runs);
    for (std::string name : names)
        itervarNames.push_back(name);
}

void CsvForSpreadsheetExporter::writeRunColumnNames()
{
    csv.writeString("run");
    for (std::string name : itervarNames)
        csv.writeString(name);
    csv.writeString(Scave::REPETITION);
}

void CsvForSpreadsheetExporter::writeRunColumns(Run *run)
{
    csv.writeString(run->getRunName());
    for (std::string name : itervarNames)
        csv.writeString(run->getIterationVariable(name));
    csv.writeString(run->getAttribute(Scave::REPETITION));
}

void CsvForSpreadsheetExporter::saveScalars(ResultFileManager *manager, const IDList& idlist, IProgressMonitor *monitor)
{
    //TODO use monitor
    collectItervars(manager, idlist);

    // write header
    if (columnNames) {
        writeRunColumnNames();
        csv.writeString("module");
        csv.writeString("name");
        csv.writeString("value");
        csv.writeNewLine();
    }

    // write data
    for (ID id : idlist) {
        ScalarResult buffer;
        const ScalarResult *scalar = manager->getScalar(id, buffer);
        writeRunColumns(scalar->getRun());
        csv.writeString(scalar->getModuleName());
        csv.writeString(scalar->getName());
        csv.writeDouble(scalar->getValue());
        csv.writeNewLine();
    }
}

void CsvForSpreadsheetExporter::saveParameters(ResultFileManager *manager, const IDList& idlist, IProgressMonitor *monitor)
{
    //TODO use monitor
    collectItervars(manager, idlist);

    // write header
    if (columnNames) {
        writeRunColumnNames();
        csv.writeString("module");
        csv.writeString("name");
        csv.writeString("value");
        csv.writeNewLine();
    }

    // write data
    for (ID id : idlist) {
        const ParameterResult *param = manager->getParameter(id);
        writeRunColumns(param->getRun());
        csv.writeString(param->getModuleName());
        csv.writeString(param->getName());
        csv.writeString(param->getValue());
        csv.writeNewLine();
    }
}

void CsvForSpreadsheetExporter::saveVectors(ResultFileManager *manager, const IDList& idlist, IProgressMonitor *monitor)
{
    //TODO use monitor
    collectItervars(manager, idlist);
    std::vector<XYArray *> xyArrays = readVectorsIntoArrays(manager, idlist, true, false, std::numeric_limits<size_t>::max(), vectorStartTime, vectorEndTime);
    assert((int)xyArrays.size() == idlist.size());

    int numVectors = (int)idlist.size();

    if (vectorLayout == HORIZONTAL) {
        // write header row
        if (columnNames) {
            writeRunColumnNames();
            csv.writeString("module");
            csv.writeString("name");
            csv.writeString("rowtype");
            csv.writeString("data...");
            csv.writeNewLine();
        }

        // write vectors
        for (int i = 0; i < numVectors; ++i) {
            const VectorResult *vector = manager->getVector(idlist.get(i));
            XYArray *data = xyArrays[i];

            // time row
            writeRunColumns(vector->getRun());
            csv.writeString(vector->getModuleName());
            csv.writeString(vector->getName());
            csv.writeString("TIME");
            for (int j = 0; j < data->length(); j++) {
                if (data->hasPreciseX())
                    csv.writeBigDecimal(data->getPreciseX(j));
                else
                    csv.writeDouble(data->getX(j));
            }
            csv.writeNewLine();

            // value row
            writeRunColumns(vector->getRun());
            csv.writeString(vector->getModuleName());
            csv.writeString(vector->getName());
            csv.writeString("VALUE");
            for (int j = 0; j < data->length(); j++)
                csv.writeBigDecimal(data->getY(j));
            csv.writeNewLine();
        }
    }
    else if (vectorLayout == VERTICAL) {
        // write header row
        if (columnNames) {
            for (int i = 0; i < numVectors; ++i) {
                const VectorResult *vector = manager->getVector(idlist.get(i));
                std::string columnTitle = vector->getName() + " " + vector->getModuleName() + " (" + makeRunTag(vector->getRun()) + ")";
                csv.writeString(columnTitle); // time column
                csv.writeBlank(); // value column
            }
            csv.writeNewLine();
        }

        // count number of data rows
        int numRows = 0;
        for (int i = 0; i < numVectors; ++i)
            numRows = std::max(numRows, xyArrays[i]->length());

        // write data rows
        for (int i = 0; i < numRows; ++i) {
            for (int j = 0; j < numVectors; ++j) {
                XYArray *data = xyArrays[j];
                if (data->length() <= i) {
                    csv.writeBlank();
                    csv.writeBlank();
                }
                else {
                    if (data->hasPreciseX())
                        csv.writeBigDecimal(data->getPreciseX(i));
                    else
                        csv.writeDouble(data->getX(i));
                    csv.writeBigDecimal(data->getY(i));
                }
            }
            csv.writeNewLine();
        }

    }
    else {
        throw opp_runtime_error("Invalid vector layout");
    }

    for (auto xyArray : xyArrays)
        delete xyArray;
}

void CsvForSpreadsheetExporter::saveStatistics(ResultFileManager *manager, const IDList& idlist, IProgressMonitor *monitor)
{
    //TODO use monitor
    collectItervars(manager, idlist);

    // header
    if (columnNames) {
        writeRunColumnNames();
        csv.writeString("module");
        csv.writeString("name");
        csv.writeString("count");
        csv.writeString("sumweights");
        csv.writeString("mean");
        csv.writeString("stddev");
        csv.writeString("min");
        csv.writeString("max");
        csv.writeNewLine();
    }

    // write statistics
    for (ID id : idlist) {
        const StatisticsResult *statistics = manager->getStatistics(id);
        writeRunColumns(statistics->getRun());
        csv.writeString(statistics->getModuleName());
        csv.writeString(statistics->getName());
        const Statistics& stat = statistics->getStatistics();
        csv.writeInt(stat.getCount());
        csv.writeDouble(stat.getSumWeights());
        csv.writeDouble(stat.getMean());
        csv.writeDouble(stat.getStddev());
        csv.writeDouble(stat.getMin());
        csv.writeDouble(stat.getMax());
        csv.writeNewLine();
    }
}

std::string CsvForSpreadsheetExporter::makeRunTag(Run *run)
{
    //TODO use experiment - measurement - replication instead?
    const std::string& iterationvars = run->getAttribute(Scave::ITERATIONVARS);
    if (iterationvars.empty())
        return std::string("#") + run->getAttribute(Scave::REPETITION) + " - " + run->getRunName();
    else
        return iterationvars + ", #" + run->getAttribute(Scave::REPETITION) + " - " + run->getRunName();
}

void CsvForSpreadsheetExporter::saveHistograms(ResultFileManager *manager, const IDList& idlist, IProgressMonitor *monitor)
{
    //TODO use monitor
    collectItervars(manager, idlist);
    bool includeHistogramStatistics = true; // could be added to options

    // header
    if (columnNames) {
        writeRunColumnNames();
        csv.writeString("module");
        csv.writeString("name");
        if (includeHistogramStatistics) {
            csv.writeString("count");
            csv.writeString("sumweights");
            csv.writeString("mean");
            csv.writeString("stddev");
            csv.writeString("min");
            csv.writeString("max");
        }
        csv.writeString("underflows");
        csv.writeString("overflows");
        csv.writeString("rowtype");
        csv.writeString("data...");
        csv.writeNewLine();
    }

    // write histograms, two lines each ("binedges" and "binvalues" lines)
    for (ID id : idlist) {
        const HistogramResult *histogram = manager->getHistogram(id);
        writeRunColumns(histogram->getRun());
        csv.writeString(histogram->getModuleName());
        csv.writeString(histogram->getName());
        if (includeHistogramStatistics) {
            const Statistics& stat = histogram->getStatistics();
            csv.writeInt(stat.getCount());
            csv.writeDouble(stat.getSumWeights());
            csv.writeDouble(stat.getMean());
            csv.writeDouble(stat.getStddev());
            csv.writeDouble(stat.getMin());
            csv.writeDouble(stat.getMax());
        }
        const Histogram& bins = histogram->getHistogram();
        csv.writeDouble(bins.getUnderflows());
        csv.writeDouble(bins.getOverflows());
        csv.writeString("binedges");
        for (double d : bins.getBinEdges())
            csv.writeDouble(d);
        csv.writeNewLine();

        writeRunColumns(histogram->getRun());
        csv.writeString(histogram->getModuleName());
        csv.writeString(histogram->getName());
        if (includeHistogramStatistics)
            for (int i = 0; i < 8; i++)
                csv.writeBlank();
        csv.writeString("binvalues");
        for (double d : bins.getBinValues())
            csv.writeDouble(d);
        csv.writeNewLine();
    }
}


}  // namespace scave
}  // namespace omnetpp

