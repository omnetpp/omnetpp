//==========================================================================
//  FIELDS.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Tamas Borbely
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include "common/commonutil.h"
#include "common/stringutil.h"
#include "common/stlutil.h"
#include "fields.h"

using namespace std;
using namespace omnetpp::common;

namespace omnetpp {
namespace scave {

const StringVector& Scave::getRunAttributeNames()
{
    static StringVector names {
        Scave::INIFILE, Scave::CONFIGNAME, Scave::RUNNUMBER, Scave::NETWORK, Scave::EXPERIMENT, Scave::MEASUREMENT, Scave::REPLICATION,
        Scave::DATETIME, Scave::PROCESSID, Scave::RESULTDIR, Scave::REPETITION, Scave::SEEDSET, Scave::ITERATIONVARS, Scave::ITERATIONVARSF
    };
    return names;
}

bool Scave::isRunAttributeName(const string& name)
{
    return contains(getRunAttributeNames(),  name);
}

const StringVector& Scave::getResultItemAttributeNames()
{
    static StringVector names { Scave::TYPE, Scave::ENUM };
    return names;
}

bool Scave::isResultItemAttributeName(const string& name)
{
    return name == Scave::TYPE || name == Scave::ENUM;
}

const StringVector& Scave::getResultItemFieldNames()
{
    static StringVector names;
    if (names.empty()) {
        addAll(names, StringVector { Scave::FILE, Scave::RUN, Scave::MODULE, Scave::NAME });
        addAll(names, getResultItemAttributeNames());
        addAll(names, getRunAttributeNames());
    }
    return names;
}

//-----------

ResultItemField::ResultItemField(const string& fieldName)
{
    this->id = getFieldID(fieldName);
    this->name = fieldName;
}

int ResultItemField::getFieldID(const string& fieldName)
{
    if (fieldName == Scave::FILE)
        return ResultItemField::FILE_ID;
    else if (fieldName == Scave::RUN)
        return ResultItemField::RUN_ID;
    else if (fieldName == Scave::MODULE)
        return ResultItemField::MODULE_ID;
    else if (fieldName == Scave::NAME)
        return ResultItemField::NAME_ID;
    else if (Scave::isResultItemAttributeName(fieldName))
        return ResultItemField::ATTR_ID;
    else if (fieldName.find_first_of('.') != string::npos)
        return ResultItemField::RUN_CONFIG_ID;
    else if (Scave::isRunAttributeName(fieldName))
        return ResultItemField::RUN_ATTR_ID;
    else
        return ResultItemField::RUN_ITERVAR_ID;
}

const std::string& ResultItemField::getFieldValue(const ResultItem& d) const
{
    switch(id) {
        case FILE_ID:       return d.getFile()->getFilePath();
        case RUN_ID:        return d.getRun()->getRunName();
        case MODULE_ID:     return d.getModuleName();
        case NAME_ID:       return d.getName();
        case ATTR_ID:       return d.getAttribute(name);
        case RUN_ATTR_ID:   return d.getRun()->getAttribute(name);
        case RUN_ITERVAR_ID:return d.getRun()->getIterationVariable(name);
        case RUN_CONFIG_ID: return d.getRun()->getConfigValue(name);
        default: throw opp_runtime_error("unknown result item");
    }
}


}  // namespace scave
}  // namespace omnetpp

