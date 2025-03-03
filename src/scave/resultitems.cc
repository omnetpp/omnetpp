//=========================================================================
//  RESULTITEMS.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
   `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "resultitems.h"
#include "resultfilemanager.h"
#include "fields.h"  // name constants

using namespace std;
using namespace omnetpp::common;

namespace omnetpp {
namespace scave {

// The value initialization makes it a definition, not just a declaration.
extern const SCAVE_API std::string NULLSTRING = "";

ResultItem::ResultItem(FileRun *fileRun, const std::string& moduleName, const std::string& name, const StringMap& attrs):
    fileRunRef(fileRun)
{
    ResultFileManager *resultFileManager = fileRun->fileRef->getResultFileManager();
    moduleNameRef = resultFileManager->moduleNames.insert(moduleName);
    nameRef = resultFileManager->names.insert(name);

    setAttributes(attrs);
}

void ResultItem::setAttributes(const StringMap& attrs)
{
    ResultFileManager *resultFileManager = fileRunRef->fileRef->getResultFileManager();
    auto it = resultFileManager->attrsPool.find(&attrs);
    if (it != resultFileManager->attrsPool.end())
        attributes = *it;
    else
        resultFileManager->attrsPool.insert(attributes = new StringMap(attrs));
}

void ResultItem::setAttribute(const std::string& attrName, const std::string& value)
{
    // TODO this is not terribly effective; it would be better to eliminate the need for this function altogether
    StringMap tmp = *attributes; // make a copy
    tmp[attrName] = value;
    setAttributes(tmp);
}

ResultItem& ResultItem::operator=(const ResultItem& rhs)
{
    if (this == &rhs)
        return *this;

    fileRunRef = rhs.fileRunRef;
    moduleNameRef = rhs.moduleNameRef;
    nameRef = rhs.nameRef;
    attributes = rhs.attributes;

    return *this;
}

ResultFileManager *ResultItem::getResultFileManager() const
{
    return fileRunRef->getFile()->getResultFileManager();
}

const char *ResultItem::itemTypeToString(int type)
{
    switch (type) {
        case ResultFileManager::PARAMETER: return Scave::PARAMETER;
        case ResultFileManager::SCALAR: return Scave::SCALAR;
        case ResultFileManager::VECTOR: return Scave::VECTOR;
        case ResultFileManager::STATISTICS: return Scave::STATISTICS;
        case ResultFileManager::HISTOGRAM: return Scave::HISTOGRAM;
        default: return "?";
    }
}

const char *ResultItem::getProperty(const char *propertyName) const
{
    switch (propertyName[0]) {
        case Scave::TYPE[0]: {
            if (strcmp(propertyName, Scave::TYPE) == 0)
                return itemTypeToString(getItemType());
            break;
        }
        case Scave::MODULE[0]: {
            if (strcmp(propertyName, Scave::MODULE) == 0)
                return getModuleName().c_str();
            break;
        }
        case Scave::NAME[0]: {
            if (strcmp(propertyName, Scave::NAME) == 0)
                return getName().c_str();
            break;
        }
        case Scave::FILE[0]: {
            if (strcmp(propertyName, Scave::FILE) == 0)
                return getFileRun()->getFile()->getFileName().c_str();
            break;
        }
        case Scave::RUN[0]: { // and RUNATTR_PREFIX
            static_assert(Scave::RUN[0] == Scave::RUNATTR_PREFIX[0], "initial letter mismatch");
            if (strcmp(propertyName, Scave::RUN) == 0)
                return getFileRun()->getRun()->getRunName().c_str();
            if (strncmp(propertyName, Scave::RUNATTR_PREFIX, strlen(Scave::RUNATTR_PREFIX)) == 0)
                return getFileRun()->getRun()->getAttribute(propertyName + strlen(Scave::RUNATTR_PREFIX)).c_str();
            break;
        }
        case Scave::ISFIELD[0]: { // and ITERVAR_PREFIX
            static_assert(Scave::ISFIELD[0] == Scave::ITERVAR_PREFIX[0], "initial letter mismatch");
            if (strcmp(propertyName, Scave::ISFIELD) == 0)
                return (getItemType() == ResultFileManager::SCALAR && static_cast<const ScalarResult *>(this)->isField()) ? Scave::TRUE : Scave::FALSE;
            if (strncmp(propertyName, Scave::ITERVAR_PREFIX, strlen(Scave::ITERVAR_PREFIX)) == 0)
                return getFileRun()->getRun()->getIterationVariable(propertyName + strlen(Scave::ITERVAR_PREFIX)).c_str();
            break;
        }
        case Scave::CONFIG_PREFIX[0]: {
            if (strncmp(propertyName, Scave::CONFIG_PREFIX, strlen(Scave::CONFIG_PREFIX)) == 0)
                return getFileRun()->getRun()->getConfigValue(propertyName + strlen(Scave::CONFIG_PREFIX)).c_str();
            break;
        }
        case Scave::ATTR_PREFIX[0]: {
            if (strncmp(propertyName, Scave::ATTR_PREFIX, strlen(Scave::ATTR_PREFIX)) == 0)
                return getAttribute(propertyName + strlen(Scave::ATTR_PREFIX)).c_str();
            break;
        }
    }
    throw opp_runtime_error("ResultItem::getProperty(): unrecognized property name '%s'", propertyName);
}

ResultItem::DataType ResultItem::getDataType() const
{
    auto it = attributes->find("type");
    if (it == attributes->end()) {
        if (attributes->find("enum") != attributes->end())
            return TYPE_ENUM;
        else
            return TYPE_DOUBLE;
    }
    else {
        const std::string& type = it->second;
        if (type == "bool")
            return TYPE_BOOL;
        else if (type == "int")
            return TYPE_INT;
        else if (type == "double")
            return TYPE_DOUBLE;
        else if (type == "enum")
            return TYPE_ENUM;
        else
            throw opp_runtime_error("Unknown type: %s", type.c_str());
    }
}

EnumType *ResultItem::getEnum() const
{
   auto it = attributes->find("enum");
    if (it == attributes->end())
        return nullptr;
    EnumType *enumPtr = new EnumType();
    enumPtr->parseFromString(it->second.c_str());
    return enumPtr;
}

int ScalarResult::getItemType() const
{
    return ResultFileManager::SCALAR;
}

bool ScalarResult::isField() const
{
    return ResultFileManager::isField(ownID);
}

const ResultItem *ScalarResult::getContainingItem() const
{
    return getResultFileManager()->getContainingItem(ownID);
}

double ScalarResult::getScalarField(FieldNum fieldId) const
{
    throw opp_runtime_error("ScalarResult has no fields");
}

int ParameterResult::getItemType() const
{
    return ResultFileManager::PARAMETER;
}

double ParameterResult::getScalarField(FieldNum fieldId) const
{
    throw opp_runtime_error("ParameterResult has no fields");
}

int VectorResult::getItemType() const
{
    return ResultFileManager::VECTOR;
}

InterpolationMode VectorResult::getInterpolationMode() const
{
    auto it = attributes->find("interpolationmode");
    if (it == attributes->end())
        return UNSPECIFIED;
    const std::string& mode = it->second;
    if (mode == "none")
        return NONE;
    else if (mode == "sample-hold")
        return SAMPLE_HOLD;
    else if (mode == "backward-sample-hold")
        return BACKWARD_SAMPLE_HOLD;
    else if (mode == "linear")
        return LINEAR;
    else
        throw opp_runtime_error("Unknown interpolation mode: %s", mode.c_str());
}

double VectorResult::getScalarField(FieldNum fieldId) const
{
    switch (fieldId) {
    case FieldNum::COUNT:  return stat.getCount();
    case FieldNum::SUM:  return stat.getSum();
    case FieldNum::SUMWEIGHTS:  return stat.getSumWeights();
    case FieldNum::MEAN:  return stat.getMean();
    case FieldNum::STDDEV:  return stat.getStddev();
    case FieldNum::MIN:  return stat.getMin();
    case FieldNum::MAX:  return stat.getMax();
    case FieldNum::STARTTIME:  return startTime.dbl();
    case FieldNum::ENDTIME:  return endTime.dbl();
    default: throw opp_runtime_error("Invalid fieldID");
    }
}

ResultItem::FieldNum *VectorResult::getAvailableFields()
{
    static FieldNum array[] = {
            FieldNum::COUNT, FieldNum::SUM, FieldNum::SUMWEIGHTS, FieldNum::MEAN, FieldNum::STDDEV,
            FieldNum::MIN, FieldNum::MAX, FieldNum::STARTTIME, FieldNum::ENDTIME, FieldNum::NONE
    };
    return array;
}

int StatisticsResult::getItemType() const
{
    return ResultFileManager::STATISTICS;
}

double StatisticsResult::getScalarField(FieldNum fieldId) const
{
    switch (fieldId) {
    case FieldNum::COUNT:  return stat.getCount();
    case FieldNum::SUM:  return stat.getSum();
    case FieldNum::SUMWEIGHTS:  return stat.getSumWeights();
    case FieldNum::MEAN:  return stat.getMean();
    case FieldNum::STDDEV:  return stat.getStddev();
    case FieldNum::MIN:  return stat.getMin();
    case FieldNum::MAX:  return stat.getMax();
    case FieldNum::NUMBINS:  return 0;
    default: throw opp_runtime_error("Invalid fieldID");
    }
}

ResultItem::FieldNum *StatisticsResult::getAvailableFields()
{
    static FieldNum array[] = {
            FieldNum::COUNT, FieldNum::SUMWEIGHTS, FieldNum::MEAN, FieldNum::STDDEV, // FieldNum::SUM is not supported in the weighted case
            FieldNum::MIN, FieldNum::MAX, FieldNum::NUMBINS, FieldNum::NONE
    };
    return array;
}

int HistogramResult::getItemType() const
{
    return ResultFileManager::HISTOGRAM;
}

double HistogramResult::getScalarField(FieldNum fieldId) const
{
    switch (fieldId) {
    case FieldNum::COUNT:
    case FieldNum::SUM:
    case FieldNum::SUMWEIGHTS:
    case FieldNum::MEAN:
    case FieldNum::STDDEV:
    case FieldNum::MIN:
    case FieldNum::MAX: return StatisticsResult::getScalarField(fieldId);

    case FieldNum::NUMBINS: return bins.getNumBins();
    case FieldNum::RANGEMIN: return bins.getBinEdge(0);
    case FieldNum::RANGEMAX: return bins.getBinEdge(bins.getNumBins());
    case FieldNum::UNDERFLOWS: return bins.getUnderflows();
    case FieldNum::OVERFLOWS: return bins.getOverflows();
    default: throw opp_runtime_error("Invalid fieldID");
    }
}

ResultItem::FieldNum *HistogramResult::getAvailableFields()
{
    static FieldNum array[] = {
            FieldNum::COUNT, FieldNum::SUMWEIGHTS, FieldNum::MEAN, FieldNum::STDDEV, FieldNum::MIN, FieldNum::MAX, // FieldNum::SUM is not supported in the weighted case
            FieldNum::NUMBINS, FieldNum::RANGEMIN, FieldNum::RANGEMAX, FieldNum::UNDERFLOWS, FieldNum::OVERFLOWS, FieldNum::NONE
    };
    return array;
}

const std::string& Run::getAttribute(const std::string& attrName) const
{
    auto it = attributes.find(attrName);
    return it==attributes.end() ? NULLSTRING : it->second;
}

const std::string& Run::getIterationVariable(const std::string& name) const
{
    auto it = itervars.find(name);
    return it==itervars.end() ? NULLSTRING : it->second;
}

const std::string& Run::getConfigValue(const std::string& key) const
{
    for (auto& p : configEntries)  // linear search -- this method is not used much (not too useful), so it should be OK
        if (p.first == key)
            return p.second;
    return NULLSTRING;
}

bool Run::isParamAssignmentConfigKey(const std::string& key)
{
    size_t dotIndex = key.rfind(".");
    if (dotIndex == std::string::npos)
        return false;
    std::string afterDot = key.substr(dotIndex+1);
    return afterDot != "typename" && afterDot.find("-") == std::string::npos;
}

bool Run::isGlobalOptionConfigKey(const std::string& key)
{
    return key.find(".") == std::string::npos;
}

const OrderedKeyValueList Run::getParamAssignmentConfigEntries() const
{
    OrderedKeyValueList result;
    for (auto& p : configEntries)
        if (isParamAssignmentConfigKey(p.first))
            result.push_back(p);
    return result;
}

const OrderedKeyValueList Run::getNonParamAssignmentConfigEntries() const
{
    OrderedKeyValueList result;
    for (auto& p : configEntries)
        if (!isParamAssignmentConfigKey(p.first))
            result.push_back(p);
    return result;
}

const char *Run::getProperty(const char *propertyName) const
{
    switch (propertyName[0]) {
        case Scave::RUN[0]: { // and RUNATTR_PREFIX
            static_assert(Scave::RUN[0] == Scave::RUNATTR_PREFIX[0], "initial letter mismatch");
            if (strcmp(propertyName, Scave::RUN) == 0)
                return getRunName().c_str();
            if (strncmp(propertyName, Scave::RUNATTR_PREFIX, strlen(Scave::RUNATTR_PREFIX)) == 0)
                return getAttribute(propertyName + strlen(Scave::RUNATTR_PREFIX)).c_str();
            break;
        }
        case Scave::ITERVAR_PREFIX[0]: {
            if (strncmp(propertyName, Scave::ITERVAR_PREFIX, strlen(Scave::ITERVAR_PREFIX)) == 0)
                return getIterationVariable(propertyName + strlen(Scave::ITERVAR_PREFIX)).c_str();
            break;
        }
        case Scave::CONFIG_PREFIX[0]: {
            if (strncmp(propertyName, Scave::CONFIG_PREFIX, strlen(Scave::CONFIG_PREFIX)) == 0)
                return getConfigValue(propertyName + strlen(Scave::CONFIG_PREFIX)).c_str();
            break;
        }
    }
    throw opp_runtime_error("Run::getProperty(): unrecognized property name '%s'", propertyName);
}


}  // namespace scave
}  // namespace omnetpp

