//==========================================================================
//  FILEOUTVECTORMGR.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga, Tamas Borbely
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "omnetpp/simkerneldefs.h"

#include <sys/stat.h>
#include <cassert>
#include <cstring>
#include <fstream>
#include <cerrno>  // SGI
#include <algorithm>
#include "common/stringutil.h"
#include "common/fileutil.h"
#include "common/unitconversion.h"
#include "omnetpp/cconfigoption.h"
#include "omnetpp/cenvir.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/regmacros.h"
#include "omnetpp/cmodule.h"
#include "omnetpp/cstatistic.h"
#include "omnetpp/platdep/platmisc.h"
#include "envirbase.h"
#include "fileoutvectormgr_merged.h"


using namespace omnetpp::common;

namespace omnetpp {
namespace envir {

#define VECTOR_FILE_VERSION    2
#define INDEX_FILE_VERSION 2

using std::ostream;
using std::ofstream;
using std::ios;

// per-run option
extern omnetpp::cConfigOption *CFGID_OUTPUTVECTOR_MEMORY_LIMIT;

// per-vector option
extern omnetpp::cConfigOption *CFGID_VECTOR_BUFFER;

// global options
extern omnetpp::cConfigOption *CFGID_OUTPUT_VECTOR_FILE;
extern omnetpp::cConfigOption *CFGID_OUTPUT_VECTOR_FILE_APPEND;
extern omnetpp::cConfigOption *CFGID_OUTPUT_VECTOR_PRECISION;

// per-vector options
extern omnetpp::cConfigOption *CFGID_VECTOR_RECORDING;
extern omnetpp::cConfigOption *CFGID_VECTOR_RECORD_EVENTNUMBERS;
extern omnetpp::cConfigOption *CFGID_VECTOR_RECORDING_INTERVALS;


Register_Class(cFileOutputVectorManager_merged);

cFileOutputVectorManager_merged::cFileOutputVectorManager_merged()
{
    nextid = 0;
    initialized = false;
    f = nullptr;
    prec = getEnvir()->getConfig()->getAsInt(CFGID_OUTPUT_VECTOR_PRECISION);

    fi = nullptr;
    memoryUsed = 0;
    totalMemoryLimit = (long)getEnvir()->getConfig()->getAsDouble(CFGID_OUTPUTVECTOR_MEMORY_LIMIT);
}

void cFileOutputVectorManager_merged::openFile()
{
    mkPath(directoryOf(fname.c_str()).c_str());
    f = fopen(fname.c_str(), "a");
    if (f == nullptr)
        throw cRuntimeError("Cannot open output vector file '%s'", fname.c_str());

    // Seek to the end of the file. This is needed because on Windows ftell() returns 0 even after
    // opening the file in append mode. On other systems ftell() correctly points to the end of the file.
    opp_fseek(f, 0, SEEK_END);  
}

void cFileOutputVectorManager_merged::closeFile()
{
    if (f) {
        fclose(f);
        f = nullptr;
    }
}

void cFileOutputVectorManager_merged::check(int fprintfResult)
{
    if (fprintfResult < 0) {
        closeFile();
        closeIndexFile();
        throw cRuntimeError("Cannot write output vector file '%s'", fname.c_str());
    }
}

void cFileOutputVectorManager_merged::checki(int fprintfResult)
{
    if (fprintfResult < 0) {
        closeFile();
        closeIndexFile();
        throw cRuntimeError("Cannot write output vector index file '%s'", ifname.c_str());
    }
}

void cFileOutputVectorManager_merged::openIndexFile()
{
    mkPath(directoryOf(ifname.c_str()).c_str());
    fi = fopen(ifname.c_str(), "w");
    if (fi == nullptr)
        throw cRuntimeError("Cannot open index file '%s'", ifname.c_str());

    // leave blank space for fingerprint
    fprintf(fi, "%64s\n", "");
}

void cFileOutputVectorManager_merged::closeIndexFile()
{
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

static opp_string createIndexFileName(const opp_string fname)
{
    int len = strlen(fname.c_str());
    opp_string ifname;
    ifname.reserve(len + 4);
    strcpy(ifname.buffer(), fname.c_str());
    char *extension = strrchr(ifname.buffer(), '.');
    if (!extension)
        extension = ifname.buffer()+len;

    strcpy(extension, ".vci");
    return ifname;
}

void cFileOutputVectorManager_merged::startRun()
{
    // finish up previous run
    initialized = false;
    closeFile();

    // delete file left over from previous runs
    bool shouldAppend = getEnvir()->getConfig()->getAsBool(CFGID_OUTPUT_VECTOR_FILE_APPEND);
    if (shouldAppend)
        throw cRuntimeError("%s does not support append mode", getClassName());

    fname = getEnvir()->getConfig()->getAsFilename(CFGID_OUTPUT_VECTOR_FILE).c_str();
    dynamic_cast<EnvirBase *>(getEnvir())->processFileName(fname);
    removeFile(fname.c_str(), "old output vector file");

    closeIndexFile();
    ifname = createIndexFileName(fname);
    removeFile(ifname.c_str(), "old index file");
}

void cFileOutputVectorManager_merged::endRun()
{
    if (f != nullptr) {
        for (std::vector<Vector *>::iterator it = vectors.begin(); it != vectors.end(); ++it)
            finalizeVector(*it);
    }

    vectors.clear();
    initialized = false;
    closeFile();
    closeIndexFile();
}

void cFileOutputVectorManager_merged::getOutVectorConfig(const char *modname, const char *vecname,
        bool& outEnabled, bool& outRecordEventNumbers,
        Intervals& outIntervals)
{
    std::string vectorfullpath = std::string(modname) + "." + vecname;
    outEnabled = getEnvir()->getConfig()->getAsBool(vectorfullpath.c_str(), CFGID_VECTOR_RECORDING);
    outRecordEventNumbers = getEnvir()->getConfig()->getAsBool(vectorfullpath.c_str(), CFGID_VECTOR_RECORD_EVENTNUMBERS);

    // get interval string
    const char *text = getEnvir()->getConfig()->getAsCustom(vectorfullpath.c_str(), CFGID_VECTOR_RECORDING_INTERVALS);
    if (text)
        outIntervals.parse(text);
}

void *cFileOutputVectorManager_merged::registerVector(const char *modulename, const char *vectorname)
{
    Vector *vp = createVectorData();
    vp->id = nextid++;
    vp->initialized = false;
    vp->moduleName = modulename;
    vp->vectorName = vectorname;
    getOutVectorConfig(modulename, vectorname, vp->enabled, vp->recordEventNumbers, vp->intervals);

    long bufferSize = (long) getEnvir()->getConfig()->getAsDouble(modulename, CFGID_VECTOR_BUFFER);
    vp->maxBufferedSamples = bufferSize / sizeof(Sample);
    if (vp->maxBufferedSamples > 0)
        vp->allocateBuffer(vp->maxBufferedSamples);

    vectors.push_back(vp);
    return vp;
}

cFileOutputVectorManager_merged::Vector *cFileOutputVectorManager_merged::createVectorData()
{
    return new Vector();
}

void cFileOutputVectorManager_merged::deregisterVector(void *vectorhandle)
{
    ASSERT(vectorhandle != nullptr);
    Vector *vp = (Vector *)vectorhandle;
    Vectors::iterator newEnd = std::remove(vectors.begin(), vectors.end(), vp);
    vectors.erase(newEnd, vectors.end());
    finalizeVector(vp);
    delete vp;
}

void cFileOutputVectorManager_merged::initVector(Vector *vp)
{
    if (!initialized) {
        initialized = true;
        openFile();
        check(fprintf(f, "version %d\n", VECTOR_FILE_VERSION));
        writeRunData();
    }

    check(fprintf(f, "vector %d  %s  %s  %s\n", vp->id, QUOTE(vp->moduleName.c_str()), QUOTE(vp->vectorName.c_str()), vp->getColumns()));
    for (opp_string_map::iterator it = vp->attributes.begin(); it != vp->attributes.end(); ++it)
        check(fprintf(f, "attr %s  %s\n", QUOTE(it->first.c_str()), QUOTE(it->second.c_str())));

    vp->initialized = true;

    // write vector declaration and vector attributes to the index file too
    checki(fprintf(fi, "vector %d  %s  %s  %s\n", vp->id, QUOTE(vp->moduleName.c_str()), QUOTE(vp->vectorName.c_str()), vp->getColumns()));
    for (opp_string_map::iterator it = vp->attributes.begin(); it != vp->attributes.end(); ++it)
        checki(fprintf(fi, "attr %s  %s\n", QUOTE(it->first.c_str()), QUOTE(it->second.c_str())));
}

void cFileOutputVectorManager_merged::finalizeVector(Vector *vp)
{
    if (f) {
        if (!vp->buffer.empty())
            writeBlock(vp);
    }
}

void cFileOutputVectorManager_merged::setVectorAttribute(void *vectorhandle, const char *name, const char *value)
{
    ASSERT(vectorhandle != nullptr);
    Vector *vp = (Vector *)vectorhandle;
    vp->attributes[name] = value;
}

bool cFileOutputVectorManager_merged::record(void *vectorhandle, simtime_t t, double value)
{
    Vector *vp = (Vector *)vectorhandle;

    if (!vp->enabled)
        return false;

    if (vp->intervals.contains(t)) {
        if (!vp->initialized)
            initVector(vp);

        Block& currentBlock = vp->currentBlock;
        eventnumber_t eventNumber = getSimulation()->getEventNumber();
        if (currentBlock.count == 0) {
            currentBlock.startTime = t;
            currentBlock.startEventNum = eventNumber;
        }
        currentBlock.endTime = t;
        currentBlock.endEventNum = eventNumber;
        currentBlock.count++;
        currentBlock.min = std::min(currentBlock.min, value);
        currentBlock.max = std::max(currentBlock.max, value);
        currentBlock.sum += value;
        currentBlock.sumSqr += value*value;

        vp->buffer.push_back(Sample(t, eventNumber, value));
        memoryUsed += sizeof(Sample);

        if (vp->maxBufferedSamples > 0 && (int)vp->buffer.size() >= vp->maxBufferedSamples)
            writeBlock(vp);
        else if (totalMemoryLimit > 0 && memoryUsed >= totalMemoryLimit)
            writeRecords();

        return true;
    }
    return false;
}

void cFileOutputVectorManager_merged::writeRunData()
{
    // write run attributes to the vector file
    ResultFileUtils::writeRunData(f, fname.c_str());

    // and to the index file
    if (!fi) {
        openIndexFile();
        checki(fprintf(fi, "version %d\n", INDEX_FILE_VERSION));
    }

    ResultFileUtils::writeRunData(fi, ifname.c_str());
}

// empties all buffer
void cFileOutputVectorManager_merged::writeRecords()
{
    for (Vectors::iterator it = vectors.begin(); it != vectors.end(); ++it) {
        if (!(*it)->buffer.empty())  // TODO: size() > configured limit
            writeBlock(*it);
    }
}

void cFileOutputVectorManager_merged::writeBlock(Vector *vp)
{
    assert(f != nullptr);
    assert(vp != nullptr);
    assert(!vp->buffer.empty());

    static char buff[64];

    Block& currentBlock = vp->currentBlock;
    currentBlock.offset = opp_ftell(f);

    if (vp->recordEventNumbers) {
        for (std::vector<Sample>::iterator it = vp->buffer.begin(); it != vp->buffer.end(); ++it)
            check(fprintf(f, "%d\t%" PRId64 "\t%s\t%.*g\n", vp->id, it->eventNumber, it->simtime.str(buff), prec, it->value));
    }
    else {
        for (std::vector<Sample>::iterator it = vp->buffer.begin(); it != vp->buffer.end(); ++it)
            check(fprintf(f, "%d\t%s\t%.*g\n", vp->id, it->simtime.str(buff), prec, it->value));
    }

    currentBlock.size = opp_ftell(f) - currentBlock.offset;
    writeBlockToIndexFile(vp);

    memoryUsed -= vp->buffer.size()*sizeof(Sample);
    vp->buffer.clear();
}

void cFileOutputVectorManager_merged::writeBlockToIndexFile(Vector *vp)
{
    assert(f != nullptr);
    assert(fi != nullptr);

    static char buff1[64], buff2[64];
    Block& block = vp->currentBlock;

    if (block.count > 0) {
        // make sure that the offsets referred by the index file are exists in the vector file
        // so the index can be used to access the vector file while it is being written
        fflush(f);

        if (vp->recordEventNumbers) {
            checki(fprintf(fi, "%d\t%" PRId64 " %" PRId64 " %" PRId64 " %" PRId64 " %s %s %ld %.*g %.*g %.*g %.*g\n",
                            vp->id, block.offset, block.size,
                            block.startEventNum, block.endEventNum,
                            block.startTime.str(buff1), block.endTime.str(buff2),
                            block.count, prec, block.min, prec, block.max, prec, block.sum, prec, block.sumSqr));
        }
        else {
            checki(fprintf(fi, "%d\t%" PRId64 " %" PRId64 " %s %s %ld %.*g %.*g %.*g %.*g\n",
                            vp->id, block.offset, block.size,
                            block.startTime.str(buff1), block.endTime.str(buff2),
                            block.count, prec, block.min, prec, block.max, prec, block.sum, prec, block.sumSqr));
        }

        fflush(fi);
        block.reset();
    }
}

const char *cFileOutputVectorManager_merged::getFileName() const
{
    return fname.c_str();
}

void cFileOutputVectorManager_merged::flush()
{
    if (f) {
        fflush(f);
        fflush(fi);
    }
}

}  // namespace envir
}  // namespace omnetpp

