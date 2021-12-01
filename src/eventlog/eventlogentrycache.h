//=========================================================================
//  EVENTLOGENTRYCACHE.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __EVENTLOGENTRYCACHE_H_
#define __EVENTLOGENTRYCACHE_H_

#include <vector>
#include <map>
#include "eventlogentries.h"

namespace omnetpp {
namespace eventlog {

/**
 * Efficient storage for a large number of eventlog entries.
 */
// TODO: there's a lot of similarity in this class, factor this out into new classes or templates, or whatnot
class EVENTLOG_API EventLogEntryCache : public EventLogEntryList
{
    protected:
        // modules
        typedef std::map<int, ModuleDescriptionEntry *> ModuleIdToModuleDescriptionEntryMap;
        ModuleIdToModuleDescriptionEntryMap *moduleIdToModuleDescriptionEntryMap; // module description entries

        typedef std::map<int, ModuleCreatedEntry *> ModuleIdToModuleCreatedEntryMap;
        ModuleIdToModuleCreatedEntryMap *moduleIdToModuleCreatedEntryMap; // module creation begin entries

        typedef std::map<int, ModuleDeletedEntry *> ModuleIdToModuleDeletedEntryMap;
        ModuleIdToModuleDeletedEntryMap *moduleIdToModuleDeletedEntryMap; // module deleted entries

        typedef std::map<int, std::vector<ModuleDisplayStringChangedEntry *> > ModuleIdToModuleDisplayStringChangedVectorMap;
        ModuleIdToModuleDisplayStringChangedVectorMap *moduleIdToModuleDisplayStringChangedVectorMap; // module display string changed entries

        // connections
        typedef std::map<std::pair<int, int>, ConnectionDescriptionEntry *> ModuleIdAndGateIdToConnectionDescriptionEntryMap;
        ModuleIdAndGateIdToConnectionDescriptionEntryMap *moduleIdAndGateIdToConnectionDescriptionEntryMap; // connection description entries

        typedef std::map<std::pair<int, int>, ConnectionCreatedEntry *> ModuleIdAndGateIdToConnectionCreatedEntryMap;
        ModuleIdAndGateIdToConnectionCreatedEntryMap *moduleIdAndGateIdToConnectionCreatedEntryMap; // connection created entries

        typedef std::map<std::pair<int, int>, ConnectionDeletedEntry *> ModuleIdAndGateIdToConnectionDeletedEntryMap;
        ModuleIdAndGateIdToConnectionDeletedEntryMap *moduleIdAndGateIdToConnectionDeletedEntryMap; // connection deleted entries

        typedef std::map<std::pair<int, int>, std::vector<ConnectionDisplayStringChangedEntry *> > ModuleIdAndGateIdToConnectionDisplayStringChangedVectorMap;
        ModuleIdAndGateIdToConnectionDisplayStringChangedVectorMap *moduleIdAndGateIdToConnectionDisplayStringChangedVectorMap; // connection display string changed entries

        // gates
        typedef std::map<std::pair<int, int>, GateDescriptionEntry *> ModuleIdAndGateIdToGateDescriptionEntryMap;
        ModuleIdAndGateIdToGateDescriptionEntryMap *moduleIdAndGateIdToGateDescriptionEntryMap; // gate description entries

        typedef std::map<std::pair<int, int>, GateCreatedEntry *> ModuleIdAndGateIdToGateCreatedEntryMap;
        ModuleIdAndGateIdToGateCreatedEntryMap *moduleIdAndGateIdToGateCreatedEntryMap; // gate created entries

        typedef std::map<std::pair<int, int>, GateDeletedEntry *> ModuleIdAndGateIdToGateDeletedEntryMap;
        ModuleIdAndGateIdToGateDeletedEntryMap *moduleIdAndGateIdToGateDeletedEntryMap; // gate deleted entries

        typedef std::map<std::pair<int, int>, std::vector<GateDisplayStringChangedEntry *> > ModuleIdAndGateIdToGateDisplayStringChangedVectorMap;
        ModuleIdAndGateIdToGateDisplayStringChangedVectorMap *moduleIdAndGateIdToGateDisplayStringChangedVectorMap; // gate display string changed entries

        // custom
        // TODO: rename
        typedef std::map<std::pair<std::string, long>, CustomDescriptionEntry *> TypeAndKeyToCustomDescriptionEntryMap;
        TypeAndKeyToCustomDescriptionEntryMap *typeAndKeyToCustomDescriptionEntryMap; // custom description entries

        typedef std::map<std::pair<std::string, long>, CustomCreatedEntry *> TypeAndKeyToCustomCreatedEntryMap;
        TypeAndKeyToCustomCreatedEntryMap *typeAndKeyToCustomCreatedEntryMap; // custom creation begin entries

        typedef std::map<std::pair<std::string, long>, CustomDeletedEntry *> TypeAndKeyToCustomDeletedEntryMap;
        TypeAndKeyToCustomDeletedEntryMap *typeAndKeyToCustomDeletedEntryMap; // custom deleted entries

        typedef std::map<std::pair<std::string, long>, std::vector<CustomChangedEntry *> > TypeAndKeyToCustomChangedVectorMap;
        TypeAndKeyToCustomChangedVectorMap *typeAndKeyToCustomChangedVectorMap; // custom changed entries

    public:
        EventLogEntryCache();
        ~EventLogEntryCache();

        void clearCache();
        void cacheEventLogEntry(EventLogEntry *eventLogEntry);

        ModuleDescriptionEntry *getModuleDescriptionEntry(int moduleId);
        std::vector<ModuleDescriptionEntry *> getModuleDescriptionEntries();
        int getNumModuleDescriptionEntries() { ensureCacheBuilt(); return moduleIdToModuleDescriptionEntryMap->size(); }
        ModuleCreatedEntry *getModuleCreatedEntry(int moduleId);
        ModuleDeletedEntry *getModuleDeletedEntry(int moduleId);
        ModuleDisplayStringChangedEntry *getModuleDisplayStringChangedEntry(int moduleId, int entryIndex);

        ConnectionDescriptionEntry *getConnectionDescriptionEntry(int sourceModuleId, int sourceGateId);
        ConnectionCreatedEntry *getConnectionCreatedEntry(int sourceModuleId, int sourceGateId);
        ConnectionDeletedEntry *getConnectionDeletedEntry(int sourceModuleId, int sourceGateId);
        ConnectionDisplayStringChangedEntry *getConnectionDisplayStringChangedEntry(int sourceModuleId, int sourceGateId, int entryIndex);

        GateDescriptionEntry *getGateDescriptionEntry(int moduleId, int gateId);
        GateCreatedEntry *getGateCreatedEntry(int moduleId, int gateId);
        GateDeletedEntry *getGateDeletedEntry(int moduleId, int gateId);
        GateDisplayStringChangedEntry *getGateDisplayStringChangedEntry(int moduleId, int gateId, int entryIndex);

        CustomDescriptionEntry *getCustomDescriptionEntry(std::string type, long key);
        CustomCreatedEntry *getCustomCreatedEntry(std::string type, long key);
        CustomDeletedEntry *getCustomDeletedEntry(std::string type, long key);
        CustomChangedEntry *getCustomChangedEntry(std::string type, long key, int entryIndex);

    protected:
        void allocateCache();
        void buildCache();
        void ensureCacheAllocated();
        void ensureCacheBuilt();
};

}  // namespace eventlog
}  // namespace omnetpp

#endif
