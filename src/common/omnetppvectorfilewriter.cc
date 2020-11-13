//==========================================================================
//  OMNETPPVECTORFILEWRITER.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Authors: Andras Varga, Tamas Borbely
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2016 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <algorithm>
#include "commonutil.h"
#include "stringutil.h"
#include "omnetppvectorfilewriter.h"


namespace omnetpp {
namespace common {

#define VECTOR_FILE_VERSION    3
#define INDEX_FILE_VERSION     3

using std::ostream;
using std::ofstream;
using std::ios;

OmnetppVectorFileWriter::~OmnetppVectorFileWriter()
{
    cleanup(); // not close() because it throws; also, close() must have been called already if there was no error
}

void OmnetppVectorFileWriter::check(int fprintfResult)
{
    if (fprintfResult < 0) {
        close();
        throw opp_runtime_error("Cannot write output vector file '%s'", fname.c_str());
    }
}

void OmnetppVectorFileWriter::checki(int fprintfResult)
{
    if (fprintfResult < 0) {
        close();
        throw opp_runtime_error("Cannot write output vector index file '%s'", ifname.c_str());
    }
}

void OmnetppVectorFileWriter::open(const char *filename)
{
    // open file
    fname = filename;
    f = fopen(fname.c_str(), "w");  // we only support overwrite but not append
    if (f == nullptr)
        throw opp_runtime_error("Cannot open output vector file '%s'", fname.c_str());
    check(fprintf(f, "version %d\n", VECTOR_FILE_VERSION));

    // open index file
    ifname = opp_substringbeforelast(fname, ".") + ".vci";
    fi = fopen(ifname.c_str(), "w");
    if (fi == nullptr)
        throw opp_runtime_error("Cannot open index file '%s'", ifname.c_str());

    fprintf(fi, "%64s\n", "");  // leave blank space for "fingerprint" (size and modification date of the vector file)
    check(fprintf(fi, "version %d\n", INDEX_FILE_VERSION));
}

void OmnetppVectorFileWriter::close()
{
    if (f) {
        fclose(f);
        f = nullptr;
    }

    if (fi) {
        // write out fingerprint (size and modification date of the vector file)
        struct opp_stat_t s;
        if (opp_stat(fname.c_str(), &s) == 0) {
            opp_fseek(fi, 0, SEEK_SET);
            fprintf(fi, "file %" PRId64 " %" PRId64, (int64_t)s.st_size, (int64_t)s.st_mtime);
        }

        fclose(fi);
        fi = nullptr;
    }

}

void OmnetppVectorFileWriter::cleanup()  // MUST NOT THROW
{
    if (f)
        fclose(f);
    if (fi)
        fclose(fi);
}

void OmnetppVectorFileWriter::beginRecordingForRun(const std::string& runName, const StringMap& attributes, const StringMap& itervars, const OrderedKeyValueList& configEntries)
{
    Assert(vectors.size() == 0);
    bufferedSamples = 0;
    Assert(isOpen());

    // note: we write everything twice, once in .vec and once in .vci

    // save run
    check(fprintf(f, "run %s\n", QUOTE(runName.c_str())));
    checki(fprintf(fi, "run %s\n", QUOTE(runName.c_str())));

    // save run attributes
    for (auto& pair : attributes) {
        check(fprintf(f, "attr %s %s\n", QUOTE(pair.first.c_str()), QUOTE(pair.second.c_str())));
        checki(fprintf(fi, "attr %s %s\n", QUOTE(pair.first.c_str()), QUOTE(pair.second.c_str())));
    }

    // save itervars
    for (auto& pair : itervars) {
        check(fprintf(f, "itervar %s %s\n", QUOTE(pair.first.c_str()), QUOTE(pair.second.c_str())));
        checki(fprintf(fi, "itervar %s %s\n", QUOTE(pair.first.c_str()), QUOTE(pair.second.c_str())));
    }

    // save config entries
    for (auto& pair : configEntries) {
        check(fprintf(f, "config %s %s\n", QUOTE(pair.first.c_str()), QUOTE(pair.second.c_str())));
        checki(fprintf(fi, "config %s %s\n", QUOTE(pair.first.c_str()), QUOTE(pair.second.c_str())));
    }

    check(fprintf(f, "\n"));
    checki(fprintf(fi, "\n"));
}

void OmnetppVectorFileWriter::finalizeVector(VectorData *vp)
{
    Assert(isOpen());
    if (!vp->buffer.empty())
        writeBlock(vp);
}

void OmnetppVectorFileWriter::endRecordingForRun()
{
    Assert(isOpen());
    for (VectorData *vp : vectors) {
        finalizeVector(vp);
        delete vp;
    }
    vectors.clear();

    check(fprintf(f, "\n"));
    check(fprintf(fi, "\n"));

    bufferedSamples = 0;
    nextVectorId = 0;
}

void *OmnetppVectorFileWriter::registerVector(const std::string& componentFullPath, const std::string& name, const StringMap& attributes, size_t bufferSize, bool recordEventNumbers)
{
    VectorData *vp = new VectorData();
    vp->id = nextVectorId++;
    vp->recordEventNumbers = recordEventNumbers;
    vp->bufferedSamplesLimit = bufferSize / sizeof(Sample);
    if (vp->bufferedSamplesLimit > 0)
        vp->buffer.reserve(vp->bufferedSamplesLimit);
    vectors.push_back(vp);


    const char *columns = vp->recordEventNumbers ? "ETV" : "TV";
    check(fprintf(f, "vector %d %s %s %s\n", vp->id, QUOTE(componentFullPath.c_str()), QUOTE(name.c_str()), columns));
    for (auto pair : attributes)
        check(fprintf(f, "attr %s %s\n", QUOTE(pair.first.c_str()), QUOTE(pair.second.c_str())));

    // write vector declaration and vector attributes to the index file too
    checki(fprintf(fi, "vector %d %s %s %s\n", vp->id, QUOTE(componentFullPath.c_str()), QUOTE(name.c_str()), columns));
    for (auto pair : attributes)
        checki(fprintf(fi, "attr %s %s\n", QUOTE(pair.first.c_str()), QUOTE(pair.second.c_str())));

    return vp;
}

void OmnetppVectorFileWriter::deregisterVector(void *vectorhandle)
{
    Assert(f != nullptr && vectorhandle != nullptr);
    VectorData *vp = (VectorData *)vectorhandle;
    Vectors::iterator newEnd = std::remove(vectors.begin(), vectors.end(), vp);
    vectors.erase(newEnd, vectors.end());
    finalizeVector(vp);
    delete vp;
}

void OmnetppVectorFileWriter::recordInVector(void *vectorhandle, eventnumber_t eventNumber, rawsimtime_t t, int simtimeScaleExp, double value)
{
    Assert(f != nullptr && vectorhandle != nullptr);
    VectorData *vp = (VectorData *)vectorhandle;

    //TODO ensure time and event number increase monotonically (and remove the same check from cOutVector)

    // store value
    vp->buffer.push_back(Sample(t, simtimeScaleExp, eventNumber, value));
    this->bufferedSamples++;

    // update vector statistics
    if (vp->currentBlock.statistics.getCount() == 0) {
        vp->currentBlock.startEventNum = eventNumber;
        vp->currentBlock.startTime = SimtimeValue{t, simtimeScaleExp};
    }
    vp->currentBlock.endEventNum = eventNumber;
    vp->currentBlock.endTime = SimtimeValue{t, simtimeScaleExp};

    vp->currentBlock.statistics.collect(value);

    // write out block if necessary
    if (vp->bufferedSamplesLimit > 0 && (int)vp->buffer.size() >= vp->bufferedSamplesLimit)
        writeBlock(vp);
    else if (bufferedSamplesLimit > 0 && bufferedSamples >= bufferedSamplesLimit)
        writeRecords();
}

void OmnetppVectorFileWriter::writeRecords()
{
    for (auto vp : vectors)
        if (!vp->buffer.empty())
            writeBlock(vp);
}

void OmnetppVectorFileWriter::writeBlock(VectorData *vp)
{
    Assert(f != nullptr);
    Assert(fi != nullptr);
    Assert(vp != nullptr);
    Assert(!vp->buffer.empty());

    char buf[64], buf2[64];

    Block& currentBlock = vp->currentBlock;
    currentBlock.offset = opp_ftell(f);

    if (vp->recordEventNumbers) {
        for (auto sample : vp->buffer)
            check(fprintf(f, "%d\t%" PRId64 "\t%s\t%.*g\n", vp->id, sample.eventNumber, sample.time.ttoa(buf), prec, sample.value));
    }
    else {
        for (auto sample : vp->buffer)
            check(fprintf(f, "%d\t%s\t%.*g\n", vp->id, sample.time.ttoa(buf), prec, sample.value));
    }

    currentBlock.size = opp_ftell(f) - currentBlock.offset;

    Block& block = vp->currentBlock;
    Statistics& stats = block.statistics;

    // make sure that the offsets referred by the index file are exists in the vector file
    // so the index can be used to access the vector file while it is being written
    fflush(f);

    if (vp->recordEventNumbers) {
        checki(fprintf(fi, "%d\t%" PRId64 " %" PRId64 " %" PRId64 " %" PRId64 " %s %s %" PRId64 " %.*g %.*g %.*g %.*g\n",
                vp->id, block.offset, block.size,
                block.startEventNum, block.endEventNum,
                block.startTime.ttoa(buf), block.endTime.ttoa(buf2),
                stats.getCount(), prec, stats.getMin(), prec, stats.getMax(), prec, stats.getSum(), prec, stats.getSumSqr()));
    }
    else {
        checki(fprintf(fi, "%d\t%" PRId64 " %" PRId64 " %s %s %" PRId64 " %.*g %.*g %.*g %.*g\n",
                vp->id, block.offset, block.size,
                block.startTime.ttoa(buf), block.endTime.ttoa(buf2),
                stats.getCount(), prec, stats.getMin(), prec, stats.getMax(), prec, stats.getSum(), prec, stats.getSumSqr()));
    }

    fflush(fi);
    block.reset();

    bufferedSamples -= vp->buffer.size();
    vp->buffer.clear();
}

void OmnetppVectorFileWriter::flush()
{
    Assert(isOpen());
    writeRecords();  // flushes both files
}


}  // namespace common
}  // namespace omnetpp


