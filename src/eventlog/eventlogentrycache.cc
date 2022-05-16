//=========================================================================
//  EVENTLOGENTRYCACHE.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cstring>
#include "common/exception.h"
#include "eventlogentrycache.h"

namespace omnetpp {
namespace eventlog {

EventLogEntryCache::EventLogEntryCache()
{
    // module
    this->moduleIdToModuleDescriptionEntryMap = nullptr;
    this->moduleIdToModuleCreatedEntryMap = nullptr;
    this->moduleIdToModuleDeletedEntryMap = nullptr;
    this->moduleIdToModuleDisplayStringChangedVectorMap = nullptr;
    // connection
    this->moduleIdAndGateIdToConnectionDescriptionEntryMap = nullptr;
    this->moduleIdAndGateIdToConnectionCreatedEntryMap = nullptr;
    this->moduleIdAndGateIdToConnectionDeletedEntryMap = nullptr;
    this->moduleIdAndGateIdToConnectionDisplayStringChangedVectorMap = nullptr;
    // gate
    this->moduleIdAndGateIdToGateDescriptionEntryMap = nullptr;
    this->moduleIdAndGateIdToGateCreatedEntryMap = nullptr;
    this->moduleIdAndGateIdToGateDeletedEntryMap = nullptr;
    this->moduleIdAndGateIdToGateDisplayStringChangedVectorMap = nullptr;
    // custom
    this->typeAndKeyToCustomDescriptionEntryMap = nullptr;
    this->typeAndKeyToCustomCreatedEntryMap = nullptr;
    this->typeAndKeyToCustomDeletedEntryMap = nullptr;
    this->typeAndKeyToCustomChangedVectorMap = nullptr;

}

EventLogEntryCache::~EventLogEntryCache()
{
    // module
    delete moduleIdToModuleDescriptionEntryMap;
    delete moduleIdToModuleCreatedEntryMap;
    delete moduleIdToModuleDeletedEntryMap;
    delete moduleIdToModuleDisplayStringChangedVectorMap;
    // connection
    delete moduleIdAndGateIdToConnectionDescriptionEntryMap;
    delete moduleIdAndGateIdToConnectionCreatedEntryMap;
    delete moduleIdAndGateIdToConnectionDeletedEntryMap;
    delete moduleIdAndGateIdToConnectionDisplayStringChangedVectorMap;
    // gate
    delete moduleIdAndGateIdToGateDescriptionEntryMap;
    delete moduleIdAndGateIdToGateCreatedEntryMap;
    delete moduleIdAndGateIdToGateDeletedEntryMap;
    delete moduleIdAndGateIdToGateDisplayStringChangedVectorMap;
    // custom
    delete typeAndKeyToCustomDescriptionEntryMap;
    delete typeAndKeyToCustomCreatedEntryMap;
    delete typeAndKeyToCustomDeletedEntryMap;
    delete typeAndKeyToCustomChangedVectorMap;
}

void EventLogEntryCache::clearCache()
{
    // module
    if (moduleIdToModuleDescriptionEntryMap)
        moduleIdToModuleDescriptionEntryMap->clear();
    if (moduleIdToModuleCreatedEntryMap)
        moduleIdToModuleCreatedEntryMap->clear();
    if (moduleIdToModuleDeletedEntryMap)
        moduleIdToModuleDeletedEntryMap->clear();
    if (moduleIdToModuleDisplayStringChangedVectorMap)
        moduleIdToModuleDisplayStringChangedVectorMap->clear();
    // connection
    if (moduleIdAndGateIdToConnectionDescriptionEntryMap)
        moduleIdAndGateIdToConnectionDescriptionEntryMap->clear();
    if (moduleIdAndGateIdToConnectionCreatedEntryMap)
        moduleIdAndGateIdToConnectionCreatedEntryMap->clear();
    if (moduleIdAndGateIdToConnectionDeletedEntryMap)
        moduleIdAndGateIdToConnectionDeletedEntryMap->clear();
    if (moduleIdAndGateIdToConnectionDisplayStringChangedVectorMap)
        moduleIdAndGateIdToConnectionDisplayStringChangedVectorMap->clear();
    // gate
    if (moduleIdAndGateIdToGateDescriptionEntryMap)
        moduleIdAndGateIdToGateDescriptionEntryMap->clear();
    if (moduleIdAndGateIdToGateCreatedEntryMap)
        moduleIdAndGateIdToGateCreatedEntryMap->clear();
    if (moduleIdAndGateIdToGateDeletedEntryMap)
        moduleIdAndGateIdToGateDeletedEntryMap->clear();
    if (moduleIdAndGateIdToGateDisplayStringChangedVectorMap)
        moduleIdAndGateIdToGateDisplayStringChangedVectorMap->clear();
    // custom
    if (typeAndKeyToCustomDescriptionEntryMap)
        typeAndKeyToCustomDescriptionEntryMap->clear();
    if (typeAndKeyToCustomCreatedEntryMap)
        typeAndKeyToCustomCreatedEntryMap->clear();
    if (typeAndKeyToCustomDeletedEntryMap)
        typeAndKeyToCustomDeletedEntryMap->clear();
    if (typeAndKeyToCustomChangedVectorMap)
        typeAndKeyToCustomChangedVectorMap->clear();
}

void EventLogEntryCache::allocateCache() {
    // module
    moduleIdToModuleDescriptionEntryMap = new ModuleIdToModuleDescriptionEntryMap();
    moduleIdToModuleCreatedEntryMap = new ModuleIdToModuleCreatedEntryMap();
    moduleIdToModuleDeletedEntryMap = new ModuleIdToModuleDeletedEntryMap();
    moduleIdToModuleDisplayStringChangedVectorMap = new ModuleIdToModuleDisplayStringChangedVectorMap();
    // connection
    moduleIdAndGateIdToConnectionDescriptionEntryMap = new ModuleIdAndGateIdToConnectionDescriptionEntryMap();
    moduleIdAndGateIdToConnectionCreatedEntryMap = new ModuleIdAndGateIdToConnectionCreatedEntryMap();
    moduleIdAndGateIdToConnectionDeletedEntryMap = new ModuleIdAndGateIdToConnectionDeletedEntryMap();
    moduleIdAndGateIdToConnectionDisplayStringChangedVectorMap = new ModuleIdAndGateIdToConnectionDisplayStringChangedVectorMap();
    // gate
    moduleIdAndGateIdToGateDescriptionEntryMap = new ModuleIdAndGateIdToGateDescriptionEntryMap();
    moduleIdAndGateIdToGateCreatedEntryMap = new ModuleIdAndGateIdToGateCreatedEntryMap();
    moduleIdAndGateIdToGateDeletedEntryMap = new ModuleIdAndGateIdToGateDeletedEntryMap();
    moduleIdAndGateIdToGateDisplayStringChangedVectorMap = new ModuleIdAndGateIdToGateDisplayStringChangedVectorMap();
    // custom
    typeAndKeyToCustomDescriptionEntryMap = new TypeAndKeyToCustomDescriptionEntryMap();
    typeAndKeyToCustomCreatedEntryMap = new TypeAndKeyToCustomCreatedEntryMap();
    typeAndKeyToCustomDeletedEntryMap = new TypeAndKeyToCustomDeletedEntryMap();
    typeAndKeyToCustomChangedVectorMap = new TypeAndKeyToCustomChangedVectorMap();
}

void EventLogEntryCache::buildCache() {
    ensureCacheAllocated();
    for (EventLogEntryList::iterator it = begin(); it != end(); it++)
        cacheEventLogEntry(*it);
}

void EventLogEntryCache::ensureCacheAllocated()
{
    if (!moduleIdToModuleCreatedEntryMap)
        allocateCache();
}

void EventLogEntryCache::ensureCacheBuilt()
{
    if (!moduleIdToModuleCreatedEntryMap)
        buildCache();
}

void EventLogEntryCache::cacheEventLogEntry(EventLogEntry *eventLogEntry)
{
    // TODO: this will always allocate all structures and that's not what we need
    ensureCacheAllocated();
    // description entries
    if (dynamic_cast<ModuleDescriptionEntry *>(eventLogEntry)) {
        ModuleDescriptionEntry *moduleDescriptionEntry = dynamic_cast<ModuleDescriptionEntry *>(eventLogEntry);
        moduleIdToModuleDescriptionEntryMap->operator[](moduleDescriptionEntry->moduleId) = moduleDescriptionEntry;
    }
    else if (dynamic_cast<ConnectionDescriptionEntry *>(eventLogEntry)) {
        ConnectionDescriptionEntry *connectionDescriptionEntry = dynamic_cast<ConnectionDescriptionEntry *>(eventLogEntry);
        moduleIdAndGateIdToConnectionDescriptionEntryMap->operator[](std::pair<int, int>(connectionDescriptionEntry->sourceModuleId, connectionDescriptionEntry->sourceGateId)) = connectionDescriptionEntry;
    }
    else if (dynamic_cast<GateDescriptionEntry *>(eventLogEntry)) {
        GateDescriptionEntry *gateDescriptionEntry = dynamic_cast<GateDescriptionEntry *>(eventLogEntry);
        moduleIdAndGateIdToGateDescriptionEntryMap->operator[](std::pair<int, int>(gateDescriptionEntry->moduleId, gateDescriptionEntry->gateId)) = gateDescriptionEntry;
    }
    else if (dynamic_cast<CustomDescriptionEntry *>(eventLogEntry)) {
        CustomDescriptionEntry *customDescriptionEntry = dynamic_cast<CustomDescriptionEntry *>(eventLogEntry);
        typeAndKeyToCustomDescriptionEntryMap->operator[](std::pair<std::string, long>(customDescriptionEntry->type, customDescriptionEntry->key)) = customDescriptionEntry;
    }
    // module
    if (dynamic_cast<ModuleCreatedEntry *>(eventLogEntry)) {
        ModuleCreatedEntry *moduleCreatedEntry = dynamic_cast<ModuleCreatedEntry *>(eventLogEntry);
        moduleIdToModuleCreatedEntryMap->operator[](moduleCreatedEntry->moduleId) = moduleCreatedEntry;
    }
    else if (dynamic_cast<ModuleDeletedEntry *>(eventLogEntry)) {
        ModuleDeletedEntry *moduleDeletedEntry = dynamic_cast<ModuleDeletedEntry *>(eventLogEntry);
        moduleIdToModuleDeletedEntryMap->operator[](moduleDeletedEntry->moduleId) = moduleDeletedEntry;
    }
    else if (dynamic_cast<ModuleDisplayStringChangedEntry *>(eventLogEntry)) {
        ModuleDisplayStringChangedEntry *moduleDisplayStringChangedEntry = dynamic_cast<ModuleDisplayStringChangedEntry *>(eventLogEntry);
        ModuleIdToModuleDisplayStringChangedVectorMap::iterator jt = moduleIdToModuleDisplayStringChangedVectorMap->find(moduleDisplayStringChangedEntry->moduleId);
        if (jt != moduleIdToModuleDisplayStringChangedVectorMap->end())
            jt->second.push_back(moduleDisplayStringChangedEntry);
        else
            moduleIdToModuleDisplayStringChangedVectorMap->operator[](moduleDisplayStringChangedEntry->moduleId) = std::vector<ModuleDisplayStringChangedEntry *>(1, moduleDisplayStringChangedEntry);
    }
    // connection
    else if (dynamic_cast<ConnectionCreatedEntry *>(eventLogEntry)) {
        ConnectionCreatedEntry *gateCreatedEntry = dynamic_cast<ConnectionCreatedEntry *>(eventLogEntry);
        moduleIdAndGateIdToConnectionCreatedEntryMap->operator[](std::pair<int, int>(gateCreatedEntry->sourceModuleId, gateCreatedEntry->sourceGateId)) = gateCreatedEntry;
    }
    else if (dynamic_cast<ConnectionDeletedEntry *>(eventLogEntry)) {
        ConnectionDeletedEntry *gateDeletedEntry = dynamic_cast<ConnectionDeletedEntry *>(eventLogEntry);
        moduleIdAndGateIdToConnectionDeletedEntryMap->operator[](std::pair<int, int>(gateDeletedEntry->sourceModuleId, gateDeletedEntry->sourceGateId)) = gateDeletedEntry;
    }
    else if (dynamic_cast<ConnectionDisplayStringChangedEntry *>(eventLogEntry)) {
        ConnectionDisplayStringChangedEntry *gateDisplayStringChangedEntry = dynamic_cast<ConnectionDisplayStringChangedEntry *>(eventLogEntry);
        ModuleIdAndGateIdToConnectionDisplayStringChangedVectorMap::iterator jt = moduleIdAndGateIdToConnectionDisplayStringChangedVectorMap->find(std::pair<int, int>(gateDisplayStringChangedEntry->sourceModuleId, gateDisplayStringChangedEntry->sourceGateId));
        if (jt != moduleIdAndGateIdToConnectionDisplayStringChangedVectorMap->end())
            jt->second.push_back(gateDisplayStringChangedEntry);
        else
            moduleIdAndGateIdToConnectionDisplayStringChangedVectorMap->operator[](std::pair<int, int>(gateDisplayStringChangedEntry->sourceModuleId, gateDisplayStringChangedEntry->sourceGateId)) = std::vector<ConnectionDisplayStringChangedEntry *>(1, gateDisplayStringChangedEntry);
    }
    // gate
    else if (dynamic_cast<GateCreatedEntry *>(eventLogEntry)) {
        GateCreatedEntry *gateCreatedEntry = dynamic_cast<GateCreatedEntry *>(eventLogEntry);
        moduleIdAndGateIdToGateCreatedEntryMap->operator[](std::pair<int, int>(gateCreatedEntry->moduleId, gateCreatedEntry->gateId)) = gateCreatedEntry;
    }
    else if (dynamic_cast<GateDeletedEntry *>(eventLogEntry)) {
        GateDeletedEntry *gateDeletedEntry = dynamic_cast<GateDeletedEntry *>(eventLogEntry);
        moduleIdAndGateIdToGateDeletedEntryMap->operator[](std::pair<int, int>(gateDeletedEntry->moduleId, gateDeletedEntry->gateId)) = gateDeletedEntry;
    }
    else if (dynamic_cast<GateDisplayStringChangedEntry *>(eventLogEntry)) {
        GateDisplayStringChangedEntry *gateDisplayStringChangedEntry = dynamic_cast<GateDisplayStringChangedEntry *>(eventLogEntry);
        ModuleIdAndGateIdToGateDisplayStringChangedVectorMap::iterator jt = moduleIdAndGateIdToGateDisplayStringChangedVectorMap->find(std::pair<int, int>(gateDisplayStringChangedEntry->moduleId, gateDisplayStringChangedEntry->gateId));
        if (jt != moduleIdAndGateIdToGateDisplayStringChangedVectorMap->end())
            jt->second.push_back(gateDisplayStringChangedEntry);
        else
            moduleIdAndGateIdToGateDisplayStringChangedVectorMap->operator[](std::pair<int, int>(gateDisplayStringChangedEntry->moduleId, gateDisplayStringChangedEntry->gateId)) = std::vector<GateDisplayStringChangedEntry *>(1, gateDisplayStringChangedEntry);
    }
    // custom
    else if (dynamic_cast<CustomCreatedEntry *>(eventLogEntry)) {
        CustomCreatedEntry *customCreatedEntry = dynamic_cast<CustomCreatedEntry *>(eventLogEntry);
        typeAndKeyToCustomCreatedEntryMap->operator[](std::pair<std::string, long>(customCreatedEntry->type, customCreatedEntry->key)) = customCreatedEntry;
    }
    else if (dynamic_cast<CustomDeletedEntry *>(eventLogEntry)) {
        CustomDeletedEntry *customDeletedEntry = dynamic_cast<CustomDeletedEntry *>(eventLogEntry);
        typeAndKeyToCustomDeletedEntryMap->operator[](std::pair<std::string, long>(customDeletedEntry->type, customDeletedEntry->key)) = customDeletedEntry;
    }
    else if (dynamic_cast<CustomChangedEntry *>(eventLogEntry)) {
        CustomChangedEntry *customChangedEntry = dynamic_cast<CustomChangedEntry *>(eventLogEntry);
        TypeAndKeyToCustomChangedVectorMap::iterator jt = typeAndKeyToCustomChangedVectorMap->find(std::pair<std::string, long>(customChangedEntry->type, customChangedEntry->key));
        if (jt != typeAndKeyToCustomChangedVectorMap->end())
            jt->second.push_back(customChangedEntry);
        else
            typeAndKeyToCustomChangedVectorMap->operator[](std::pair<std::string, long>(customChangedEntry->type, customChangedEntry->key)) = std::vector<CustomChangedEntry *>(1, customChangedEntry);
    }
}

ModuleDescriptionEntry *EventLogEntryCache::getModuleDescriptionEntry(int moduleId)
{
    ensureCacheBuilt();
    if (moduleIdToModuleDescriptionEntryMap) {
        ModuleIdToModuleDescriptionEntryMap::iterator it = moduleIdToModuleDescriptionEntryMap->find(moduleId);
        if (it != moduleIdToModuleDescriptionEntryMap->end())
            return it->second;
    }
    else {
        for (EventLogEntryList::iterator it = begin(); it != end(); it++) {
            ModuleDescriptionEntry *moduleDescriptionEntry = dynamic_cast<ModuleDescriptionEntry *>(*it);
            if (moduleDescriptionEntry && moduleDescriptionEntry->moduleId == moduleId)
                return moduleDescriptionEntry;
        }
    }
    return nullptr;
}

std::vector<ModuleDescriptionEntry *> EventLogEntryCache::getModuleDescriptionEntries()
{
    ensureCacheBuilt();
    // TODO: linear search when the map is not filled
    std::vector<ModuleDescriptionEntry *> moduleDescriptionEntries;
    for (ModuleIdToModuleDescriptionEntryMap::iterator it = moduleIdToModuleDescriptionEntryMap->begin(); it != moduleIdToModuleDescriptionEntryMap->end(); it++)
        moduleDescriptionEntries.push_back(it->second);
    return moduleDescriptionEntries;
}

ModuleCreatedEntry *EventLogEntryCache::getModuleCreatedEntry(int moduleId)
{
    ensureCacheBuilt();
    if (moduleIdToModuleCreatedEntryMap) {
        ModuleIdToModuleCreatedEntryMap::iterator it = moduleIdToModuleCreatedEntryMap->find(moduleId);
        if (it != moduleIdToModuleCreatedEntryMap->end())
            return it->second;
    }
    else {
        for (EventLogEntryList::iterator it = begin(); it != end(); it++) {
            ModuleCreatedEntry *moduleCreatedEntry = dynamic_cast<ModuleCreatedEntry *>(*it);
            if (moduleCreatedEntry && moduleCreatedEntry->moduleId == moduleId)
                return moduleCreatedEntry;
        }
    }
    return nullptr;
}

ModuleDeletedEntry *EventLogEntryCache::getModuleDeletedEntry(int moduleId)
{
    ensureCacheBuilt();
    if (moduleIdToModuleDeletedEntryMap) {
        ModuleIdToModuleDeletedEntryMap::iterator it = moduleIdToModuleDeletedEntryMap->find(moduleId);
        if (it != moduleIdToModuleDeletedEntryMap->end())
            return it->second;
    }
    else {
        for (EventLogEntryList::iterator it = begin(); it != end(); it++) {
            ModuleDeletedEntry *moduleDeletedEntry = dynamic_cast<ModuleDeletedEntry *>(*it);
            if (moduleDeletedEntry && moduleDeletedEntry->moduleId == moduleId)
                return moduleDeletedEntry;
        }
    }
    return nullptr;
}

ModuleDisplayStringChangedEntry *EventLogEntryCache::getModuleDisplayStringChangedEntry(int moduleId, int entryIndex)
{
    ensureCacheBuilt();
    if (moduleIdToModuleDisplayStringChangedVectorMap) {
        ModuleIdToModuleDisplayStringChangedVectorMap::iterator it = moduleIdToModuleDisplayStringChangedVectorMap->find(moduleId);
        if (it != moduleIdToModuleDisplayStringChangedVectorMap->end()) {
            std::vector<ModuleDisplayStringChangedEntry *>& moduleDisplayStringChangedVector = it->second;
            for (std::vector<ModuleDisplayStringChangedEntry *>::iterator jt = moduleDisplayStringChangedVector.begin(); jt != moduleDisplayStringChangedVector.end(); jt++) {
                ModuleDisplayStringChangedEntry *moduleDisplayStringChangedEntry = *jt;
                if (moduleDisplayStringChangedEntry->getEntryIndex() >= entryIndex)
                    return moduleDisplayStringChangedEntry;
            }
        }
    }
    else {
        for (EventLogEntryList::iterator it = begin() + entryIndex; it != end(); it++) {
            ModuleDisplayStringChangedEntry *moduleDisplayStringChangedEntry = dynamic_cast<ModuleDisplayStringChangedEntry *>(*it);
            if (moduleDisplayStringChangedEntry && moduleDisplayStringChangedEntry->moduleId == moduleId)
                return moduleDisplayStringChangedEntry;
        }
    }
    return nullptr;
}

ConnectionDescriptionEntry *EventLogEntryCache::getConnectionDescriptionEntry(int sourceModuleId, int sourceGateId)
{
    ensureCacheBuilt();
    if (moduleIdAndGateIdToConnectionDescriptionEntryMap) {
        ModuleIdAndGateIdToConnectionDescriptionEntryMap::iterator it = moduleIdAndGateIdToConnectionDescriptionEntryMap->find(std::pair<int, int>(sourceModuleId, sourceGateId));
        if (it != moduleIdAndGateIdToConnectionDescriptionEntryMap->end())
            return it->second;
    }
    else {
        for (EventLogEntryList::iterator it = begin(); it != end(); it++) {
            ConnectionDescriptionEntry *connectionDescriptionEntry = dynamic_cast<ConnectionDescriptionEntry *>(*it);
            if (connectionDescriptionEntry && connectionDescriptionEntry->sourceModuleId == sourceModuleId && connectionDescriptionEntry->sourceGateId == sourceGateId)
                return connectionDescriptionEntry;
        }
    }
    return nullptr;
}

ConnectionCreatedEntry *EventLogEntryCache::getConnectionCreatedEntry(int sourceModuleId, int sourceGateId)
{
    ensureCacheBuilt();
    if (moduleIdAndGateIdToConnectionCreatedEntryMap) {
        ModuleIdAndGateIdToConnectionCreatedEntryMap::iterator it = moduleIdAndGateIdToConnectionCreatedEntryMap->find(std::pair<int, int>(sourceModuleId, sourceGateId));
        if (it != moduleIdAndGateIdToConnectionCreatedEntryMap->end())
            return it->second;
    }
    else {
        for (EventLogEntryList::iterator it = begin(); it != end(); it++) {
            ConnectionCreatedEntry *connectionCreatedEntry = dynamic_cast<ConnectionCreatedEntry *>(*it);
            if (connectionCreatedEntry && connectionCreatedEntry->sourceModuleId == sourceModuleId && connectionCreatedEntry->sourceGateId == sourceGateId)
                return connectionCreatedEntry;
        }
    }
    return nullptr;
}

ConnectionDeletedEntry *EventLogEntryCache::getConnectionDeletedEntry(int sourceModuleId, int sourceGateId)
{
    ensureCacheBuilt();
    if (moduleIdAndGateIdToConnectionDeletedEntryMap) {
        ModuleIdAndGateIdToConnectionDeletedEntryMap::iterator it = moduleIdAndGateIdToConnectionDeletedEntryMap->find(std::pair<int, int>(sourceModuleId, sourceGateId));
        if (it != moduleIdAndGateIdToConnectionDeletedEntryMap->end())
            return it->second;
    }
    else {
        for (EventLogEntryList::iterator it = begin(); it != end(); it++) {
            ConnectionDeletedEntry *connectionDeletedEntry = dynamic_cast<ConnectionDeletedEntry *>(*it);
            if (connectionDeletedEntry && connectionDeletedEntry->sourceModuleId == sourceModuleId && connectionDeletedEntry->sourceGateId == sourceGateId)
                return connectionDeletedEntry;
        }
    }
    return nullptr;
}

ConnectionDisplayStringChangedEntry *EventLogEntryCache::getConnectionDisplayStringChangedEntry(int sourceModuleId, int sourceGateId, int entryIndex)
{
    ensureCacheBuilt();
    if (moduleIdAndGateIdToConnectionDisplayStringChangedVectorMap) {
        ModuleIdAndGateIdToConnectionDisplayStringChangedVectorMap::iterator it = moduleIdAndGateIdToConnectionDisplayStringChangedVectorMap->find(std::pair<int, int>(sourceModuleId, sourceGateId));
        if (it != moduleIdAndGateIdToConnectionDisplayStringChangedVectorMap->end()) {
            std::vector<ConnectionDisplayStringChangedEntry *>& gateDisplayStringChangedVector = it->second;
            for (std::vector<ConnectionDisplayStringChangedEntry *>::iterator jt = gateDisplayStringChangedVector.begin(); jt != gateDisplayStringChangedVector.end(); jt++) {
                ConnectionDisplayStringChangedEntry *connectionDisplayStringChangedEntry = *jt;
                if (connectionDisplayStringChangedEntry->getEntryIndex() >= entryIndex)
                    return connectionDisplayStringChangedEntry;
            }
        }
    }
    else {
        for (EventLogEntryList::iterator it = begin() + entryIndex; it != end(); it++) {
            ConnectionDisplayStringChangedEntry *connectionDisplayStringChangedEntry = dynamic_cast<ConnectionDisplayStringChangedEntry *>(*it);
            if (connectionDisplayStringChangedEntry && connectionDisplayStringChangedEntry->sourceModuleId == sourceModuleId && connectionDisplayStringChangedEntry->sourceGateId == sourceGateId)
                return connectionDisplayStringChangedEntry;
        }
    }
    return nullptr;
}

GateDescriptionEntry *EventLogEntryCache::getGateDescriptionEntry(int moduleId, int gateId)
{
    ensureCacheBuilt();
    if (moduleIdAndGateIdToGateDescriptionEntryMap) {
        ModuleIdAndGateIdToGateDescriptionEntryMap::iterator it = moduleIdAndGateIdToGateDescriptionEntryMap->find(std::pair<int, int>(moduleId, gateId));
        if (it != moduleIdAndGateIdToGateDescriptionEntryMap->end())
            return it->second;
    }
    else {
        for (EventLogEntryList::iterator it = begin(); it != end(); it++) {
            GateDescriptionEntry *gateDescriptionEntry = dynamic_cast<GateDescriptionEntry *>(*it);
            if (gateDescriptionEntry && gateDescriptionEntry->moduleId == moduleId && gateDescriptionEntry->gateId == gateId)
                return gateDescriptionEntry;
        }
    }
    return nullptr;
}

GateCreatedEntry *EventLogEntryCache::getGateCreatedEntry(int moduleId, int gateId)
{
    ensureCacheBuilt();
    if (moduleIdAndGateIdToGateCreatedEntryMap) {
        ModuleIdAndGateIdToGateCreatedEntryMap::iterator it = moduleIdAndGateIdToGateCreatedEntryMap->find(std::pair<int, int>(moduleId, gateId));
        if (it != moduleIdAndGateIdToGateCreatedEntryMap->end())
            return it->second;
    }
    else {
        for (EventLogEntryList::iterator it = begin(); it != end(); it++) {
            GateCreatedEntry *gateCreatedEntry = dynamic_cast<GateCreatedEntry *>(*it);
            if (gateCreatedEntry && gateCreatedEntry->moduleId == moduleId && gateCreatedEntry->gateId == gateId)
                return gateCreatedEntry;
        }
    }
    return nullptr;
}

GateDeletedEntry *EventLogEntryCache::getGateDeletedEntry(int moduleId, int gateId)
{
    ensureCacheBuilt();
    if (moduleIdAndGateIdToGateDeletedEntryMap) {
        ModuleIdAndGateIdToGateDeletedEntryMap::iterator it = moduleIdAndGateIdToGateDeletedEntryMap->find(std::pair<int, int>(moduleId, gateId));
        if (it != moduleIdAndGateIdToGateDeletedEntryMap->end())
            return it->second;
    }
    else {
        for (EventLogEntryList::iterator it = begin(); it != end(); it++) {
            GateDeletedEntry *gateDeletedEntry = dynamic_cast<GateDeletedEntry *>(*it);
            if (gateDeletedEntry && gateDeletedEntry->moduleId == moduleId && gateDeletedEntry->gateId == gateId)
                return gateDeletedEntry;
        }
    }
    return nullptr;
}

GateDisplayStringChangedEntry *EventLogEntryCache::getGateDisplayStringChangedEntry(int moduleId, int gateId, int entryIndex)
{
    ensureCacheBuilt();
    if (moduleIdAndGateIdToGateDisplayStringChangedVectorMap) {
        ModuleIdAndGateIdToGateDisplayStringChangedVectorMap::iterator it = moduleIdAndGateIdToGateDisplayStringChangedVectorMap->find(std::pair<int, int>(moduleId, gateId));
        if (it != moduleIdAndGateIdToGateDisplayStringChangedVectorMap->end()) {
            std::vector<GateDisplayStringChangedEntry *>& gateDisplayStringChangedVector = it->second;
            for (std::vector<GateDisplayStringChangedEntry *>::iterator jt = gateDisplayStringChangedVector.begin(); jt != gateDisplayStringChangedVector.end(); jt++) {
                GateDisplayStringChangedEntry *gateDisplayStringChangedEntry = *jt;
                if (gateDisplayStringChangedEntry->getEntryIndex() >= entryIndex)
                    return gateDisplayStringChangedEntry;
            }
        }
    }
    else {
        for (EventLogEntryList::iterator it = begin() + entryIndex; it != end(); it++) {
            GateDisplayStringChangedEntry *gateDisplayStringChangedEntry = dynamic_cast<GateDisplayStringChangedEntry *>(*it);
            if (gateDisplayStringChangedEntry && gateDisplayStringChangedEntry->moduleId == moduleId && gateDisplayStringChangedEntry->gateId == gateId)
                return gateDisplayStringChangedEntry;
        }
    }
    return nullptr;
}

CustomDescriptionEntry *EventLogEntryCache::getCustomDescriptionEntry(std::string type, long key)
{
    ensureCacheBuilt();
    if (typeAndKeyToCustomDescriptionEntryMap) {
        TypeAndKeyToCustomDescriptionEntryMap::iterator it = typeAndKeyToCustomDescriptionEntryMap->find(std::pair<std::string, long>(type, key));
        if (it != typeAndKeyToCustomDescriptionEntryMap->end())
            return it->second;
    }
    else {
        for (EventLogEntryList::iterator it = begin(); it != end(); it++) {
            CustomDescriptionEntry *customDescriptionEntry = dynamic_cast<CustomDescriptionEntry *>(*it);
            if (customDescriptionEntry && type == customDescriptionEntry->type && customDescriptionEntry->key == key)
                return customDescriptionEntry;
        }
    }
    return nullptr;
}

CustomCreatedEntry *EventLogEntryCache::getCustomCreatedEntry(std::string type, long key)
{
    ensureCacheBuilt();
    if (typeAndKeyToCustomCreatedEntryMap) {
        TypeAndKeyToCustomCreatedEntryMap::iterator it = typeAndKeyToCustomCreatedEntryMap->find(std::pair<std::string, long>(type, key));
        if (it != typeAndKeyToCustomCreatedEntryMap->end())
            return it->second;
    }
    else {
        for (EventLogEntryList::iterator it = begin(); it != end(); it++) {
            CustomCreatedEntry *customCreatedEntry = dynamic_cast<CustomCreatedEntry *>(*it);
            if (customCreatedEntry && type == customCreatedEntry->type && customCreatedEntry->key == key)
                return customCreatedEntry;
        }
    }
    return nullptr;
}

CustomDeletedEntry *EventLogEntryCache::getCustomDeletedEntry(std::string type, long key)
{
    ensureCacheBuilt();
    if (typeAndKeyToCustomDeletedEntryMap) {
        TypeAndKeyToCustomDeletedEntryMap::iterator it = typeAndKeyToCustomDeletedEntryMap->find(std::pair<std::string, long>(type, key));
        if (it != typeAndKeyToCustomDeletedEntryMap->end())
            return it->second;
    }
    else {
        for (EventLogEntryList::iterator it = begin(); it != end(); it++) {
            CustomDeletedEntry *customDeletedEntry = dynamic_cast<CustomDeletedEntry *>(*it);
            if (customDeletedEntry && type == customDeletedEntry->type && customDeletedEntry->key == key)
                return customDeletedEntry;
        }
    }
    return nullptr;
}

CustomChangedEntry *EventLogEntryCache::getCustomChangedEntry(std::string type, long key, int entryIndex)
{
    ensureCacheBuilt();
    if (typeAndKeyToCustomChangedVectorMap) {
        TypeAndKeyToCustomChangedVectorMap::iterator it = typeAndKeyToCustomChangedVectorMap->find(std::pair<std::string, long>(type, key));
        if (it != typeAndKeyToCustomChangedVectorMap->end()) {
            std::vector<CustomChangedEntry *>& customDisplayStringChangedVector = it->second;
            for (std::vector<CustomChangedEntry *>::iterator jt = customDisplayStringChangedVector.begin(); jt != customDisplayStringChangedVector.end(); jt++) {
                CustomChangedEntry *customChangedEntry = *jt;
                if (customChangedEntry->getEntryIndex() >= entryIndex)
                    return customChangedEntry;
            }
        }
    }
    else {
        for (EventLogEntryList::iterator it = begin() + entryIndex; it != end(); it++) {
            CustomChangedEntry *customChangedEntry = dynamic_cast<CustomChangedEntry *>(*it);
            if (customChangedEntry && type == customChangedEntry->type && customChangedEntry->key == key)
                return customChangedEntry;
        }
    }
    return nullptr;
}

}  // namespace eventlog
}  // namespace omnetpp
