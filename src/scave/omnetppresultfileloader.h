//=========================================================================
//  OMNETPPRESULTFILELOADER.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga, Tamas Borbely, Zoltan Bojthe
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_SCAVE_OMNETPPRESULTFILELOADER_H
#define __OMNETPP_SCAVE_OMNETPPRESULTFILELOADER_H

#include <cassert>
#include <cmath>
#include <string>
#include <string.h>
#include <vector>
#include <set>
#include <map>
#include <list>

#include "common/exception.h"
#include "common/commonutil.h"
#include "idlist.h"
#include "enumtype.h"
#include "statistics.h"
#include "scaveutils.h"
#include "enums.h"

#ifdef THREADED
#include "common/rwlock.h"
#endif

#include "resultfilemanager.h"

namespace omnetpp {
namespace scave {

class SCAVE_API OmnetppResultFileLoader : public IResultFileLoader
{
  protected:
    struct ParseContext {
        ResultFile *fileRef; /*in*/
        const char *fileName; /*in*/
        int64_t lineNo; /*inout*/
        FileRun *fileRunRef; /*inout*/
        // references to the result items which attributes should be added to
        int lastResultItemType; /*inout*/
        int lastResultItemIndex; /*inout*/
        // collected fields of the histogram to be created when the
        // first 'bin' is parsed
        std::string moduleName;
        std::string statisticName;
        long count;
        double min, max, sum, sumSqr;

        ParseContext(ResultFile* fileRef);
        void clearHistogram();
    };
  protected:
    void processLine(char **vec, int numTokens, ParseContext& ctx);
    void loadVectorsFromIndex(const char *filename, ResultFile *fileRef);
  public:
    OmnetppResultFileLoader(ResultFileManager *resultFileManagerPar) : IResultFileLoader(resultFileManagerPar) {}
    virtual ResultFile *loadFile(const char *fileName, const char *fileSystemFileName=nullptr, bool reload=false) override;
};

} // namespace scave
}  // namespace omnetpp


#endif
