//=========================================================================
//  CSVSPREADEXPORTER.H - part of
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

#ifndef __OMNETPP_SCAVE_CSVSPREADEXPORTER_H
#define __OMNETPP_SCAVE_CSVSPREADEXPORTER_H

#include "exporter.h"
#include "common/csvwriter.h"

namespace omnetpp {
namespace scave {

using common::CsvWriter;

/**
 * Export data in various CSV formats.
 * With default parameters the format is the same as described in RFC 4180.
 */
class SCAVE_API CsvForSpreadsheetExporter : public Exporter
{
    public:
        enum VectorLayout {HORIZONTAL, VERTICAL};

    private:
        CsvWriter csv;
        bool columnNames = true;
        bool allowMixedContent = false;
        VectorLayout vectorLayout = VERTICAL;

        std::vector<std::string> itervarNames;

    public:
        CsvForSpreadsheetExporter() {}

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
        void setMixedContentAllowed(bool b) {allowMixedContent = b;}
        bool getMixedContentAllowed() const {return allowMixedContent;}
        void setVectorLayout(VectorLayout d) {vectorLayout = d;}
        VectorLayout getVectorLayout() {return vectorLayout;}

        virtual void setOption(const std::string& key, const std::string& value);
        virtual void saveResults(const std::string& fileName, ResultFileManager *manager, const IDList& idlist, IProgressMonitor *monitor=nullptr);

        static ExporterType *getDescription();

    private:
        void saveScalars(ResultFileManager *manager, const IDList& idlist, IProgressMonitor *monitor);
        void saveParameters(ResultFileManager *manager, const IDList& idlist, IProgressMonitor *monitor);
        void saveVectors(ResultFileManager *manager, const IDList& idlist, IProgressMonitor *monitor);
        void saveStatistics(ResultFileManager *manager, const IDList& idlist, IProgressMonitor *monitor);
        void saveHistograms(ResultFileManager *manager, const IDList& idlist, IProgressMonitor *monitor);
        void collectItervars(ResultFileManager *manager, const IDList& idlist);
        void writeRunColumnNames();
        void writeRunColumns(Run *run);
        std::string makeRunTag(Run *run);
};

}  // namespace scave
}  // namespace omnetpp

#endif


