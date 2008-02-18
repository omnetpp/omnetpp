//==========================================================================
//  IVFILEMGR.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Members of the following classes:
//     cIndexedFileOutputVectorManager
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "simkerneldefs.h"

#include <assert.h>
#include <string.h>
#include <fstream>
#include <errno.h> // SGI
#include <algorithm>
#include <sys/stat.h>
#include "cconfigkey.h"
#include "timeutil.h"
#include "platmisc.h"
#include "cenvir.h"
#include "omnetapp.h"
#include "csimulation.h"
#include "regmacros.h"
#include "cmodule.h"
#include "cstat.h"
#include "stringutil.h"
#include "unitconversion.h"
#include "ivfilemgr.h"

USING_NAMESPACE

#define LL  INT64_PRINTF_FORMAT

using std::ostream;
using std::ofstream;
using std::ios;

Register_PerRunConfigEntryU(CFGID_OUTPUTVECTOR_MEMORY_LIMIT, "output-vectors-memory-limit", "B", DEFAULT_MEMORY_LIMIT, "Total memory that can be used for buffering output vectors. Larger values produce less fragmented vector files (i.e. cause vector data to be grouped into larger chunks), and therefore allow more efficient processing later.");
Register_PerObjectConfigEntry(CFGID_OUTVECTOR_MAX_BUFFERED_SAMPLES, "max-buffered-samples", CFG_INT, NULL, "For output vectors: the maximum number of values to buffer per vector, before writing out a block into the output vector file.");


#ifdef CHECK
#undef CHECK
#endif

#define CHECK(fprintf, fname)    if (fprintf<0) throw cRuntimeError("Cannot write output file `%s'", fname.c_str())
#define WARN(msg)       fprintf(stderr,msg)


static void removeFile(const char *fname, const char *descr)
{
    if (unlink(fname)!=0 && errno!=ENOENT)
        throw cRuntimeError("Cannot remove %s `%s': %s", descr, fname, strerror(errno));
}


//=================================================================

Register_Class(cIndexedFileOutputVectorManager);

cIndexedFileOutputVectorManager::cIndexedFileOutputVectorManager()
  : cFileOutputVectorManager()
{
    fi = NULL;
    memoryUsed = 0;

    long d = (long) ev.config()->getAsDouble(CFGID_OUTPUTVECTOR_MEMORY_LIMIT);
    maxMemoryUsed = (size_t) max(d, MIN_BUFFER_MEMORY);
}

void cIndexedFileOutputVectorManager::openIndexFile()
{
    fi = fopen(ifname.c_str(),"w");
    if (fi==NULL)
        throw cRuntimeError("Cannot open index file `%s'",ifname.c_str());

    // leave blank space for fingerprint
    fprintf(fi, "%64s\n", "");
}

