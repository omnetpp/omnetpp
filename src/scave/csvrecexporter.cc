//==========================================================================
//  CSVRECEXPORTER.CC - part of
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


#include "csvrecexporter.h"

#include <cstdio>
#include "common/stringutil.h"
#include "common/stringtokenizer.h"
#include "common/stlutil.h"
#include "common/fileutil.h"
#include "xyarray.h"
#include "resultfilemanager.h"
#include "datasorter.h"
#include "datatable.h"
#include "exportutils.h"

using namespace std;
using namespace omnetpp::common;

namespace omnetpp {
namespace scave {

static const std::map<std::string,CsvWriter::QuoteEscapingMethod> QUOTEMETHODS = {{"doubling", CsvWriter::DOUBLING}, {"backslash", CsvWriter::BACKSLASH}};
static const std::map<std::string,char> SEPARATORS = {{"tab", '\t'}, {"comma", ','}, {"semicolon", ';'}, {"colon", ':'}};
static const std::map<std::string,char> QUOTECHARS = {{"doublequote", '"'}, {"singlequote", '\''}};
static const std::map<std::string,bool> BOOLS = {{"true", true}, {"false", false}};

class CsvRecordsExporterType : public ExporterType
{
    public:
        virtual std::string getFormatName() const {return "CSV-R";}
        virtual std::string getDisplayName() const {return "CSV Records";}
        virtual std::string getDescription() const {return "Export results in CSV format as a table of records, suitable for importing into Python's Pandas or R.";}
        virtual int getSupportedResultTypes() {return ResultFileManager::SCALAR | ResultFileManager::VECTOR | ResultFileManager::STATISTICS | ResultFileManager::HISTOGRAM;}
        virtual std::string getFileExtension() {return "csv"; }
        virtual StringMap getSupportedOptions() const;
        virtual std::string getXswtForm() const;
        virtual Exporter *create() const {return new CsvRecordsExporter();}
};

string CsvRecordsExporterType::getXswtForm() const
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
            "         <button x:id='omitBlankColumns' text='Omit blank columns' x:style='CHECK' selection='true'>\n"
            "           <layoutData x:class='GridData' horizontalSpan='2'/>\n"
            "         </button>\n"
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

StringMap CsvRecordsExporterType::getSupportedOptions() const
{
    StringMap options {
        {"precision", "The number of significant digits for floating-point values (double). The maximum value is ~15."},
        {"columnNames", "Whether to print a row containing column names. Values: 'true', 'false'"},
        {"omitBlankColumns", "Whether to omit columns generated by result types not present in the output. Values: 'true', 'false'"},
        {"separator", "Separator character. Values: 'tab', 'comma', 'semicolon', 'colon'"},
        {"quoteChar", "Quote character. Values: 'doublequote', 'singlequote'"},
        {"quoteEscaping", "How to escape the quote character within quoted values. Values: 'doubling', 'backslash'"},
        {"vectorFilters", "A semicolon-separated list of operations to be applied to the vectors to be exported. See the 'operations' help page. Example value: 'winavg(10);mean'"},
    };
    return options;
}

//---

ExporterType *CsvRecordsExporter::getDescription()
{
    static CsvRecordsExporterType desc;
    return &desc;
}

void CsvRecordsExporter::setOption(const std::string& key, const std::string& value)
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
    else if (key == "omitBlankColumns")
        setOmitBlankColumns(translateOptionValue(BOOLS,value));
    else if (key == "vectorFilters")
        setVectorFilters(StringTokenizer(value.c_str(), ";").asVector());
    else
        throw opp_runtime_error("Exporter: unhandled option '%s'", key.c_str());
}

void CsvRecordsExporter::saveResults(const std::string& fileName, ResultFileManager *manager, const IDList& idlist, IProgressMonitor *monitor)
{
    if (fileName == "-")
        csv.setOut(std::cout);
    else
        csv.open(fileName.c_str());

    saveResultsAsRecords(manager, idlist, monitor);

    if (fileName != "-")
        csv.close();
}

void CsvRecordsExporter::saveResultsAsRecords(ResultFileManager *manager, const IDList& idlist, IProgressMonitor *monitor)
{
    // preparation
    int itemTypes = idlist.getItemTypes();
    bool haveScalars = (itemTypes & ResultFileManager::SCALAR) != 0;
    bool haveStatistics = (itemTypes & (ResultFileManager::STATISTICS | ResultFileManager::HISTOGRAM)) != 0;
    bool haveHistograms = (itemTypes & ResultFileManager::HISTOGRAM) != 0;
    bool haveVectors = (itemTypes & ResultFileManager::VECTOR) != 0;

    bool haveStatisticColumns = haveStatistics || !omitBlankColumns;
    bool haveHistogramColumns = haveHistograms || !omitBlankColumns;
    bool haveVectorColumns = haveVectors || !omitBlankColumns;

    std::vector<std::string> commonAndScalarColumnNames = {"run", "type", "module", "name", "attrname", "attrvalue", "value"}; // note: 'value' is scalar value
    std::vector<std::string> statisticColumnNames, histogramColumnNames, vectorColumnNames;
    if (haveStatisticColumns)
        statisticColumnNames = {"count", "sumweights", "mean", "stddev", "min", "max"};
    if (haveHistogramColumns)
        histogramColumnNames = {"binedges", "binvalues"};
    if (haveVectorColumns)
        vectorColumnNames = {"vectime", "vecvalue"};

    std::vector<std::string> allColumnNames = commonAndScalarColumnNames;
    addAll(allColumnNames, statisticColumnNames);
    addAll(allColumnNames, histogramColumnNames);
    addAll(allColumnNames, vectorColumnNames);

    int numColumns = allColumnNames.size();

    // write header line
    if (columnNames) {
        for (auto c : allColumnNames)
            csv.writeString(c);
        csv.writeNewLine();
    }

    // record runs
    RunList *runList = manager->getUniqueRuns(idlist);
    for (Run *run : *runList) {
        for (auto pair : run->getAttributes())
            writeRunAttrRecord(run->getRunName(), "runattr", pair.first, pair.second, numColumns);
        for (auto pair : run->getIterationVariables())
            writeRunAttrRecord(run->getRunName(), "itervar", pair.first, pair.second, numColumns);
        for (auto pair : run->getParamAssignments())
            writeRunAttrRecord(run->getRunName(), "param", pair.first, pair.second, numColumns);
    }
    delete runList;

    // record scalars
    if (haveScalars) {
        IDList scalarIDs = idlist.filterByTypes(ResultFileManager::SCALAR);
        for (int i = 0; i < (int)scalarIDs.size(); ++i) {
            const ScalarResult& scalar = manager->getScalar(scalarIDs.get(i));
            writeResultItemBase(scalar, "scalar", numColumns);
            csv.writeDouble(scalar.getValue());
            finishRecord(numColumns);
            writeResultAttrRecords(scalar, numColumns);
        }
    }

    // record statistics and histograms
    if (haveStatistics) {
        IDList statisticsIDs = idlist.filterByTypes(ResultFileManager::STATISTICS | ResultFileManager::HISTOGRAM);
        for (int i = 0; i < (int)statisticsIDs.size(); ++i) {
            ID id = statisticsIDs.get(i);
            bool isHistogram = ResultFileManager::getTypeOf(id) == ResultFileManager::HISTOGRAM;
            const StatisticsResult& statistic = manager->getStatistics(id);
            writeResultItemBase(statistic, isHistogram ? "histogram" : "statistic", numColumns);
            csv.writeBlank(); // skip 'value'
            const Statistics& stat = statistic.getStatistics();
            csv.writeInt(stat.getCount());
            if (stat.isWeighted())
                csv.writeDouble(stat.getSumWeights());
            else
                csv.writeBlank();
            csv.writeDouble(stat.getMean());
            csv.writeDouble(stat.getStddev());
            csv.writeDouble(stat.getMin());
            csv.writeDouble(stat.getMax());
            if (isHistogram) {
                const Histogram& histogram = dynamic_cast<const HistogramResult*>(&statistic)->getHistogram();
                auto binLowerBounds = histogram.getBinLowerBounds();
                auto binValues = histogram.getBinValues();
                Assert(binLowerBounds[0] == NEGATIVE_INFINITY);
                Assert(binValues.size() == binLowerBounds.size());
                writeAsString(binLowerBounds, 1, binLowerBounds.size()); // skip initial -inf element
                writeAsString(binValues, 1, binValues.size()-1); // remove first and last element (overflow/underflow counts)
            }
            finishRecord(numColumns);
            writeResultAttrRecords(statistic, numColumns);
        }
    }

    // record vectors
    if (haveVectors) {
        // load vector data
        IDList vectorIDs = idlist.filterByTypes(ResultFileManager::VECTOR);
        std::vector<XYArray *> xyArrays = readVectorsIntoArrays(manager, vectorIDs, vectorFilters);
        assert((int)xyArrays.size() == vectorIDs.size());

        // write vectors
        int numVectors = (int)vectorIDs.size();
        for (int i = 0; i < numVectors; ++i) {
            const VectorResult& vector = manager->getVector(vectorIDs.get(i));
            writeResultItemBase(vector, "vector", numColumns);
            csv.writeBlank(); // skip 'value'
            for (size_t i = 0; i < statisticColumnNames.size() + histogramColumnNames.size(); i++)
                csv.writeBlank(); // skip intermediate columns
            XYArray *data = xyArrays[i];
            writeXAsString(data);
            writeYAsString(data);
            finishRecord(numColumns);
            writeResultAttrRecords(vector, numColumns);
        }

        for (auto xyArray : xyArrays)
            delete xyArray;
    }
}

void CsvRecordsExporter::writeRunAttrRecord(const std::string& runId, const char *type, const std::string& attrName, const std::string& value, int numColumns)
{
    csv.writeString(runId);
    csv.writeString(type);
    csv.writeBlank();
    csv.writeBlank();
    csv.writeString(attrName);
    csv.writeString(value);
    finishRecord(numColumns);
}

void CsvRecordsExporter::writeResultAttrRecords(const ResultItem& result, int numColumns)
{
    for (auto pair : result.getAttributes()) {
        csv.writeString(result.getRun()->getRunName());
        csv.writeString("attr");
        csv.writeString(result.getModuleName());
        csv.writeString(result.getName());
        csv.writeString(pair.first); // attrName
        csv.writeString(pair.second); // attrValue
        finishRecord(numColumns);
    }
}

void CsvRecordsExporter::writeResultItemBase(const ResultItem& result, const char *type, int numColumns)
{
    csv.writeString(result.getRun()->getRunName());
    csv.writeString(type);
    csv.writeString(result.getModuleName());
    csv.writeString(result.getName());
    csv.writeBlank();  // skip 'attrName'
    csv.writeBlank();  // skip 'attrValue'
}

void CsvRecordsExporter::finishRecord(int numColumns)
{
    for (int i = csv.getColumn(); i < numColumns; i++)
        csv.writeBlank();
    csv.writeNewLine();
}

void CsvRecordsExporter::writeAsString(const std::vector<double>& data, int startIndex, int endIndex)
{
    std::ostream& out = csv.out();
    csv.beginRaw();
    out << "\"";
    for (int i = startIndex; i < endIndex; i++) { // endIndex is exclusive
        if (i != 0)
            out << " ";
        csv.writeRawDouble(data[i]);
    }
    csv.out() << "\"";
    csv.endRaw();
}

void CsvRecordsExporter::writeXAsString(const XYArray *data)
{
    std::ostream& out = csv.out();
    csv.beginRaw();
    out << "\"";
    for (int j = 0; j < data->length(); j++) {
        if (j != 0)
            out << " ";
        if (data->hasPreciseX())
            out << data->getPreciseX(j).str();
        else
            csv.writeRawDouble(data->getX(j));
    }
    csv.out() << "\"";
    csv.endRaw();
}

void CsvRecordsExporter::writeYAsString(const XYArray *data)
{
    std::ostream& out = csv.out();
    csv.beginRaw();
    out << "\"";
    for (int j = 0; j < data->length(); j++) {
        if (j != 0)
            out << " ";
        csv.writeRawDouble(data->getY(j));
    }
    csv.out() << "\"";
    csv.endRaw();
}

}  // namespace scave
}  // namespace omnetpp

