//=========================================================================
//  EXPORT.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Tamas Borbely
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_SCAVE_EXPORT_H
#define __OMNETPP_SCAVE_EXPORT_H

#include <string>
#include <vector>
#include <set>
#include <fstream>
#include <utility>
#include "common/bigdecimal.h"
#include "xyarray.h"
#include "resultfilemanager.h"
#include "datasorter.h"
#include "datatable.h"

namespace omnetpp {
namespace scave {

/**
 * Base class for scalar/vector exporters.
 */
class SCAVE_API ScaveExport
{
    private:
        std::set<std::string> identifiers;

    protected:
        std::string baseFileName;
        std::string fileName;
        std::ofstream out;
        int prec;

    protected:
        void open();
        void close();
        virtual std::string makeFileName(const std::string& name) = 0;

    public:
        ScaveExport() : prec(DEFAULT_RESULT_PRECISION) {}
        virtual ~ScaveExport();

        void setPrecision(int prec) { this->prec = prec; }
        void setBaseFileName(const std::string& baseFileName) { this->baseFileName = baseFileName; }

        virtual void saveVector(const std::string& name, const std::string& description,
                        ID vectorID, bool computed, const XYArray *vec, ResultFileManager& manager,
                        int startIndex=0, int endIndex=-1);
        virtual void saveVectors(const std::string& name, const std::string& description,
                                     const IDList& vectors, const std::vector<XYArray*> xyarrays,
                                     const ResultFileManager& manager);
        virtual void saveScalars(const std::string& name, const std::string& description,
                                    const IDList& scalars, const ResultItemFields& groupBy, ResultFileManager& manager);

        virtual void saveScalars(const std::string& name, const std::string& description,
                                    const IDList& scalars, const std::string& moduleName, const std::string& scalarName,
                                    const ResultItemFields& columnFields,
                                    const std::vector<std::string>& isoModuleNames, const StringVector& isoScalarNames,
                                    const ResultItemFields& isoFields, ResultFileManager& manager);

        virtual void saveHistograms(const std::string& name, const std::string& description,
                                    const IDList& histograms, ResultFileManager& manager);

        const std::string& getLastFileName() const { return fileName; }
    protected:
        std::string makeUniqueIdentifier(const std::string& name);
        virtual std::string makeIdentifier(const std::string& name) = 0;
        virtual void saveTable(const DataTable& table, int startIndex, int endIndex) = 0;
};

/**
 * Common base class for formats that creates a matlab structure when loaded.
 */
class MatlabStructExport : public ScaveExport
{
    protected:
        void writeDouble(double value);
        virtual std::string makeIdentifier(const std::string& name) override;
        static std::string quoteString(const std::string& str);
};

/**
 * Exports data as a Matlab script.
 *
 * The script defines a structure containing a description field,
 * and a column fields for each column. The column fields are
 * equal length vectors.
 *
 * Note: Although BigDecimal values (e.g. simulation time) are
 * written out with the specified precision, Matlab reads them as IEEE
 * double (max 15 digit), so their precision gets lost.
 *
 * FIXME: BigDecimals always written out with maximal precision (19 digit), prec is ignored.
 */
class SCAVE_API MatlabScriptExport : public MatlabStructExport
{
    protected:
        virtual std::string makeFileName(const std::string& name) override;
        virtual void saveTable(const DataTable& table, int startRow, int endRow) override;
    private:
        void writeDescriptionField(const DataTable& rows, const std::string& tableName);
        void writeColumnFields(const DataTable& rows, int startRow, int endRow, const std::string& tableName);
        void writeDoubleColumn(const DataTable& table, int col, int startRow, int endRow);
        void writeBigDecimalColumn(const DataTable& table, int col, int startRow, int endRow);
        void writeStringColumn(const DataTable& table, int col, int startRow, int endRow);
};

/**
 * Exports data in Octave's "save -text" format. This format can be loaded
 * into R (r-project.org) as well.
 *
 * To see the an example, type the following commands in Octave:
 * <pre>
 *    a = [1 2; 3 4; 5 6]
 *    save -text - a
 * </pre>
 *
 * The output:
 * <pre>
 *  # Created by Octave 2.1.73 ...
 *  # name: a
 *  # type: matrix
 *  # rows: 3
 *  # columns: 2
 *   1 2
 *   3 4
 *   5 6
 * </pre>
 *
 * If several variables are saved, the above pattern repeats.
 *
 * FIXME: BigDecimals always written out with maximal precision (19 digit), prec is ignored.
 */
class SCAVE_API OctaveTextExport : public MatlabStructExport
{
    protected:
        virtual std::string makeFileName(const std::string& name) override;
        virtual void saveTable(const DataTable& table, int startRow, int endRow) override;
    private:
        void writeStructHeader(const DataTable& table);
        void writeDescriptionField(const DataTable& table);
        void writeColumnFields(const DataTable& table, int startRow, int endRow);
        void writeDoubleColumn(const DataTable& table, int col, int startRow, int endRow);
        void writeBigDecimalColumn(const DataTable& table, int col, int startRow, int endRow);
        void writeStringColumn(const DataTable& table, int col, int startRow, int endRow);
};

/**
 * Export data in various CSV formats.
 * With default parameters the format is the same as described in RFC 4180.
 */
class SCAVE_API CsvExport : public ScaveExport
{
    public: // options
        typedef enum {DOUBLE, ESCAPE} QuoteMethod;
        char separator;
        char quoteChar;
        std::string eol;
        QuoteMethod quoteMethod;
        bool columnNames;
        // state
        int fileNameSuffix; // zero = do not add suffix, otherwise incremented when writing a new table
    public:
        CsvExport() : separator(','), quoteChar('"'), eol("\r\n"), quoteMethod(DOUBLE),
                        columnNames(true), fileNameSuffix(0) {}
        virtual void saveVector(const std::string& name, const std::string& description,
                        ID vectorID, bool computed, const XYArray *vec, ResultFileManager& manager,
                        int startIndex=0, int endIndex=-1) override;
    protected:
        virtual std::string makeFileName(const std::string& name) override;
        virtual std::string makeIdentifier(const std::string& name) override;
        virtual void saveTable(const DataTable& table, int startRow, int endRow) override;
    private:
        void writeHeader(const DataTable& table);
        void writeRow(const DataTable& table, int row);
        void writeDouble(double value);
        void writeBigDecimal(BigDecimal value);
        void writeString(const std::string& value);
        bool needsQuote(const std::string& value);
        void writeChar(char ch);
};

class SCAVE_API ExporterFactory
{
    public:
        static ScaveExport *createExporter(const std::string& format);
};

} // namespace scave
}  // namespace omnetpp


#endif


