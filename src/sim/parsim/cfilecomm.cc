//=========================================================================
//  CFILECOMM.CC - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <sys/types.h>
#include <sys/stat.h>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cerrno>
#include "common/fileglobber.h"
#include "omnetpp/cexception.h"
#include "omnetpp/globals.h"
#include "omnetpp/cconfigoption.h"
#include "omnetpp/regmacros.h"
#include "omnetpp/cenvir.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/cconfiguration.h"
#include "omnetpp/clog.h"
#include "omnetpp/cmessage.h"
#include "omnetpp/platdep/platmisc.h"
#include "cfilecomm.h"
#include "cfilecommbuffer.h"

using namespace omnetpp::common;

namespace omnetpp {

Register_Class(cFileCommunications);

Register_GlobalConfigOption(CFGID_FILECOMM_PREFIX, "parsim-filecommunications-prefix", CFG_STRING, "comm/", "When `cFileCommunications` is selected as parsim communications class: specifies the prefix (directory+potential filename prefix) for creating the files for cross-partition messages.");
Register_GlobalConfigOption(CFGID_FILECOMM_READ_PREFIX, "parsim-filecommunications-read-prefix", CFG_STRING, "comm/read/", "When `cFileCommunications` is selected as parsim communications class: specifies the prefix (directory) where files will be moved after having been consumed.");
Register_GlobalConfigOption(CFGID_FILECOMM_PRESERVE_READ, "parsim-filecommunications-preserve-read", CFG_BOOL, "false", "When `cFileCommunications` is selected as parsim communications class: specifies that consumed files should be moved into another directory instead of being deleted.");

cFileCommunications::cFileCommunications()
{
}

void cFileCommunications::configure(cSimulation *sim, cConfiguration *cfg, int np, int partitionId, const std::map<std::string,cValue>& extraData)
{
    simulation = sim;
    numPartitions = np;
    myPartitionId = partitionId;
    if (numPartitions == -1 || myPartitionId == -1)
        throw cRuntimeError("%s: Number of partitions or partitionId not specified", getClassName());
    if (numPartitions < 1 || myPartitionId < 0 || myPartitionId >= numPartitions)
        throw cRuntimeError("%s: Invalid value for the number of partitions (%d) or partitionId (%d)", getClassName(), np, partitionId);

    commDirPrefix = cfg->getAsString(CFGID_FILECOMM_PREFIX);
    readDirPrefix = cfg->getAsString(CFGID_FILECOMM_READ_PREFIX);
    preserveReadFiles = cfg->getAsBool(CFGID_FILECOMM_PRESERVE_READ);

    EV << "cFileCommunications: started as process " << myPartitionId << " out of " << numPartitions << ".\n";

    // We cannot check here that the communications directory is empty, because
    // other partitions may have already sent messages to us...
}

void cFileCommunications::shutdown()
{
}

int cFileCommunications::getNumPartitions() const
{
    return numPartitions;
}

int cFileCommunications::getPartitionId() const
{
    return myPartitionId;
}

bool cFileCommunications::packMessage(cCommBuffer *buffer, cMessage *msg, int destPartitionId)
{
    buffer->packObject(msg);
    return false;
}

cMessage *cFileCommunications::unpackMessage(cCommBuffer *buffer)
{
    return (cMessage *)buffer->unpackObject();
}

cCommBuffer *cFileCommunications::createCommBuffer()
{
    return new cFileCommBuffer();
}

void cFileCommunications::recycleCommBuffer(cCommBuffer *buffer)
{
    delete buffer;
}

void cFileCommunications::send(cCommBuffer *buffer, int tag, int destination)
{
    cFileCommBuffer *b = (cFileCommBuffer *)buffer;

    // to prevent concurrency problems, first create the file as .tmp,
    // then rename it to .msg
    char fname[100], fname2[100];
    sprintf(fname, "%s#%.6d-s%d-d%d-t%d.tmp", commDirPrefix.buffer(), seqNum++, myPartitionId, destination, tag);

    // create
    FILE *f = fopen(fname, "wb");
    if (!f)
        throw cRuntimeError("cFileCommunications: Cannot open %s for write: %s", fname, strerror(errno));
    if (fwrite(b->getBuffer(), b->getMessageSize(), 1, f) < 1) {
        fclose(f);
        throw cRuntimeError("cFileCommunications: Cannot write %s: %s", fname, strerror(errno));
    }
    if (fclose(f) != 0)
        throw cRuntimeError("cFileCommunications: Cannot close %s after writing: %s", fname, strerror(errno));

    // rename
    strcpy(fname2, fname);
    strcpy(fname2+strlen(fname2)-4, ".msg");
    if (rename(fname, fname2) != 0)
        throw cRuntimeError("cFileCommunications: Cannot rename %s to %s: %s", fname, fname2, strerror(errno));
}

bool cFileCommunications::receiveBlocking(int filtTag, cCommBuffer *buffer, int& receivedTag, int& sourcePartitionId)
{
    while (!receiveNonblocking(filtTag, buffer, receivedTag, sourcePartitionId)) {
        if (getEnvir()->idle())
            return false;
        usleep(100000);  // be nice and polite: wait 0.1s
    }
    return true;
}

bool cFileCommunications::receiveNonblocking(int filtTag, cCommBuffer *buffer, int& receivedTag, int& sourcePartitionId)
{
    cFileCommBuffer *b = (cFileCommBuffer *)buffer;
    b->reset();

    char fmask[100];
    char fname2[100];
    if (filtTag == PARSIM_ANY_TAG)
        sprintf(fmask, "%s#*-s*-d%d-t*.msg", commDirPrefix.buffer(), myPartitionId);
    else
        sprintf(fmask, "%s#*-s*-d%d-t%d.msg", commDirPrefix.buffer(), myPartitionId, filtTag);

    bool ret = false;
    const char *fname = FileGlobber(fmask).getNext();
    if (fname) {
        ret = true;

        // parse fname
        const char *s = strstr(fname, "-s");
        sourcePartitionId = atol(s+2);
        const char *t = strstr(fname, "-t");
        receivedTag = atol(t+2);
        // const char *n = strstr(fname, "#");
        // int seqNum = atol(n+2);

        // DBG: printf("%d: filecomm: found %s -- src=%d, tag=%d\n",getPartitionId(),fname,sourcePartitionId,receivedTag);

        // read data
        struct stat statbuf;
        if (stat(fname, &statbuf) != 0)
            throw cRuntimeError("cFileCommunications: Cannot stat() file %s: %s", fname, strerror(errno));
        int len = statbuf.st_size;
        b->allocateAtLeast(len);
        FILE *f = fopen(fname, "rb");
        if (!f) {
            // try a bit harder. On Windows, first fopen() sometimes fails with
            // "permission denied".
            for (int i = 0; i < 20; i++) {
                usleep(500000);  // wait 0.5s
                EV << "cFileCommunications: retrying opening file " << fname << " (previous attempt failed)\n";
                f = fopen(fname, "rb");
                if (f)
                    break;
            }
        }
        if (!f)
            throw cRuntimeError("cFileCommunications: Cannot open existing file %s for read: %s", fname, strerror(errno));
        if (fread(b->getBuffer(), len, 1, f) == 0)
            // FIXME condition always fires. why?
            // throw cRuntimeError("cFileCommunications: Cannot read existing file %s: %s", fname, strerror(errno));
            ;
        fclose(f);
        b->setMessageSize(len);

        if (preserveReadFiles) {
            // move file to 'read' directory
            //
            // BEWARE: for mysterious reasons, it appears that there cannot be more
            // than about 19800 files in a directory. When that point is reached,
            // an exception is thrown somewhere inside the standard C library, which
            // materializes itself in OMNeT++ as an "Error: (null)" message...
            // Strangely, this can be reproduced in both Linux and Windows.
            //
            strcpy(fname2, readDirPrefix.buffer());
            strcat(fname2, fname + strlen(commDirPrefix.buffer()));
            if (rename(fname, fname2) != 0)
                throw cRuntimeError("cFileCommunications: Cannot rename %s to %s: %s", fname, fname2, strerror(errno));
        }
        else {
            // delete file
            if (unlink(fname) != 0)
                throw cRuntimeError("cFileCommunications: Cannot delete file %s: %s", fname, strerror(errno));
        }
    }
    // DBG: printf("%d: filecomm: nothing found matching %s\n",getPartitionId(),fmask);
    return ret;
}

}  // namespace omnetpp

