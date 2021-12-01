//==========================================================================
//  FIELDS.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_SCAVE_FIELDS_H
#define __OMNETPP_SCAVE_FIELDS_H

#include <functional>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include "common/commonutil.h"
#include "common/patternmatcher.h"
#include "common/stringutil.h"
#include "resultfilemanager.h"
#include "scaveutils.h"

namespace omnetpp {
namespace scave {

/**
 * String constants for all sorts of names: run and result item fields,
 * attribute names, field prefixes, etc.
 */
class SCAVE_API Scave
{
  public:
    // basic fields of result items
    static constexpr const char *FILE   = "file";
    static constexpr const char *RUN    = "run";
    static constexpr const char *MODULE = "module";
    static constexpr const char *NAME   = "name";

    // name prefixes in filter expressions
    static constexpr const char *RUNATTR_PREFIX = "runattr:";
    static constexpr const char *ATTR_PREFIX = "attr:";
    static constexpr const char *ITERVAR_PREFIX = "itervar:";
    static constexpr const char *CONFIG_PREFIX = "config:";

    // run attributes
    static constexpr const char *INIFILE     = "inifile";
    static constexpr const char *CONFIGNAME  = "configname";
    static constexpr const char *RUNNUMBER   = "runnumber";
    static constexpr const char *NETWORK     = "network";
    static constexpr const char *EXPERIMENT  = "experiment";
    static constexpr const char *MEASUREMENT = "measurement";
    static constexpr const char *REPLICATION = "replication";
    static constexpr const char *DATETIME    = "datetime";
    static constexpr const char *PROCESSID   = "processid";
    static constexpr const char *RESULTDIR   = "resultdir";
    static constexpr const char *REPETITION  = "repetition";
    static constexpr const char *SEEDSET     = "seedset";
    static constexpr const char *ITERATIONVARS = "iterationvars";
    static constexpr const char *ITERATIONVARSF = "iterationvarsf";

    // item type names
    static constexpr const char *PARAMETER = "parameter";
    static constexpr const char *SCALAR = "scalar";
    static constexpr const char *VECTOR = "vector";
    static constexpr const char *STATISTICS = "statistics";
    static constexpr const char *HISTOGRAM = "histogram";

    // frequently used result attributes
    static constexpr const char *TYPE = "type";
    static constexpr const char *ENUM = "enum";
    static constexpr const char *UNIT = "unit";
    static constexpr const char *INTERPOLATIONMODE = "interpolationmode";

    static constexpr const char *ISFIELD = "isfield";
    static constexpr const char *TRUE = "true";
    static constexpr const char *FALSE = "false";

    // derived scalar suffixes
    static constexpr const char *COUNT = "count";
    static constexpr const char *SUM = "sum";
    static constexpr const char *SUMWEIGHTS = "sumweights";
    static constexpr const char *MEAN = "mean";
    static constexpr const char *STDDEV = "stddev";
    static constexpr const char *MIN = "min";
    static constexpr const char *MAX = "max";
    static constexpr const char *NUMBINS = "numbins";
    static constexpr const char *RANGEMIN = "rangemin";
    static constexpr const char *RANGEMAX = "rangemax";
    static constexpr const char *UNDERFLOWS = "underflows";
    static constexpr const char *OVERFLOWS = "overflows";
    static constexpr const char *STARTTIME = "starttime";
    static constexpr const char *ENDTIME = "endtime";

};



}  // namespace scave
}  // namespace omnetpp


#endif

