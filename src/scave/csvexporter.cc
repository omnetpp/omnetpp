//==========================================================================
//  CSVEXPORTER.CC - part of
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
#include "common/stringutil.h"
#include "common/stringtokenizer.h"
#include "common/stlutil.h"
#include "xyarray.h"
#include "resultfilemanager.h"
#include "datasorter.h"
#include "datatable.h"
#include "exportutils.h"
#include "csvexporter.h"

using namespace std;
using namespace omnetpp::common;

namespace omnetpp {
namespace scave {

static const std::map<std::string,CsvWriter::QuoteEscapingMethod> QUOTEMETHODS = {{"doubling", CsvWriter::DOUBLING}, {"backslash", CsvWriter::BACKSLASH}};
static const std::map<std::string,char> SEPARATORS = {{"tab", '\t'}, {"comma", ','}, {"semicolon", ';'}, {"colon", ':'}};
static const std::map<std::string,char> QUOTECHARS = {{"doublequote", '"'}, {"singlequote", '\''}};
static const std::map<std::string,bool> BOOLS = {{"true", true}, {"false", false}};
static const std::map<std::string,CsvExporter::VectorLayout> VECTORLAYOUTS = {{"horizontal", CsvExporter::HORIZONTAL}, {"vertical", CsvExporter::VERTICAL}, {"vertical-joined", CsvExporter::JOINED_VERTICAL}};

class CsvExporterType : public ExporterType
{
    public:
        virtual std::string getFormatName() const {return "CSV";}
        virtual std::string getDisplayName() const {return "CSV";}
        virtual std::string getDescription() const {return "Export results in CSV format.";}
        virtual int getSupportedResultTypes() {return ResultFileManager::SCALAR | ResultFileManager::VECTOR | ResultFileManager::HISTOGRAM;}
        virtual std::string getFileExtension() {return "csv"; }
        virtual StringMap getSupportedOptions() const;
        virtual std::string getXswtForm() const;
        virtual Exporter *create() const {return new CsvExporter();}
};

string CsvExporterType::getXswtForm() const
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
            "    <!-- allowMixed TODO -->\n"
            "    <group text='CSV Options'>\n"
            "      <layoutData x:class='GridData' horizontalSpan='2' horizontalAlignment='FILL' grabExcessHorizontalSpace='true'/>\n"
            "      <layout x:class='GridLayout' numColumns='2'/>\n"
            "      <x:children>\n"
            "         <label text='Scalar arrangement:'/>\n"
            "         <combo x:id='scalarsGroupBy' x:style='BORDER|READ_ONLY'>\n"
            "           <add x:p0=''/>\n"
            "           <add x:p0='name'/>\n"
            "           <add x:p0='module,name'/>\n"
            "           <add x:p0='module'/>\n"
            "           <text x:p0='name'/>\n"
            "         </combo>\n"
            "         <label text='Vector arrangement:'/>\n"
            "         <combo x:id='vectorLayout' x:style='BORDER|READ_ONLY'>\n"
            "           <add x:p0='horizontal'/>\n"
            "           <add x:p0='vertical'/>\n"
            "           <add x:p0='vertical-joined'/>\n"
            "           <text x:p0='vertical'/>\n"
            "         </combo>\n"
            "         <button x:id='allowMixed' text='Allow mixed content (i.e. scalars + vectors)' x:style='CHECK' selection='true'>\n"
            "           <layoutData x:class='GridData' horizontalSpan='2'/>\n"
            "         </button>\n"
            "         <button x:id='columnNames' text='Add header row' x:style='CHECK' selection='true'>\n"
            "           <layoutData x:class='GridData' horizontalSpan='2'/>\n"
            "         </button>\n"
            "         <label text='Data precision:'/>\n"
            "         <spinner x:id='precision' x:style='BORDER' minimum='1' maximum='16' textLimit='2' selection='14'>\n"
            "           <layoutData x:class='GridData' widthHint='100'/>\n"
            "         </spinner>\n"
            "         <label text='CSV separator:'/>\n"
            "         <combo x:id='separator' x:style='BORDER|READ_ONLY'>\n"
            "           <add x:p0='tab'/>\n"
            "           <add x:p0='comma'/>\n"
            "           <add x:p0='semicolon'/>\n"
            "           <add x:p0='colon'/>\n"
            "           <text x:p0='comma'/>\n"
            "         </combo>\n"
            "         <label text='CSV quoting:'/>\n"
            "         <combo x:id='quoteChar' x:style='BORDER|READ_ONLY'>\n"
            "           <add x:p0='doublequote'/>\n"
            "           <add x:p0='singlequote'/>\n"
            "           <text x:p0='doublequote'/>\n"
            "         </combo>\n"
            "         <label text='CSV quote escaping:'/>\n"
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

StringMap CsvExporterType::getSupportedOptions() const
{
    StringMap options {
        {"precision", "The number of significant digits for floating-point values (double). The maximum value is ~15."},
        {"columnNames", "Whether to print a row containing column names. Values: 'true', 'false'"},
        {"allowMixed", "Whether to allow mixed content (e.g. vectors and scalars in the same file). Values: 'true', 'false'"},
        {"separator", "Separator character. Values: 'tab', 'comma', 'semicolon', 'colon'"},
        {"quoteChar", "Quote character. Values: 'doublequote', 'singlequote'"},
        {"quoteEscaping", "How to escape the quote character within quoted values. Values: 'doubling', 'backslash'"},
        {"scalarsGroupBy", "Group scalars by the given fields. Accepts a comma-separated list of field names: 'file', 'run', 'module', 'name', etc."},
        {"vectorFilters", "A semicolon-separated list of operations to be applied to the vectors to be exported. See the 'operations' help page. Example value: 'winavg(10);mean'"},
        {"vectorLayout", "The layout (format) in which vectors are exported. Values: 'horizontal','vertical', 'vertical-joined'"},
    };
    return options;
}

//---

ExporterType *CsvExporter::getDescription()
{
    static CsvExporterType desc;
    return &desc;
}

void CsvExporter::setOption(const std::string& key, const std::string& value)
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
    else if (key == "scalarsGroupBy")
        setScalarsGroupBy(ResultItemFields(StringTokenizer(value.c_str(),",").asVector()));
    else if (key == "vectorFilters")
        setVectorFilters(StringTokenizer(value.c_str(), ";").asVector());
    else if (key == "vectorLayout")
        setVectorLayout(translateOptionValue(VECTORLAYOUTS,value));
    else
        throw opp_runtime_error("Exporter: unhandled option '%s'", key.c_str());
}

void CsvExporter::saveResults(const std::string& fileName, ResultFileManager *manager, const IDList& idlist, IProgressMonitor *monitor)
{
    int itemTypes = idlist.getItemTypes();
    bool mixedContent = itemTypes != 0 && itemTypes != ResultFileManager::SCALAR &&
            itemTypes != ResultFileManager::VECTOR && itemTypes != ResultFileManager::HISTOGRAM;

    if (mixedContent && !allowMixedContent)
        throw opp_runtime_error("CSV exporter: homogeneous results expected (all scalars, all vectors, "
                "or all histograms); set allowMixed=true to allow mixed content");

    if (fileName == "-")
        csv.setOut(std::cout);
    else
        csv.open(fileName.c_str());

    if ((itemTypes & ResultFileManager::SCALAR) != 0) {
        if (mixedContent) {csv.writeString("*** SCALARS ***"); csv.writeNewLine();}
        saveScalars(manager, idlist.filterByTypes(ResultFileManager::SCALAR), monitor);
    }
    if ((itemTypes & ResultFileManager::VECTOR) != 0) {
        if (mixedContent) {csv.writeString("*** VECTORS ***"); csv.writeNewLine();}
        saveVectors(manager, idlist.filterByTypes(ResultFileManager::VECTOR), monitor);
    }
    if ((itemTypes & ResultFileManager::HISTOGRAM) != 0) {
        if (mixedContent) {csv.writeString("*** HISTOGRAMS ***"); csv.writeNewLine();}
        saveHistograms(manager, idlist.filterByTypes(ResultFileManager::HISTOGRAM), monitor);
    }

    if (fileName != "-")
        csv.close();
}

void CsvExporter::saveScalars(ResultFileManager *manager, const IDList& idlist, IProgressMonitor *monitor)
{
    //TODO use monitor
    const ScalarDataTable table("unused", "unused", idlist, scalarsGroupBy.complement(), *manager);
    saveTable(csv, table, columnNames);
}

void CsvExporter::saveVectors(ResultFileManager *manager, const IDList& idlist, IProgressMonitor *monitor)
{
    //TODO use monitor
    std::vector<XYArray *> xyArrays = readVectorsIntoArrays(manager, idlist, vectorFilters);
    assert((int)xyArrays.size() == idlist.size());

    int numVectors = (int)idlist.size();

    if (vectorLayout == HORIZONTAL) {
        // write header row
        if (columnNames) {
            csv.writeString("Run");
            csv.writeString("Module");
            csv.writeString("Name");
            csv.writeString("Row type");
            csv.writeString("Data...");
            csv.writeNewLine();
        }

        // write vectors
        for (int i = 0; i < numVectors; ++i) {
            const VectorResult& vector = manager->getVector(idlist.get(i));
            XYArray *data = xyArrays[i];

            // time row
            csv.writeString(vector.fileRunRef->runRef->runName); //TODO
            csv.writeString(*vector.moduleNameRef);
            csv.writeString(*vector.nameRef);
            csv.writeString("TIME");
            for (int j = 0; j < data->length(); j++) {
                if (data->hasPreciseX())
                    csv.writeBigDecimal(data->getPreciseX(j));
                else
                    csv.writeDouble(data->getX(j));
            }
            csv.writeNewLine();

            // value row
            csv.writeString(vector.fileRunRef->runRef->runName); //TODO
            csv.writeString(*vector.moduleNameRef);
            csv.writeString(*vector.nameRef);
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
                const VectorResult& vector = manager->getVector(idlist.get(i));
                std::string columnTitle = *vector.nameRef + " " + *vector.moduleNameRef + " (" + vector.fileRunRef->runRef->runName + ")"; // trying to come up with something usable when viewed from spreadsheets; should be customizable via a format string
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
    else if (vectorLayout == JOINED_VERTICAL) {
        // collect vectors
        vector<DataTable *> tables;
        for (int i = 0; i < numVectors; ++i) {
            const VectorResult& vector = manager->getVector(idlist.get(i));
            std::string yColumnName = *vector.moduleNameRef + "." + *vector.nameRef;
            tables.push_back(new XYDataTable("unused", "unused", "t", yColumnName, xyArrays[i]));
        }

        // join them on the time column
        JoinedDataTable table("unused", "unused", tables, 0);

        // save the result
        saveTable(csv, table, columnNames);
    }

    for (auto xyArray : xyArrays)
        delete xyArray;
}

void CsvExporter::saveHistograms(ResultFileManager *manager, const IDList& idlist, IProgressMonitor *monitor)
{
    //TODO use monitor

    // header
    if (columnNames) {
        csv.writeString("run");
        csv.writeString("module");
        csv.writeString("name");
        csv.writeString("row type");
        csv.writeString("data...");
        csv.writeNewLine();
    }

    // write histograms, two lines each ("bins" and "counters" lines)
    for (int i = 0; i < (int)idlist.size(); ++i) {
        const HistogramResult& histogram = manager->getHistogram(idlist.get(i));
        csv.writeString(histogram.fileRunRef->runRef->runName);
        csv.writeString(*histogram.moduleNameRef);
        csv.writeString(*histogram.nameRef);
        csv.writeString("bins");
        for (double d : histogram.bins)
            csv.writeDouble(d);
        csv.writeNewLine();

        csv.writeString(histogram.fileRunRef->runRef->runName);
        csv.writeString(*histogram.moduleNameRef);
        csv.writeString(*histogram.nameRef);
        csv.writeString("counters");
        for (double d : histogram.values)
            csv.writeDouble(d);
        csv.writeNewLine();
    }
}

}  // namespace scave
}  // namespace omnetpp

