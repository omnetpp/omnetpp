//=========================================================================
//  CSVRECEXPORTER.H - part of
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

#ifndef __OMNETPP_SCAVE_CSVRECEXPORTER_H
#define __OMNETPP_SCAVE_CSVRECEXPORTER_H

#include "exporter.h"
#include "common/csvwriter.h"

namespace omnetpp {
namespace scave {

using common::CsvWriter;

/**
 * Export data in various CSV formats.
 * With default parameters the format is the same as described in RFC 4180.
 */
class SCAVE_API CsvRecordsExporter : public Exporter
{
    private:
        CsvWriter csv;
        bool columnNames = true;
        bool omitBlankColumns = true;

    public:
        CsvRecordsExporter() {}

        void setPrecision(int prec) {csv.setPrecision(prec);}
        int getPrecision() const {return csv.getPrecision();}
        void setSeparator(char sep) {csv.setSeparator(sep);}
        char getSeparator() const {return csv.getSeparator();}
        void setQuoteChar(char quoteChar) {csv.setQuoteChar(quoteChar);}
        char getQuoteChar() const {return csv.getQuoteChar();}
        void setQuoteEscapingMethod(CsvWriter::QuoteEscapingMethod q) {csv.setQuoteEscapingMethod(q);}
        CsvWriter::QuoteEscapingMethod getQuoteEscapingMethod() const {return csv.getQuoteEscapingMethod();}
        void setColumnNames(bool b) {columnNames = b;}
        bool getColumnNames() {return columnNames;}
        void setOmitBlankColumns(bool b) {omitBlankColumns = b;}
        bool getOmitBlankColumns() const {return omitBlankColumns;}

        virtual void setOption(const std::string& key, const std::string& value);
        virtual void saveResults(const std::string& fileName, ResultFileManager *manager, const IDList& idlist, IProgressMonitor *monitor=nullptr);

        static ExporterType *getDescription();

    protected:
        virtual void saveResultsAsRecords(ResultFileManager *manager, const IDList& idlist, IProgressMonitor *monitor);
        virtual void writeRunAttrRecord(const std::string& runId, const char *type, const std::string& attrName, const std::string& value, int numColumns);
        virtual void writeResultAttrRecords(const ResultItem *result, int numColumns);
        virtual void writeResultItemBase(const ResultItem *result, const char *type, int numColumns);
        virtual void writeAsString(const std::vector<double>& data);
        virtual void writeXAsString(const XYArray *data);
        virtual void writeYAsString(const XYArray *data);
        virtual void finishRecord(int numColumns);
};

}  // namespace scave
}  // namespace omnetpp

#endif