void cIndexedFileOutputVectorManager::closeIndexFile()
{
    if (fi)
    {
        // write out fingerprint (size and modification date of the vector file)
        struct opp_stat_t s;
        if (opp_stat(fname.c_str(), &s) == 0)
        {
            opp_fseek(fi, 0, SEEK_SET);
            fprintf(fi, "file %"LL"d %"LL"d", (int64)s.st_size, (int64)s.st_mtime);
        }

        fclose(fi);
        fi = NULL;
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
    if (f!=NULL) {
        for (std::vector<sVector*>::iterator it=vectors.begin(); it!=vectors.end(); ++it)
            finalizeVector(*it);
    }

    vectors.clear();
    cFileOutputVectorManager::endRun();
    closeIndexFile();
}

void *cIndexedFileOutputVectorManager::registerVector(const char *modulename, const char *vectorname)
{
    sVector *vp = (sVector *)cFileOutputVectorManager::registerVector(modulename, vectorname);
    vp->maxBufferedSamples = ev.config()->getAsInt(modulename, CFGID_OUTVECTOR_MAX_BUFFERED_SAMPLES);
    if (vp->maxBufferedSamples > 0)
        vp->allocateBuffer(vp->maxBufferedSamples);

    vectors.push_back(vp);
    return vp;
}

cFileOutputVectorManager::sVectorData *cIndexedFileOutputVectorManager::createVectorData()
{
    return new sVector();
}

void cIndexedFileOutputVectorManager::deregisterVector(void *vectorhandle)
{
    sVector *vp = (sVector *)vectorhandle;
    Vectors::iterator newEnd = std::remove(vectors.begin(), vectors.end(), vp);
    vectors.erase(newEnd, vectors.end());
    finalizeVector(vp);
    delete[] vp->intervals;
    delete vp;
}

void cIndexedFileOutputVectorManager::initVector(sVectorData *vp)
{
    cFileOutputVectorManager::initVector(vp);

    // write vector declaration and vector attributes to the index file too
    CHECK(fprintf(fi,"vector %d  %s  %s  %s\n",
                  vp->id, QUOTE(vp->modulename.c_str()), QUOTE(vp->vectorname.c_str()), vp->getColumns()),
          ifname);
    for (opp_string_map::iterator it=vp->attributes.begin(); it!=vp->attributes.end(); it++)
        CHECK(fprintf(fi,"attr %s  %s\n", QUOTE(it->first.c_str()), QUOTE(it->second.c_str())),
              ifname);
}


void cIndexedFileOutputVectorManager::finalizeVector(sVector *vp)
{
    if (f)
    {
        if (!vp->buffer.empty())
            writeBlock(vp);
    }
}

bool cIndexedFileOutputVectorManager::record(void *vectorhandle, simtime_t t, double value)
{
    sVector *vp = (sVector *)vectorhandle;

    if (!vp->enabled)
        return false;

    if (!vp->intervals || containsTime(t, vp->intervals))
    {
        if (!vp->initialized)
            initVector(vp);

        sBlock &currentBlock = vp->currentBlock;
        long eventNumber = simulation.eventNumber();
        if (currentBlock.count == 0)
        {
            currentBlock.startTime = t;
            currentBlock.startEventNum = eventNumber;
        }
        currentBlock.endTime = t;
        currentBlock.endEventNum = eventNumber;
        currentBlock.count++;
        currentBlock.min = min(currentBlock.min, value);
        currentBlock.max = max(currentBlock.max, value);
        currentBlock.sum += value;
        currentBlock.sumSqr += value*value;

        vp->buffer.push_back(sSample(t, eventNumber, value));
        memoryUsed += sizeof(sSample);

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
    if (!fi)
    {
        openIndexFile();
    }

    CHECK(fprintf(fi, "run %s\n", QUOTE(run.runId.c_str())), ifname);
    for (opp_string_map::const_iterator it = run.attributes.begin(); it != run.attributes.end(); ++it)
    {
        CHECK(fprintf(fi, "attr %s %s\n", it->first.c_str(), QUOTE(it->second.c_str())), ifname);
    }
    for (opp_string_map::const_iterator it = run.moduleParams.begin(); it != run.moduleParams.end(); ++it)
    {
        CHECK(fprintf(fi, "param %s %s\n", it->first.c_str(), QUOTE(it->second.c_str())), ifname);
    }
}

// empties all buffer
void cIndexedFileOutputVectorManager::writeRecords()
{
    for (Vectors::iterator it = vectors.begin(); it != vectors.end(); ++it)
    {
        if (!(*it)->buffer.empty()) // TODO: size() > configured limit
            writeBlock(*it);
    }
}

void cIndexedFileOutputVectorManager::writeBlock(sVector *vp)
{
    assert(f!=NULL);
    assert(vp!=NULL);
    assert(!vp->buffer.empty());

    static char buff[64];

    sBlock &currentBlock = vp->currentBlock;
    currentBlock.offset = opp_ftell(f);

    if (vp->recordEventNumbers)
    {
        for (std::vector<sSample>::iterator it = vp->buffer.begin(); it != vp->buffer.end(); ++it)
            CHECK(fprintf(f,"%d\t%ld\t%s\t%.*g\n", vp->id, it->eventNumber, SIMTIME_TTOA(buff, it->simtime), prec, it->value), fname);
    }
    else
    {
        for (std::vector<sSample>::iterator it = vp->buffer.begin(); it != vp->buffer.end(); ++it)
            CHECK(fprintf(f,"%d\t%s\t%.*g\n", vp->id, SIMTIME_TTOA(buff, it->simtime), prec, it->value), fname);
    }

    currentBlock.size = opp_ftell(f) - currentBlock.offset;
    writeBlockToIndexFile(vp);

    memoryUsed -= vp->buffer.size()*sizeof(sSample);
    vp->buffer.clear();
}

void cIndexedFileOutputVectorManager::writeBlockToIndexFile(sVector *vp)
{
    assert(f!=NULL);
    assert(fi!=NULL);

    static char buff1[64], buff2[64];
    sBlock &block = vp->currentBlock;

    if (block.count > 0)
    {
        // make sure that the offsets refered by the index file are exists in the vector file
        // so the index can be used to access the vector file while it is being written
        fflush(f);

        if (vp->recordEventNumbers)
        {
            CHECK(fprintf(fi, "%d\t%"LL"d %"LL"d %ld %ld %s %s %ld %.*g %.*g %.*g %.*g\n",
                        vp->id, block.offset, block.size,
                        block.startEventNum, block.endEventNum,
                        SIMTIME_TTOA(buff1, block.startTime), SIMTIME_TTOA(buff2, block.endTime),
                        block.count, prec, block.min, prec, block.max, prec, block.sum, prec, block.sumSqr)
                  , ifname);
        }
        else
        {
            CHECK(fprintf(fi, "%d\t%"LL"d %"LL"d %s %s %ld %.*g %.*g %.*g %.*g\n",
                        vp->id, block.offset, block.size,
                        SIMTIME_TTOA(buff1, block.startTime), SIMTIME_TTOA(buff2, block.endTime),
                        block.count, prec, block.min, prec, block.max, prec, block.sum, prec, block.sumSqr)
                  , ifname);
        }

        fflush(fi);
        block.reset();
    }
}


