//=========================================================================
//  RESULTITEMS.CC - part of
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

#include "resultitems.h"
#include "resultfilemanager.h"

using namespace std;
using namespace omnetpp::common;

namespace omnetpp {
namespace scave {

const std::string NULLSTRING = "";

ResultItem::ResultItem(FileRun *fileRun, const std::string& moduleName, const std::string& name, const StringMap& attrs) :
        fileRunRef(fileRun), moduleNameRef(nullptr), nameRef(nullptr), attributes(attrs)
{
    ResultFileManager *resultFileManager = fileRun->fileRef->getResultFileManager();
    moduleNameRef = resultFileManager->moduleNames.insert(moduleName);
    nameRef = resultFileManager->names.insert(name);
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

ResultItem::DataType ResultItem::getDataType() const
{
    StringMap::const_iterator it = attributes.find("type");
    if (it == attributes.end()) {
        if (attributes.find("enum") != attributes.end())
            return TYPE_ENUM;
        else
            return TYPE_DOUBLE;
    }
    else {
        const std::string& type = it->second;
        if (type == "int")
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
    StringMap::const_iterator it = attributes.find("enum");
    if (it != attributes.end()) {
        EnumType *enumPtr = new EnumType();
        enumPtr->parseFromString(it->second.c_str());
        return enumPtr;
    }
    else {
        return nullptr;
    }
}

int ScalarResult::getItemType() const
{
    return ResultFileManager::SCALAR;
}

int ParameterResult::getItemType() const
{
    return ResultFileManager::PARAMETER;
}

int VectorResult::getItemType() const
{
    return ResultFileManager::VECTOR;
}

InterpolationMode VectorResult::getInterpolationMode() const
{
    StringMap::const_iterator it = attributes.find("interpolationmode");
    if (it != attributes.end()) {
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
    else {
        return UNSPECIFIED;
    }
}

int StatisticsResult::getItemType() const
{
    return ResultFileManager::STATISTICS;
}

int HistogramResult::getItemType() const
{
    return ResultFileManager::HISTOGRAM;
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
    for (auto& p : configEntries)  // TODO some kind of ordered map would be better (e.g. std::map plus an std::vector<string> to store the order)
        if (p.first == key)
            return p.second;
    return NULLSTRING;
}

bool Run::isKeyParamAssignment(const std::string& key)
{
    size_t dotIndex = key.rfind(".");
    if (dotIndex == std::string::npos)
        return false;
    std::string afterDot = key.substr(dotIndex+1);
    return afterDot != "typename" && afterDot.find("-") == std::string::npos;
}

bool Run::isKeyGlobalConfigOption(const std::string& key)
{
    return key.find(".") == std::string::npos;
}

const OrderedKeyValueList Run::getParamAssignments() const
{
    OrderedKeyValueList result;
    for (auto& p : configEntries)
        if (isKeyParamAssignment(p.first))
            result.push_back(p);
    return result;
}

const std::string& Run::getParamAssignment(const std::string& key) const
{
    return isKeyParamAssignment(key) ? getConfigValue(key) : NULLSTRING;
}

const OrderedKeyValueList Run::getNonParamAssignmentConfigEntries() const
{
    OrderedKeyValueList result;
    for (auto& p : configEntries)
        if (!isKeyParamAssignment(p.first))
            result.push_back(p);
    return result;
}

const std::string& Run::getNonParamAssignmentConfigEntry(const std::string& key) const
{
    return !isKeyParamAssignment(key) ? getConfigValue(key) : NULLSTRING;
}

}  // namespace scave
}  // namespace omnetpp

