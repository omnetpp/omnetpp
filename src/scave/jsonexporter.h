//=========================================================================
//  JSONEXPORTER.H - part of
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

#ifndef __OMNETPP_SCAVE_JSONEXPORTER_H
#define __OMNETPP_SCAVE_JSONEXPORTER_H

#include <string>
#include <vector>

#include "common/jsonwriter.h"
#include "exporter.h"
#include "idlist.h"

namespace omnetpp {
namespace scave {

using common::JsonWriter;

/**
 * Export data in JSON format. The exported data structure closely corresponds
 * to the structure of OMNeT++ result files. Python-flavoured JSON that can be
 * "parsed" with a simple eval() call can also be exported.
 */
class SCAVE_API JsonExporter : public Exporter
{
    private:
        JsonWriter writer;
        bool pythonFlavoured = false;
        bool useNumpy = true;
        bool skipResultAttributes = false;

    protected:
        void writeStringMap(const std::string& key, const StringMap& attrs);
        void writeOrderedKeyValueList(const std::string& key, const OrderedKeyValueList& list);
        void writeStatisticsFields(const Statistics& stat);
        void writeVector(const std::vector<double>& v);
        void writeX(XYArray *array);
        void writeY(XYArray *array);
        void writeEventNumbers(XYArray *array);
        void writeVectorProlog();
        void writeVectorEpilog();

    public:
        JsonExporter() {}

        void setPrecision(int prec) {writer.setPrecision(prec);}
        int getPrecision() const {return writer.getPrecision();}
        void setPythonFlavoured(bool b) {pythonFlavoured = b;}
        bool isPythonFlavoured() const {return pythonFlavoured;}
        void setUseNumpy(bool b) {useNumpy = b;}
        bool getUseNumpy() const {return useNumpy;}
        void setIndentSize(int n) {writer.setIndentSize(n);}
        int getIndentSize() const {return writer.getIndentSize();}
        void setSkipResultAttributes(bool b) {skipResultAttributes = b;}
        bool getSkipResultAttributes() const {return skipResultAttributes;}

        virtual void setOption(const std::string& key, const std::string& value);
        virtual void saveResults(const std::string& fileName, ResultFileManager *manager, const IDList& idlist, IProgressMonitor *monitor=nullptr);

        static ExporterType *getDescription();
};

}  // namespace scave
}  // namespace omnetpp

#endif


