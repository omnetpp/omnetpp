//==========================================================================
//  INDEXEDFILEOUTVECTORMGR.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Members of the following classes:
//     cIndexedFileOutputVectorManager
//
//  Author: Tamas Borbely
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
#include "omnetpp/platdep/timeutil.h"
#include "omnetpp/platdep/platmisc.h"
#include "envirbase.h"
#include "indexedfileoutvectormgr.h"

using namespace OPP::common;

NAMESPACE_BEGIN
namespace envir {

#define INDEX_FILE_VERSION 2
#define LL  INT64_PRINTF_FORMAT

using std::ostream;
using std::ofstream;
using std::ios;

Register_PerRunConfigOptionU(CFGID_OUTPUTVECTOR_MEMORY_LIMIT, "output-vectors-memory-limit", "B", DEFAULT_MEMORY_LIMIT, "Total memory that can be used for buffering output vectors. Larger values produce less fragmented vector files (i.e. cause vector data to be grouped into larger chunks), and therefore allow more efficient processing later.");
Register_PerObjectConfigOption(CFGID_VECTOR_MAX_BUFFERED_VALUES, "vector-max-buffered-values", KIND_VECTOR, CFG_INT, nullptr, "For output vectors: the maximum number of values to buffer per vector, before writing out a block into the output vector file. The default is no per-vector limit (i.e. only the total memory limit is in effect)");

#ifdef CHECK
#undef CHECK
#endif

#define CHECK(fprintf, fname)    if (fprintf<0) throw cRuntimeError("Cannot write output file `%s'", fname.c_str())


Register_Class(cIndexedFileOutputVectorManager);

cIndexedFileOutputVectorManager::cIndexedFileOutputVectorManager()
    : cFileOutputVectorManager()
{
    fi = nullptr;
    memoryUsed = 0;

    long d = (long)getEnvir()->getConfig()->getAsDouble(CFGID_OUTPUTVECTOR_MEMORY_LIMIT);
    maxMemoryUsed = (size_t)std::max(d, (long)MIN_BUFFER_MEMORY);
}

void cIndexedFileOutputVectorManager::openIndexFile()
{
    mkPath(directoryOf(ifname.c_str()).c_str());
    fi = fopen(ifname.c_str(), "w");
    if (fi == nullptr)
        throw cRuntimeError("Cannot open index file `%s'", ifname.c_str());

    // leave blank space for fingerprint
    fprintf(fi, "%64s\n", "");
}

void cIndexedFileOutputVectorManager::closeIndexFile()
{
    if (fi) {
        // write out fingerprint (size and modification date of the vector file)
        struct opp_stat_t s;
        if (opp_stat(fname.c_str(), &s) == 0) {
            opp_fseek(fi, 0, SEEK_SET);
            fprintf(fi, "file %" LL "d %" LL "d", (int64_t)s.st_size, (int64_t)s.st_mtime);
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

void cIndexedFileOutputVectorManager::startRun()
{
    cFileOutputVectorManager::startRun();

    closeIndexFile();
    ifname = createIndexFileName(fname);
    removeFile(ifname.c_str(), "old index file");
}

void cIndexedFileOutputVectorManager::endRun()
{
    if (f != nullptr) {
        for (std::vector<Vector *>::iterator it = vectors.begin(); it != vectors.end(); ++it)
            finalizeVector(*it);
    }

    vectors.clear();
    cFileOutputVectorManager::endRun();
    closeIndexFile();
}

void *cIndexedFileOutputVectorManager::registerVector(const char *modulename, const char *vectorname)
{
    Vector *vp = (Vector *)cFileOutputVectorManager::registerVector(modulename, vectorname);
    vp->maxBufferedSamples = getEnvir()->getConfig()->getAsInt(modulename, CFGID_VECTOR_MAX_BUFFERED_VALUES);
    if (vp->maxBufferedSamples > 0)
        vp->allocateBuffer(vp->maxBufferedSamples);

    vectors.push_back(vp);
    return vp;
}

cFileOutputVectorManager::VectorData *cIndexedFileOutputVectorManager::createVectorData()
{
    return new Vector();
}

void cIndexedFileOutputVectorManager::deregisterVector(void *vectorhandle)
{
    Vector *vp = (Vector *)vectorhandle;
    Vectors::iterator newEnd = std::remove(vectors.begin(), vectors.end(), vp);
    vectors.erase(newEnd, vectors.end());
    finalizeVector(vp);
    cFileOutputVectorManager::deregisterVector(vectorhandle);
}

void cIndexedFileOutputVectorManager::initVector(VectorData *vp)
{
    cFileOutputVectorManager::initVector(vp);

    // write vector declaration and vector attributes to the index file too
    CHECK(fprintf(fi, "vector %d  %s  %s  %s\n",
                    vp->id, QUOTE(vp->moduleName.c_str()), QUOTE(vp->vectorName.c_str()), vp->getColumns()),
            ifname);
    for (opp_string_map::iterator it = vp->attributes.begin(); it != vp->attributes.end(); it++)
        CHECK(fprintf(fi, "attr %s  %s\n", QUOTE(it->first.c_str()), QUOTE(it->second.c_str())),
                ifname);
}

void cIndexedFileOutputVectorManager::finalizeVector(Vector *vp)
{
    if (f) {
        if (!vp->buffer.empty())
            writeBlock(vp);
    }
}

bool cIndexedFileOutputVectorManager::record(void *vectorhandle, simtime_t t, double value)
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
        else if (memoryUsed >= maxMemoryUsed)
            writeRecords();

        return true;
    }
    return false;
}

void cIndexedFileOutputVectorManager::writeRunData()
{
    // write run attributes to the vector file
    cFileOutputVectorManager::writeRunData();

    // and to the index file
    if (!fi) {
        openIndexFile();
        CHECK(fprintf(fi, "version %d\n", INDEX_FILE_VERSION), ifname);
    }

    run.writeRunData(fi, ifname);
}

// empties all buffer
void cIndexedFileOutputVectorManager::writeRecords()
{
    for (Vectors::iterator it = vectors.begin(); it != vectors.end(); ++it) {
        if (!(*it)->buffer.empty())  // TODO: size() > configured limit
            writeBlock(*it);
    }
}

void cIndexedFileOutputVectorManager::writeBlock(Vector *vp)
{
    assert(f != nullptr);
    assert(vp != nullptr);
    assert(!vp->buffer.empty());

    static char buff[64];

    Block& currentBlock = vp->currentBlock;
    currentBlock.offset = opp_ftell(f);

    if (vp->recordEventNumbers) {
        for (std::vector<Sample>::iterator it = vp->buffer.begin(); it != vp->buffer.end(); ++it)
            CHECK(fprintf(f, "%d\t%" LL "d\t%s\t%.*g\n", vp->id, it->eventNumber, SIMTIME_TTOA(buff, it->simtime), prec, it->value), fname);
    }
    else {
        for (std::vector<Sample>::iterator it = vp->buffer.begin(); it != vp->buffer.end(); ++it)
            CHECK(fprintf(f, "%d\t%s\t%.*g\n", vp->id, SIMTIME_TTOA(buff, it->simtime), prec, it->value), fname);
    }

    currentBlock.size = opp_ftell(f) - currentBlock.offset;
    writeBlockToIndexFile(vp);

    memoryUsed -= vp->buffer.size()*sizeof(Sample);
    vp->buffer.clear();
}

void cIndexedFileOutputVectorManager::writeBlockToIndexFile(Vector *vp)
{
    assert(f != nullptr);
    assert(fi != nullptr);

    static char buff1[64], buff2[64];
    Block& block = vp->currentBlock;

    if (block.count > 0) {
        // make sure that the offsets refered by the index file are exists in the vector file
        // so the index can be used to access the vector file while it is being written
        fflush(f);

        if (vp->recordEventNumbers) {
            CHECK(fprintf(fi, "%d\t%" LL "d %" LL "d %" LL "d %" LL "d %s %s %ld %.*g %.*g %.*g %.*g\n",
                            vp->id, block.offset, block.size,
                            block.startEventNum, block.endEventNum,
                            SIMTIME_TTOA(buff1, block.startTime), SIMTIME_TTOA(buff2, block.endTime),
                            block.count, prec, block.min, prec, block.max, prec, block.sum, prec, block.sumSqr)
                    , ifname);
        }
        else {
            CHECK(fprintf(fi, "%d\t%" LL "d %" LL "d %s %s %ld %.*g %.*g %.*g %.*g\n",
                            vp->id, block.offset, block.size,
                            SIMTIME_TTOA(buff1, block.startTime), SIMTIME_TTOA(buff2, block.endTime),
                            block.count, prec, block.min, prec, block.max, prec, block.sum, prec, block.sumSqr)
                    , ifname);
        }

        fflush(fi);
        block.reset();
    }
}

}  // namespace envir
NAMESPACE_END

