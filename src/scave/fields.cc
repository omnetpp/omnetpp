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

/*----------------------------------------
 *            ResultItemAttribute
 *----------------------------------------*/
const char *const ResultItemAttribute::TYPE = "type";
const char *const ResultItemAttribute::ENUM = "enum";

const StringVector& ResultItemAttribute::getAttributeNames()
{
    static StringVector names { TYPE, ENUM };
    return names;
}

bool ResultItemAttribute::isAttributeName(const string& name)
{
    return name == TYPE || name == ENUM;
}

/*----------------------------------------
 *              RunAttribute
 *----------------------------------------*/

//TODO: table to be kept consistent with ConfigVarDescription table in sectionbasedconfig.cc; use common source?
const char * const RunAttribute::INIFILE     = "inifile";
const char * const RunAttribute::CONFIGNAME  = "configname";
const char * const RunAttribute::RUNNUMBER   = "runnumber";
const char * const RunAttribute::NETWORK     = "network";
const char * const RunAttribute::EXPERIMENT  = "experiment";
const char * const RunAttribute::MEASUREMENT = "measurement";
const char * const RunAttribute::REPLICATION = "replication";
const char * const RunAttribute::DATETIME    = "datetime";
const char * const RunAttribute::PROCESSID   = "processid";
const char * const RunAttribute::RESULTDIR   = "resultdir";
const char * const RunAttribute::REPETITION  = "repetition";
const char * const RunAttribute::SEEDSET     = "seedset";
const char * const RunAttribute::ITERATIONVARS = "iterationvars";
const char * const RunAttribute::ITERATIONVARS2 = "iterationvars2"; // obsolete, no longer generated

const StringVector& RunAttribute::getAttributeNames()
{
    static StringVector names {
        INIFILE, CONFIGNAME, RUNNUMBER, NETWORK, EXPERIMENT, MEASUREMENT, REPLICATION,
        DATETIME, PROCESSID, RESULTDIR, REPETITION, SEEDSET, ITERATIONVARS, ITERATIONVARS2
    };
    return names;
}

bool RunAttribute::isAttributeName(const string& name)
{
    return name == INIFILE || name == CONFIGNAME || name == RUNNUMBER || name == NETWORK ||
           name == EXPERIMENT || name == MEASUREMENT || name == REPLICATION ||
           name == DATETIME || name == PROCESSID || name == RESULTDIR ||
           name == REPETITION || name == SEEDSET || name == ITERATIONVARS ||
           name == ITERATIONVARS2;
}

/*----------------------------------------
 *              ResultItemField
 *----------------------------------------*/
const char * const ResultItemField::FILE   = "file";
const char * const ResultItemField::RUN    = "run";
const char * const ResultItemField::MODULE = "module";
const char * const ResultItemField::NAME   = "name";

ResultItemField::ResultItemField(const string& fieldName)
{
    this->id = getFieldID(fieldName);
    this->name = fieldName;
}

int ResultItemField::getFieldID(const string& fieldName)
{
    if (fieldName == ResultItemField::FILE)
        return ResultItemField::FILE_ID;
    else if (fieldName == ResultItemField::RUN)
        return ResultItemField::RUN_ID;
    else if (fieldName == ResultItemField::MODULE)
        return ResultItemField::MODULE_ID;
    else if (fieldName == ResultItemField::NAME)
        return ResultItemField::NAME_ID;
    else if (ResultItemAttribute::isAttributeName(fieldName))
        return ResultItemField::ATTR_ID;
    else if (fieldName.find_first_of('.') != string::npos)
        return ResultItemField::RUN_PARAM_ID;
    else if (RunAttribute::isAttributeName(fieldName))
        return ResultItemField::RUN_ATTR_ID;
    else
        return ResultItemField::RUN_ITERVAR_ID;
}

/*----------------------------------------
 *              ResultItemFields
 *----------------------------------------*/

const StringVector& ResultItemFields::getFieldNames()
{
    static StringVector names;
    if (names.empty()) {
        addAll(names, StringVector { ResultItemField::FILE, ResultItemField::RUN, ResultItemField::MODULE, ResultItemField::NAME });
        addAll(names, ResultItemAttribute::getAttributeNames());
        addAll(names, RunAttribute::getAttributeNames());
    }
    return names;
}

ResultItemFields::ResultItemFields(ResultItemField field)
{
    this->fields.push_back(field);
}

ResultItemFields::ResultItemFields(const string& fieldName)
{
    this->fields.push_back(ResultItemField(fieldName));
}

ResultItemFields::ResultItemFields(const StringVector& fieldNames)
{
    for (StringVector::const_iterator fieldName = fieldNames.begin(); fieldName != fieldNames.end(); ++fieldName)
        this->fields.push_back(ResultItemField(*fieldName));
}

bool ResultItemFields::hasField(ResultItemField field) const
{
    return hasField(field.getName());
}

bool ResultItemFields::hasField(const string& fieldName) const
{
    for (vector<ResultItemField>::const_iterator field = fields.begin(); field != fields.end(); ++field)
        if (field->getName() == fieldName)
            return true;
    return false;
}

ResultItemFields ResultItemFields::complement() const
{
    StringVector complementFields;
    StringVector fieldNames = getFieldNames();
    for (StringVector::const_iterator fieldName = fieldNames.begin(); fieldName != fieldNames.end(); ++fieldName)
        if (!hasField(*fieldName))
            complementFields.push_back(*fieldName);
    return ResultItemFields(complementFields);
}

bool ResultItemFields::equal(ID id1, ID id2, ResultFileManager *manager) const
{
    if (id1 == -1 || id2 == -1 || id1 == id2)
        return id1 == id2;
    const ResultItem& d1 = manager->getItem(id1);
    const ResultItem& d2 = manager->getItem(id2);
    return equal(d1, d2);
}

bool ResultItemFields::equal(const ResultItem& d1, const ResultItem& d2) const
{
    for (vector<ResultItemField>::const_iterator field = fields.begin(); field != fields.end(); ++field)
        if (!field->equal(d1, d2))
            return false;
    return true;
}

bool ResultItemFields::less(ID id1, ID id2, ResultFileManager *manager) const
{
    if (id1 == -1 || id2 == -1)
        return id2 != -1;  // -1 is the smallest
    if (id1 == id2)
        return false;
    const ResultItem& d1 = manager->getItem(id1);
    const ResultItem& d2 = manager->getItem(id2);
    return less(d1, d2);
}

bool ResultItemFields::less(const ResultItem& d1, const ResultItem& d2) const
{
    for (vector<ResultItemField>::const_iterator field = fields.begin(); field != fields.end(); ++field) {
        int cmp = field->compare(d1, d2);
        if (cmp)
            return cmp < 0;
    }
    return false;  // ==
}

}  // namespace scave
}  // namespace omnetpp

