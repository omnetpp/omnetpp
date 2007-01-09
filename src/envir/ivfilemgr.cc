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
#include "platdep/timeutil.h"
#include "platdep/platmisc.h"
#include "cenvir.h"
#include "omnetapp.h"
#include "csimulation.h"
#include "regmacros.h"
#include "cmodule.h"
#include "cstat.h"
#include "stringutil.h"
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
    char section[16];
    sprintf(section,"Run %d",run_no);

    fname = ev.config()->getAsFilenames2(section,"General",configentry,defaultval).c_str();
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
}

void cIndexedFileOutputVectorManager::openIndexFile()
{
        fi = fopen(ifname.c_str(),"a");
        if (fi==NULL)
            throw new cRuntimeError("Cannot open index file `%s'",ifname.c_str());
}

void cIndexedFileOutputVectorManager::closeIndexFile()
{
    if (fi)
    {
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
        for (std::vector<sVectorDataExt*>::iterator it=vectors.begin(); it!=vectors.end(); ++it)
            finalizeVector(*it);
    }

    cFileOutputVectorManager::endRun();
    closeIndexFile();
}

void *cIndexedFileOutputVectorManager::registerVector(const char *modulename, const char *vectorname, int tuple)
{
    if (tuple > 1) WARN("cIndexedFileOutputVectorManager does not support 2-component vectors. Second value will be dropped.\n");

    sVectorDataExt *vp = (sVectorDataExt *)createVectorData();
    vp->tuple = tuple;
    vp->id = nextid++;
    vp->initialised = false;
    vp->modulename = modulename;
    vp->vectorname = vectorname;

    // TODO: add "output-vector-block-size" to getOutVectorConfig()
    //   ^^^^^ we probably don't need that. getOutVectorConfig() is per-vector, and probably there isn't that
    //         much benefit in being able to set the block size individually for different vectors...   --Andras
    char section[16];
    sprintf(section,"Run %d",simulation.runNumber());
    int buffer_size = (int)ev.config()->getAsInt2(section,"General","output-vector-block-size",DEFAULT_BUFFER_SIZE);
    buffer_size=max(buffer_size, MIN_BUFFER_SIZE);
    vp->allocate_buffer(buffer_size);

    ev.app->getOutVectorConfig(simulation.runNumber(), modulename, vectorname,
                               vp->enabled, vp->starttime, vp->stoptime);

    vectors.push_back(vp);
    return vp;
}

cFileOutputVectorManager::sVectorData *cIndexedFileOutputVectorManager::createVectorData()
{
    return new sVectorDataExt;
}

void cIndexedFileOutputVectorManager::deregisterVector(void *vectorhandle)
{
    sVectorDataExt *vp = (sVectorDataExt *)vectorhandle;
    std::remove(vectors.begin(), vectors.end(), vp);
    finalizeVector(vp);
    delete vp;
}

void cIndexedFileOutputVectorManager::finalizeVector(sVectorDataExt *vp)
{
    if (f != NULL) {
        if (vp->dirty())
            writeRecords(vp);
        writeIndex(vp);
    }
}

bool cIndexedFileOutputVectorManager::record(void *vectorhandle, simtime_t t, double value)
{
    sVectorDataExt *vp = (sVectorDataExt *)vectorhandle;

    if (!vp->enabled)
        return false;

    if (t>=vp->starttime && (vp->stoptime==0.0 || t<=vp->stoptime))
    {
        if (!vp->initialised)
            initVector(vp);
        assert(f!=NULL);

        if (vp->buffer_ptr - vp->buffer > vp->buffer_size - 100)
            writeRecords(vp);
        int c = sprintf(vp->buffer_ptr,"%ld\t%.*g\t%.*g\n", vp->id, prec, t, prec, value);
        if (c > 0) {
            vp->buffer_ptr += c;
            vp->buffer_num_samples++;
            vp->num_samples++;
            vp->min = min(vp->min, value);
            vp->max = max(vp->max, value);
            vp->sum += value;
            vp->sumsqr += value*value;
        }
        return true;
    }
    return false;
}

bool cIndexedFileOutputVectorManager::record(void *vectorhandle, simtime_t t, double value1, double value2)
{
    // drop second component, see warning in registerVector()
    return record(vectorhandle, t, value1);
}

void cIndexedFileOutputVectorManager::writeRecords(sVectorDataExt *vp)
{
    assert(f!=NULL);
    long offset = ftell(f);
    CHECK(fputs(vp->buffer, f));
    vp->blocks.push_back(sBlock(offset, vp->buffer_num_samples));
    vp->clear_buffer();
}

void cIndexedFileOutputVectorManager::writeIndex(sVectorDataExt *vp)
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
        CHECK(fprintf(fi,"vector %ld  \"%s\"  \"%s\"  %d  %d  %d  %.*g  %.*g  %.*g  %.*g\n",
                      vp->id, QUOTE(vp->modulename.c_str()), QUOTE(vp->vectorname.c_str()), vp->tuple, vp->buffer_size,
                      vp->num_samples, prec, vp->min, prec, vp->max, prec, vp->sum, prec, vp->sumsqr));
        for (int i=0; i<nBlocks; i+=10)
        {
            fprintf(fi, "%ld\t", vp->id);
            for (int j = 0; j<10 && i+j < nBlocks; ++j)
            {
                sBlock &block=vp->blocks[i+j];
                fprintf(fi, "%ld:%ld ", block.offset, block.num_samples);
            }
            fprintf(fi, "\n");
        }
        vp->blocks.clear();
    }
}

