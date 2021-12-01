//=========================================================================
//  INDEXFILEUTILS.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <sys/stat.h>
#include <cstdint>
#include <algorithm>
#include <clocale>
#include "common/exception.h"
#include "common/filereader.h"
#include "common/linetokenizer.h"
#include "common/stringutil.h"
#include "scaveutils.h"
#include "scaveexception.h"
#include "indexfileutils.h"
#include "indexfilereader.h"
#include "vectorfileindex.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace scave {

static bool isFileReadable(const char *filename)
{
    FILE *f = fopen(filename, "r");
    if (f != nullptr)
        fclose(f);
    return f != nullptr;
}

bool IndexFileUtils::isIndexFile(const char *filename)
{
    return opp_stringendswith(filename, ".vci");
}

bool IndexFileUtils::isExistingVectorFile(const char *filename)
{
    if (!opp_stringendswith(filename, ".vec"))
        return false;

    FILE *f = fopen(filename, "r");
    if (!f)
        return false;

    char buf[20] = "";
    fgets(buf, 20, f);
    fclose(f);
    std::string trimmed = opp_trim(buf);
    return trimmed == "version 2" || trimmed == "version 3";
}

std::string IndexFileUtils::getVectorFileName(const char *filename)
{
    std::string vectorFileName(filename);
    std::string::size_type pos = vectorFileName.rfind('.');
    if (pos != std::string::npos)
        vectorFileName.replace(vectorFileName.begin()+pos, vectorFileName.end(), ".vec");
    else
        vectorFileName.append(".vec");
    return vectorFileName;
}

std::string IndexFileUtils::getIndexFileName(const char *filename)
{
    std::string indexFileName(filename);
    std::string::size_type pos = indexFileName.rfind('.');
    if (pos != std::string::npos)
        indexFileName.replace(indexFileName.begin()+pos, indexFileName.end(), ".vci");
    else
        indexFileName.append(".vci");
    return indexFileName;
}

bool IndexFileUtils::isIndexFileUpToDate(const char *filename)
{
    std::string indexFileName, vectorFileName;

    if (isIndexFile(filename)) {
        indexFileName = std::string(filename);
        vectorFileName = getVectorFileName(filename);
    }
    else {
        indexFileName = getIndexFileName(filename);
        vectorFileName = std::string(filename);
    }

    if (!isFileReadable(indexFileName.c_str()))
        return false;

    IndexFileReader reader(indexFileName.c_str());
    FileFingerprint fingerprint = reader.readRecordedFingerprint();

    // when the fingerprint not found assume the index file is being written therefore it is up to date
    if (fingerprint.isEmpty())
        return true;

    return fingerprint == readFileFingerprint(vectorFileName.c_str());
}

}  // namespace scave
}  // namespace omnetpp

