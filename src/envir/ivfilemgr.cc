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

using std::ostream;
using std::ofstream;
using std::ios;

Register_PerRunConfigEntryU(CFGID_OUTPUT_VECTORS_MEMORY_LIMIT, "output-vectors-memory-limit", "General", "B", DEFAULT_MEMORY_LIMIT, "Total memory that can be used for buffering output vectors. Larger values produce less fragmented vector files (i.e. cause vector data to be grouped into larger chunks), and therefore allow more efficient processing later.");

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

    long d = (long) ev.config()->getAsDouble(CFGID_OUTPUT_VECTORS_MEMORY_LIMIT);
    maxMemoryUsed = max(d, MIN_BUFFER_MEMORY);
}

void cIndexedFileOutputVectorManager::openIndexFile()
{
        fi = fopen(ifname.c_str(),"w");
        if (fi==NULL)
            throw cRuntimeError("Cannot open index file `%s'",ifname.c_str());

        // leave blank space for header
        fprintf(fi, "%64s\n", "");
}

void cIndexedFileOutputVectorManager::closeIndexFile()
{
    if (fi)
    {
        // write out size and modification date
        struct stat s;
        if (stat(fname.c_str(), &s) == 0)
        {
            fseek(fi, 0, SEEK_SET);
            fprintf(fi, "file %ld %ld", (long)s.st_size, (long)s.st_mtime);
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
    static char param[1024];
    sprintf(param, "%s.%s.max-buffered-samples", modulename, vectorname);
    vp->maxBufferedSamples = ev.config()->getAsInt2(NULL, "OutVectors", param, -1);
    if (vp->maxBufferedSamples > 0)
        vp->allocateBuffer(vp->maxBufferedSamples);

    vp->blocks.push_back(sBlock());
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
    std::remove(vectors.begin(), vectors.end(), vp);
    finalizeVector(vp);
    delete vp;
}

void cIndexedFileOutputVectorManager::finalizeVector(sVector *vp)
{
    if (f)
    {
        if (!vp->buffer.empty())
            writeRecords(vp);
        writeIndex(vp);
    }
}

bool cIndexedFileOutputVectorManager::record(void *vectorhandle, simtime_t t, double value)
{
    sVector *vp = (sVector *)vectorhandle;

    if (!vp->enabled)
        return false;

    if (t>=vp->starttime && (vp->stoptime==0.0 || t<=vp->stoptime))
    {
        if (!vp->initialised)
            initVector(vp);

        sBlock &currentBlock = vp->blocks.back();
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
            writeRecords(vp);
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
            writeRecords(*it);
    }
}

void cIndexedFileOutputVectorManager::writeRecords(sVector *vp)
{
    assert(f!=NULL);
    assert(vp!=NULL);
    assert(!vp->buffer.empty());
    assert(!vp->blocks.empty());

    static char buff[64];

    sBlock &currentBlock = vp->blocks.back();
    currentBlock.offset = ftell(f);

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

    vp->maxBlockSize = max(vp->maxBlockSize, ftell(f) - currentBlock.offset);
    vp->count += currentBlock.count;
    vp->min = min(vp->min, currentBlock.min);
    vp->max = max(vp->max, currentBlock.max);
    vp->sum += currentBlock.sum;
    vp->sumsqr += currentBlock.sumSqr;

    memoryUsed -= vp->buffer.size()*sizeof(sSample);
    vp->buffer.clear();
    vp->blocks.push_back(sBlock());
}

void cIndexedFileOutputVectorManager::writeIndex(sVector *vp)
{
    static char buff1[64], buff2[64];

    if (vp->count > 0)
    {
        // vector
        CHECK(fprintf(fi,"vector %d  %s  %s  %s  %ld  %ld  %.*g  %.*g  %.*g  %.*g\n",
                      vp->id, QUOTE(vp->modulename.c_str()), QUOTE(vp->vectorname.c_str()),
                      vp->getColumns(), vp->maxBlockSize,
                      vp->count, prec, vp->min, prec, vp->max, prec, vp->sum, prec, vp->sumsqr), ifname);
        // attrs
        for (opp_string_map::iterator it=vp->attributes.begin(); it!=vp->attributes.end(); it++)
            CHECK(fprintf(fi,"attr %s  %s\n", QUOTE(it->first.c_str()), QUOTE(it->second.c_str())), ifname);

        // blocks
        int nBlocks = vp->blocks.size();
        if (vp->recordEventNumbers)
        {
            for (Blocks::iterator blockPtr = vp->blocks.begin(); blockPtr != vp->blocks.end(); ++blockPtr)
            {
                if (blockPtr->count > 0) // last block might be empty
                {
                    CHECK(fprintf(fi, "%d\t%ld %ld %ld %s %s %ld %.*g %.*g %.*g %.*g\n",
                                vp->id, blockPtr->offset,
                                blockPtr->startEventNum, blockPtr->endEventNum,
                                SIMTIME_TTOA(buff1, blockPtr->startTime), SIMTIME_TTOA(buff2, blockPtr->endTime),
                                blockPtr->count, prec, blockPtr->min, prec, blockPtr->max, prec, blockPtr->sum, prec, blockPtr->sumSqr)
                          , ifname);
                }
            }
        }
        else
        {
            for (Blocks::iterator blockPtr = vp->blocks.begin(); blockPtr != vp->blocks.end(); ++blockPtr)
            {
                if (blockPtr->count > 0) // last block might be empty
                {
                    CHECK(fprintf(fi, "%d\t%ld %s %s %ld %.*g %.*g %.*g %.*g\n",
                                vp->id, blockPtr->offset,
                                SIMTIME_TTOA(buff1, blockPtr->startTime), SIMTIME_TTOA(buff2, blockPtr->endTime),
                                blockPtr->count, prec, blockPtr->min, prec, blockPtr->max, prec, blockPtr->sum, prec, blockPtr->sumSqr)
                          , ifname);
                }
            }
        }
        vp->blocks.clear();
        vp->blocks.push_back(sBlock());
    }
}

