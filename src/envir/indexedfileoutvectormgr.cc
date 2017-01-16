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
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

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
#include "indexedfileoutvectormgr.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace envir {

#define INDEX_FILE_VERSION 2

using std::ostream;
using std::ofstream;
using std::ios;

// per-run option
extern omnetpp::cConfigOption *CFGID_OUTPUTVECTOR_MEMORY_LIMIT;

// per-vector option
extern omnetpp::cConfigOption *CFGID_VECTOR_BUFFER;

#ifdef CHECK
#undef CHECK
#endif
#define CHECK(fprintf, fname)    if (fprintf<0) throw cRuntimeError("Cannot write output file '%s'", fname.c_str())


Register_Class(cIndexedFileOutputVectorManager);

cIndexedFileOutputVectorManager::cIndexedFileOutputVectorManager()
    : cFileOutputVectorManager()
{
    fi = nullptr;
    memoryUsed = 0;
    totalMemoryLimit = (long)getEnvir()->getConfig()->getAsDouble(CFGID_OUTPUTVECTOR_MEMORY_LIMIT);
}

void cIndexedFileOutputVectorManager::openIndexFile()
{
    mkPath(directoryOf(ifname.c_str()).c_str());
    fi = fopen(ifname.c_str(), "w");
    if (fi == nullptr)
        throw cRuntimeError("Cannot open index file '%s'", ifname.c_str());

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
    long bufferSize = (long) getEnvir()->getConfig()->getAsDouble(modulename, CFGID_VECTOR_BUFFER);
    vp->maxBufferedSamples = bufferSize / sizeof(Sample);
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
    CHECK(fprintf(fi, "vector %d %s %s %s\n",
                    vp->id, QUOTE(vp->moduleName.c_str()), QUOTE(vp->vectorName.c_str()), vp->getColumns()),
            ifname);
    for (opp_string_map::iterator it = vp->attributes.begin(); it != vp->attributes.end(); ++it)
        CHECK(fprintf(fi, "attr %s %s\n", QUOTE(it->first.c_str()), QUOTE(it->second.c_str())),
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
        else if (totalMemoryLimit > 0 && memoryUsed >= totalMemoryLimit)
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

    ResultFileUtils::writeRunData(fi, ifname.c_str());
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
            CHECK(fprintf(f, "%d\t%" PRId64 "\t%s\t%.*g\n", vp->id, it->eventNumber, it->simtime.str(buff), prec, it->value), fname);
    }
    else {
        for (std::vector<Sample>::iterator it = vp->buffer.begin(); it != vp->buffer.end(); ++it)
            CHECK(fprintf(f, "%d\t%s\t%.*g\n", vp->id, it->simtime.str(buff), prec, it->value), fname);
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
        // make sure that the offsets referred by the index file are exists in the vector file
        // so the index can be used to access the vector file while it is being written
        fflush(f);

        if (vp->recordEventNumbers) {
            CHECK(fprintf(fi, "%d\t%" PRId64 " %" PRId64 " %" PRId64 " %" PRId64 " %s %s %ld %.*g %.*g %.*g %.*g\n",
                            vp->id, block.offset, block.size,
                            block.startEventNum, block.endEventNum,
                            block.startTime.str(buff1), block.endTime.str(buff2),
                            block.count, prec, block.min, prec, block.max, prec, block.sum, prec, block.sumSqr)
                    , ifname);
        }
        else {
            CHECK(fprintf(fi, "%d\t%" PRId64 " %" PRId64 " %s %s %ld %.*g %.*g %.*g %.*g\n",
                            vp->id, block.offset, block.size,
                            block.startTime.str(buff1), block.endTime.str(buff2),
                            block.count, prec, block.min, prec, block.max, prec, block.sum, prec, block.sumSqr)
                    , ifname);
        }

        fflush(fi);
        block.reset();
    }
}

}  // namespace envir
}  // namespace omnetpp

