//=========================================================================
//  OMNETPPRESULTFILELOADER.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga, Tamas Borbely, Zoltan Bojthe
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

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
        ResultFile *fileRef = nullptr;
        const char *fileName = nullptr;
        int64_t lineNo = 0;
        FileRun *fileRunRef = nullptr;

        enum {NONE, RUN, SCALAR, VECTOR, STATISTICS, HISTOGRAM} currentItemType = NONE;
        std::string runName;
        OrderedKeyValueList moduleParams;
        std::string moduleName;
        std::string resultName;
        StringMap attrs;
        StringMap itervars;
        int vectorId = -1;
        std::string vectorColumns; //TODO switch to 'bool hasEventNumber'
        double scalarValue;
        struct {
            int64_t count;
            double minValue;
            double maxValue;
            double sum;
            double sumSquares;
            double sumWeights;
            double sumWeightedValues;
            double sumSquaredWeights;
            double sumWeightedSquaredValues;
        } fields;
        Histogram bins;
    };
  protected:
    void processLine(char **vec, int numTokens, ParseContext& ctx);
    void loadVectorsFromIndex(const char *filename, ResultFile *fileRef);
    void flush(ParseContext& ctx);
    void resetFields(ParseContext& ctx);
    Statistics makeStatsFromFields(ParseContext& ctx);
    void separateItervarsFromAttrs(StringMap& attrs, StringMap& itervars);
  public:
    OmnetppResultFileLoader(ResultFileManager *resultFileManagerPar) : IResultFileLoader(resultFileManagerPar) {}
    virtual ResultFile *loadFile(const char *fileName, const char *fileSystemFileName=nullptr, bool reload=false) override;
};

} // namespace scave
}  // namespace omnetpp


#endif
