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

#include "defs.h"

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


#ifdef CHECK
#undef CHECK
#endif
#define CHECK(fprintf)    if (fprintf<0) throw new cRuntimeError("Cannot write output vector file `%s'", fname.c_str())
#define WARN(msg)       fprintf(stderr,msg)

// helper function
static void createFileName(opp_string& fname, int run_no, const char *configentry, const char *defaultval)
{
    // get file name from ini file
    fname = ev.config()->getAsFilenames2(getRunSectionName(run_no),"General",configentry,defaultval).c_str();
    ev.app->processFileName(fname);
}

static void removeFile(const char *fname, const char *descr)
{
    if (unlink(fname)!=0 && errno!=ENOENT)
        throw new cRuntimeError("Cannot remove %s `%s': %s", descr, fname, strerror(errno));
}


//=================================================================

Register_Class(cIndexedFileOutputVectorManager);

cIndexedFileOutputVectorManager::cIndexedFileOutputVectorManager()
  : cFileOutputVectorManager()
{
    fi=NULL;
    memoryUsed = 0;

    const char *currentRunSection = getRunSectionName(simulation.runNumber());
    const char *memoryLimitStr = ev.config()->getAsCustom2(currentRunSection,"General","output-vectors-memory-limit",DEFAULT_MEMORY_LIMIT);
    UnitConversion units;
    maxMemoryUsed=max((long)units.parseQuantity(memoryLimitStr, "B"), MIN_BUFFER_MEMORY);
}

void cIndexedFileOutputVectorManager::openIndexFile()
{
        fi = fopen(ifname.c_str(),"w");
        if (fi==NULL)
            throw new cRuntimeError("Cannot open index file `%s'",ifname.c_str());

        // place for header
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

    cFileOutputVectorManager::endRun();
    closeIndexFile();
}

void *cIndexedFileOutputVectorManager::registerVector(const char *modulename, const char *vectorname, int tuple)
{
    if (tuple > 1) WARN("cIndexedFileOutputVectorManager does not support 2-component vectors. Second value will be dropped.\n");

    sVector *vp = (sVector *)createVectorData();
    vp->tuple = tuple;
    vp->id = nextid++;
    vp->initialised = false;
    vp->modulename = modulename;
    vp->vectorname = vectorname;

    // TODO: make this configurable per vector
    vp->maxBufferedSamples = ev.config()->getAsInt2(getRunSectionName(simulation.runNumber()),"General","output-vectors-max-buffered-samples", -1);
    if (vp->maxBufferedSamples > 0)
        vp->allocateBuffer(vp->maxBufferedSamples);

    ev.app->getOutVectorConfig(simulation.runNumber(), modulename, vectorname,
                               vp->enabled, vp->starttime, vp->stoptime);

    vectors.push_back(vp);
    return vp;
}

cFileOutputVectorManager::sVectorData *cIndexedFileOutputVectorManager::createVectorData()
{
    return new sVector;
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
    if (f != NULL) {
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

        vp->buffer.push_back(sSample(t, value));
        vp->count++;
        vp->min = min(vp->min, value);
        vp->max = max(vp->max, value);
        vp->sum += value;
        vp->sumsqr += value*value;

        memoryUsed += sizeof(sSample);

        if (vp->maxBufferedSamples > 0 && (int)vp->buffer.size() >= vp->maxBufferedSamples)
            writeRecords(vp);
        else if (memoryUsed >= maxMemoryUsed)
            writeRecords();

        return true;
    }
    return false;
}

bool cIndexedFileOutputVectorManager::record(void *vectorhandle, simtime_t t, double value1, double value2)
{
    // drop second component, see warning in registerVector()
    return record(vectorhandle, t, value1);
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
    long startOffset = ftell(f);
    for (std::vector<sSample>::iterator it = vp->buffer.begin(); it != vp->buffer.end(); ++it)
    {
        CHECK(fprintf(f,"%d\t%.*g\t%.*g\n", vp->id, prec, it->symtime, prec, it->value));
    }
    long endOffset = ftell(f);

    vp->maxBlockSize = max(vp->maxBlockSize, endOffset - startOffset);
    vp->blocks.push_back(sBlock(startOffset, vp->buffer.size()));
    memoryUsed -= vp->buffer.size()*sizeof(sSample);
    vp->buffer.clear();
}

void cIndexedFileOutputVectorManager::writeIndex(sVector *vp)
{
    if (!fi)
    {
        openIndexFile();
        if (!fi)
            return;
    }

    int nBlocks = vp->blocks.size();
    if (nBlocks > 0)
    {
        CHECK(fprintf(fi,"vector %d  \"%s\"  \"%s\"  %d  %ld  %ld  %.*g  %.*g  %.*g  %.*g\n",
                      vp->id, QUOTE(vp->modulename.c_str()), QUOTE(vp->vectorname.c_str()), 1, vp->maxBlockSize,
                      vp->count, prec, vp->min, prec, vp->max, prec, vp->sum, prec, vp->sumsqr));
        for (int i=0; i<nBlocks; i+=10)
        {
            fprintf(fi, "%d\t", vp->id);   //FIXME use CHECK()
            for (int j = 0; j<10 && i+j < nBlocks; ++j)
            {
                sBlock& block=vp->blocks[i+j];
                fprintf(fi, "%ld:%ld ", block.offset, block.count);
            }
            fprintf(fi, "\n");  //FIXME use CHECK()
        }
        vp->blocks.clear();
    }
}

