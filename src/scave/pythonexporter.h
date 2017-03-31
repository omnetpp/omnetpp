//=========================================================================
//  PYTHONEXPORTER.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_SCAVE_PYTHONEXPORTER_H
#define __OMNETPP_SCAVE_PYTHONEXPORTER_H

#include <string>
#include <vector>

#include "common/jsonwriter.h"
#include "exporter.h"
#include "idlist.h"

namespace omnetpp {
namespace scave {

using common::JsonWriter;

/**
 * Export data in Python source file format. The generated code
 * defines a variable that contains the data in a JSON-like
 * data structure that closely corresponds to the structure
 * of OMNeT++ result files.
 */
class SCAVE_API PythonExporter : public Exporter
{
    private:
        JsonWriter writer;
        std::vector<std::string> vectorFilters;
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
        PythonExporter() {writer.setNakedNan(true);}

        void setPrecision(int prec) {writer.setPrecision(prec);}
        int getPrecision() const {return writer.getPrecision();}
        void setUseNumpy(bool b) {useNumpy = b;}
        bool getUseNumpy() const {return useNumpy;}
        void setNakedNan(bool b) {writer.setNakedNan(b);}
        bool getNakedNan() const {return writer.getNakedNan();}
        void setIndentSize(int n) {writer.setIndentSize(n);}
        int getIndentSize() const {return writer.getIndentSize();}
        void setSkipResultAttributes(bool b) {skipResultAttributes = b;}
        bool getSkipResultAttributes() const {return skipResultAttributes;}
        void setVectorFilters(const std::vector<std::string>& filters) {vectorFilters = filters;}
        const std::vector<std::string>& getVectorFilters() const {return vectorFilters;}

        virtual void setOption(const std::string& key, const std::string& value);
        virtual void saveResults(const std::string& fileName, ResultFileManager *manager, const IDList& idlist, IProgressMonitor *monitor=nullptr);

        static ExporterType *getDescription();
};

} // namespace scave
}  // namespace omnetpp

#endif


