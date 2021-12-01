//==========================================================================
//  EXPORTER.CC - part of
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

#include "common/stringutil.h"
#include "common/stlutil.h"
#include "exporter.h"

#include "csvrecexporter.h"
#include "csvspreadexporter.h"
#include "jsonexporter.h"
#include "omnetppscalarfileexporter.h"
#include "omnetppvectorfileexporter.h"
#include "sqlitescalarfileexporter.h"
#include "sqlitevectorfileexporter.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace scave {

void Exporter::checkOptionKey(ExporterType *desc, const std::string& key)
{
    const StringMap& options = desc->getSupportedOptions();
    if (options.find(key) == options.end())
        throw opp_runtime_error("Exporter: unknown option \"%s\", accepted ones are: %s", key.c_str(), opp_join(keys(options), ", ", '"').c_str());
}

void Exporter::setOptions(const StringMap& options)
{
    for (auto pair : options)
        setOption(pair.first, pair.second);
}

void Exporter::checkItemTypes(const IDList& idlist, int supportedTypes)
{
    // check items are supported by the format
    int itemTypes = idlist.getItemTypes();
    int unsupportedItemTypes = itemTypes & ~supportedTypes;
    if (unsupportedItemTypes != 0)
        throw opp_runtime_error("Data set contains items of type not supported by the export format");
}

//----

static std::vector<ExporterType*> exporters;

void ExporterFactory::ensureInitialized()
{
    if (exporters.empty()) {
        exporters.push_back(CsvRecordsExporter::getDescription());  // IMPORTANT: this must precede CsvForSpreadsheetExporter so .csv resolves to this one
        exporters.push_back(CsvForSpreadsheetExporter::getDescription());
        exporters.push_back(JsonExporter::getDescription());
        exporters.push_back(OmnetppScalarFileExporter::getDescription());
        exporters.push_back(OmnetppVectorFileExporter::getDescription());
        exporters.push_back(SqliteScalarFileExporter::getDescription());
        exporters.push_back(SqliteVectorFileExporter::getDescription());
    }
}

StringVector ExporterFactory::getSupportedFormats()
{
    ensureInitialized();
    StringVector result;
    for (ExporterType *exporter : exporters)
        result.push_back(exporter->getFormatName());
    return result;
}

ExporterType *ExporterFactory::getByFormat(const std::string& format)
{
    ensureInitialized();
    for (ExporterType *exporter : exporters)
        if (exporter->getFormatName() == format)
            return exporter;
    return nullptr;
}

std::string ExporterFactory::getFormatFromOutputFileName(const std::string& fileName)
{
    //TODO obey PREFER_SQLITE option (for sca and vec extensions)
    ensureInitialized();
    std::string extension = opp_substringafterlast(fileName, ".");
    for (ExporterType *exporter : exporters)
        if (extension == exporter->getFileExtension())
            return exporter->getFormatName();
    return "";
}

Exporter *ExporterFactory::createExporter(const std::string& format)
{
    ExporterType *e = getByFormat(format);
    return e ? e->create() : nullptr;
}

}  // namespace scave
}  // namespace omnetpp

