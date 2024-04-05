//=========================================================================
//  INDEXFILEWRITER.CC - part of
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
#include "indexfilewriter.h"
#include "vectorfileindex.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace scave {

using VectorInfo = VectorFileIndex::VectorInfo;
using Block = VectorFileIndex::Block;

#ifdef CHECK
#undef CHECK
#endif
#define CHECK(fprintf)    if ((fprintf)<0) throw opp_runtime_error("Cannot write output file '%s'", filename.c_str())
#define INDEX_FILE_VERSION 3

IndexFileWriter::IndexFileWriter(const char *filename, int precision)
    : filename(filename), precision(precision), file(nullptr)
{
}

IndexFileWriter::~IndexFileWriter()
{
    if (file != nullptr) {
        closeFile();
    }
}

void IndexFileWriter::writeAll(const VectorFileIndex& index)
{
    openFile();
    writeFingerprint(index.vectorFileName);
    writeRun(index.run);

    int numOfVectors = index.getNumberOfVectors();
    for (int i = 0; i < numOfVectors; ++i) {
        const VectorInfo *vectorRef = index.getVectorAt(i);
        writeVector(*vectorRef);
    }

    closeFile();
}

void IndexFileWriter::writeFingerprint(std::string vectorFileName)
{
    FileFingerprint fingerprint = readFileFingerprint(vectorFileName.c_str());

    if (file == nullptr)
        openFile();

    file_offset_t saveOffset = opp_ftell(file);
    opp_fseek(file, 0, SEEK_SET);
    CHECK(fprintf(file, "file %" PRId64 " %" PRId64, fingerprint.fileSize, fingerprint.lastModified));
    opp_fseek(file, saveOffset, SEEK_SET);
}

void IndexFileWriter::writeRun(const RunData& run)
{
    if (file == nullptr)
        openFile();
    run.writeToFile(file, filename.c_str());
}

void IndexFileWriter::writeVector(const VectorInfo& vector)
{
    if (file == nullptr)
        openFile();

    int numBlocks = vector.blocks.size();
    if (numBlocks > 0) {
        writeVectorDeclaration(vector);
        writeVectorAttributes(vector);

        for (int i = 0; i < numBlocks; i++) {
            writeBlock(vector, vector.blocks[i]);
        }
    }
}

void IndexFileWriter::writeVectorDeclaration(const VectorInfo& vector)
{
    CHECK(fprintf(file, "vector %d %s %s %s\n",
                    vector.vectorId, QUOTE(vector.moduleName.c_str()), QUOTE(vector.name.c_str()), vector.columns.c_str()));
}

void IndexFileWriter::writeVectorAttributes(const VectorInfo& vector)
{
    for (StringMap::const_iterator it = vector.attributes.begin(); it != vector.attributes.end(); ++it) {
        CHECK(fprintf(file, "attr %s %s\n", QUOTE(it->first.c_str()), QUOTE(it->second.c_str())));
    }
}

void IndexFileWriter::writeBlock(const VectorInfo& vector, const Block *block)
{
    static char buff1[64], buff2[64];
    char *e;

    if (block->getCount() > 0) {
        CHECK(fprintf(file, "%d\t%" PRId64 " %" PRId64, vector.vectorId, (int64_t)block->startOffset, (int64_t)block->size));
        if (vector.hasColumn('E')) {
            CHECK(fprintf(file, " %" PRId64 " %" PRId64, block->startEventNum, block->endEventNum));
        }
        if (vector.hasColumn('T')) {
            CHECK(fprintf(file, " %s %s",
                            BigDecimal::ttoa(buff1, block->startTime, e),
                            BigDecimal::ttoa(buff2, block->endTime, e)));
        }
        if (vector.hasColumn('V')) {
            const Statistics& stat = block->stat;
            CHECK(fprintf(file, " %ld %.*g %.*g %.*g %.*g",
                            block->getCount(), precision, stat.getMin(), precision, stat.getMax(),
                            precision, stat.getSum(), precision, stat.getSumSqr()));
        }
        CHECK(fprintf(file, "\n"));
    }
}

void IndexFileWriter::openFile()
{
    file = fopen(filename.c_str(), "w");
    if (file == nullptr)
        throw opp_runtime_error("Cannot open index file: %s", filename.c_str());

    setlocale(LC_NUMERIC, "C");

    // space for header
    CHECK(fprintf(file, "%64s\n", ""));
    // version
    CHECK(fprintf(file, "version %d\n", INDEX_FILE_VERSION));
}

void IndexFileWriter::closeFile()
{
    if (file != nullptr) {
        fclose(file);
        file = nullptr;
    }
}

}  // namespace scave
}  // namespace omnetpp

