package org.omnetpp.eventlog;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.TreeMap;

import org.omnetpp.common.util.Pair;
import org.omnetpp.eventlog.entry.ConnectionCreatedEntry;
import org.omnetpp.eventlog.entry.ConnectionDeletedEntry;
import org.omnetpp.eventlog.entry.ConnectionDescriptionEntry;
import org.omnetpp.eventlog.entry.ConnectionDisplayStringChangedEntry;
import org.omnetpp.eventlog.entry.CustomChangedEntry;
import org.omnetpp.eventlog.entry.CustomCreatedEntry;
import org.omnetpp.eventlog.entry.CustomDeletedEntry;
import org.omnetpp.eventlog.entry.CustomDescriptionEntry;
import org.omnetpp.eventlog.entry.GateCreatedEntry;
import org.omnetpp.eventlog.entry.GateDeletedEntry;
import org.omnetpp.eventlog.entry.GateDescriptionEntry;
import org.omnetpp.eventlog.entry.GateDisplayStringChangedEntry;
import org.omnetpp.eventlog.entry.ModuleCreatedEntry;
import org.omnetpp.eventlog.entry.ModuleDeletedEntry;
import org.omnetpp.eventlog.entry.ModuleDescriptionEntry;
import org.omnetpp.eventlog.entry.ModuleDisplayStringChangedEntry;

/**
 * Efficient storage for a large number of eventlog entries.
 */
// TODO: there's a lot of similarity in this class, factor this out into new classes or templates, or whatnot
public class EventLogEntryCache
{
    protected ArrayList<EventLogEntry> eventLogEntries = new ArrayList<EventLogEntry>();

    // modules
    protected TreeMap<Integer, ModuleDescriptionEntry> moduleIdToModuleDescriptionEntryMap; // module description entries

    protected TreeMap<Integer, ModuleCreatedEntry> moduleIdToModuleCreatedEntryMap; // module creation begin entries

    protected TreeMap<Integer, ModuleDeletedEntry> moduleIdToModuleDeletedEntryMap; // module deleted entries

    protected TreeMap<Integer, ArrayList<ModuleDisplayStringChangedEntry>> moduleIdToModuleDisplayStringChangedVectorMap; // module display string changed entries

    // connections
    protected HashMap<Pair<Integer, Integer>, ConnectionDescriptionEntry> moduleIdAndGateIdToConnectionDescriptionEntryMap; // connection description entries

    protected HashMap<Pair<Integer, Integer>, ConnectionCreatedEntry> moduleIdAndGateIdToConnectionCreatedEntryMap; // connection created entries

    protected HashMap<Pair<Integer, Integer>, ConnectionDeletedEntry> moduleIdAndGateIdToConnectionDeletedEntryMap; // connection deleted entries

    protected HashMap<Pair<Integer, Integer>, ArrayList<ConnectionDisplayStringChangedEntry>> moduleIdAndGateIdToConnectionDisplayStringChangedVectorMap; // connection display string changed entries

    // gates
    protected HashMap<Pair<Integer, Integer>, GateDescriptionEntry> moduleIdAndGateIdToGateDescriptionEntryMap; // gate description entries

    protected HashMap<Pair<Integer, Integer>, GateCreatedEntry> moduleIdAndGateIdToGateCreatedEntryMap; // gate created entries

    protected HashMap<Pair<Integer, Integer>, GateDeletedEntry> moduleIdAndGateIdToGateDeletedEntryMap; // gate deleted entries

    protected HashMap<Pair<Integer, Integer>, ArrayList<GateDisplayStringChangedEntry>> moduleIdAndGateIdToGateDisplayStringChangedVectorMap; // gate display string changed entries

    // custom
    protected HashMap<Pair<String, Long>, CustomDescriptionEntry> typeAndKeyToCustomDescriptionEntryMap; // custom description entries

    protected HashMap<Pair<String, Long>, CustomCreatedEntry> typeAndKeyToCustomCreatedEntryMap; // custom creation begin entries

    protected HashMap<Pair<String, Long>, CustomDeletedEntry> typeAndKeyToCustomDeletedEntryMap; // custom deleted entries

    protected HashMap<Pair<String, Long>, ArrayList<CustomChangedEntry>> typeAndKeyToCustomChangedVectorMap; // custom changed entries

    public ArrayList<EventLogEntry> getEventLogEntries() {
        return eventLogEntries;
    }

    public final void clearCache() {
        // module
        if (moduleIdToModuleDescriptionEntryMap != null)
            moduleIdToModuleDescriptionEntryMap.clear();
        if (moduleIdToModuleCreatedEntryMap != null)
            moduleIdToModuleCreatedEntryMap.clear();
        if (moduleIdToModuleDeletedEntryMap != null)
            moduleIdToModuleDeletedEntryMap.clear();
        if (moduleIdToModuleDisplayStringChangedVectorMap != null)
            moduleIdToModuleDisplayStringChangedVectorMap.clear();
        // connection
        if (moduleIdAndGateIdToConnectionDescriptionEntryMap != null)
            moduleIdAndGateIdToConnectionDescriptionEntryMap.clear();
        if (moduleIdAndGateIdToConnectionCreatedEntryMap != null)
            moduleIdAndGateIdToConnectionCreatedEntryMap.clear();
        if (moduleIdAndGateIdToConnectionDeletedEntryMap != null)
            moduleIdAndGateIdToConnectionDeletedEntryMap.clear();
        if (moduleIdAndGateIdToConnectionDisplayStringChangedVectorMap != null)
            moduleIdAndGateIdToConnectionDisplayStringChangedVectorMap.clear();
        // gate
        if (moduleIdAndGateIdToGateDescriptionEntryMap != null)
            moduleIdAndGateIdToGateDescriptionEntryMap.clear();
        if (moduleIdAndGateIdToGateCreatedEntryMap != null)
            moduleIdAndGateIdToGateCreatedEntryMap.clear();
        if (moduleIdAndGateIdToGateDeletedEntryMap != null)
            moduleIdAndGateIdToGateDeletedEntryMap.clear();
        if (moduleIdAndGateIdToGateDisplayStringChangedVectorMap != null)
            moduleIdAndGateIdToGateDisplayStringChangedVectorMap.clear();
        // custom
        if (typeAndKeyToCustomDescriptionEntryMap != null)
            typeAndKeyToCustomDescriptionEntryMap.clear();
        if (typeAndKeyToCustomCreatedEntryMap != null)
            typeAndKeyToCustomCreatedEntryMap.clear();
        if (typeAndKeyToCustomDeletedEntryMap != null)
            typeAndKeyToCustomDeletedEntryMap.clear();
        if (typeAndKeyToCustomChangedVectorMap != null)
            typeAndKeyToCustomChangedVectorMap.clear();
    }

    public final void cacheEventLogEntry(EventLogEntry eventLogEntry) {
        // TODO: this will always allocate all structures and that's not what we need
        eventLogEntries.add(eventLogEntry);
        ensureCacheAllocated();
        // description entries
        if (eventLogEntry instanceof ModuleDescriptionEntry) {
            ModuleDescriptionEntry moduleDescriptionEntry = eventLogEntry instanceof ModuleDescriptionEntry ? (ModuleDescriptionEntry)eventLogEntry : null;
            moduleIdToModuleDescriptionEntryMap.put(moduleDescriptionEntry.moduleId, moduleDescriptionEntry);
        }
        else if (eventLogEntry instanceof ConnectionDescriptionEntry) {
            ConnectionDescriptionEntry connectionDescriptionEntry = eventLogEntry instanceof ConnectionDescriptionEntry ? (ConnectionDescriptionEntry)eventLogEntry : null;
            moduleIdAndGateIdToConnectionDescriptionEntryMap.put(new Pair<Integer, Integer>(connectionDescriptionEntry.sourceModuleId, connectionDescriptionEntry.sourceGateId), connectionDescriptionEntry);
        }
        else if (eventLogEntry instanceof GateDescriptionEntry) {
            GateDescriptionEntry gateDescriptionEntry = eventLogEntry instanceof GateDescriptionEntry ? (GateDescriptionEntry)eventLogEntry : null;
            moduleIdAndGateIdToGateDescriptionEntryMap.put(new Pair<Integer, Integer>(gateDescriptionEntry.moduleId, gateDescriptionEntry.gateId), gateDescriptionEntry);
        }
        else if (eventLogEntry instanceof CustomDescriptionEntry) {
            CustomDescriptionEntry customDescriptionEntry = eventLogEntry instanceof CustomDescriptionEntry ? (CustomDescriptionEntry)eventLogEntry : null;
            typeAndKeyToCustomDescriptionEntryMap.put(new Pair<String, Long>(customDescriptionEntry.type, customDescriptionEntry.key), customDescriptionEntry);
        }
        // module
        if (eventLogEntry instanceof ModuleCreatedEntry) {
            ModuleCreatedEntry moduleCreatedEntry = eventLogEntry instanceof ModuleCreatedEntry ? (ModuleCreatedEntry)eventLogEntry : null;
            moduleIdToModuleCreatedEntryMap.put(moduleCreatedEntry.moduleId, moduleCreatedEntry);
        }
        else if (eventLogEntry instanceof ModuleDeletedEntry) {
            ModuleDeletedEntry moduleDeletedEntry = eventLogEntry instanceof ModuleDeletedEntry ? (ModuleDeletedEntry)eventLogEntry : null;
            moduleIdToModuleDeletedEntryMap.put(moduleDeletedEntry.moduleId, moduleDeletedEntry);
        }
        else if (eventLogEntry instanceof ModuleDisplayStringChangedEntry) {
            ModuleDisplayStringChangedEntry moduleDisplayStringChangedEntry = eventLogEntry instanceof ModuleDisplayStringChangedEntry ? (ModuleDisplayStringChangedEntry)eventLogEntry : null;
            ArrayList<ModuleDisplayStringChangedEntry> jt = moduleIdToModuleDisplayStringChangedVectorMap.get(moduleDisplayStringChangedEntry.moduleId);
            if (jt != null)
                jt.add(moduleDisplayStringChangedEntry);
            else
                moduleIdToModuleDisplayStringChangedVectorMap.put(moduleDisplayStringChangedEntry.moduleId, new ArrayList<ModuleDisplayStringChangedEntry>(List.of(moduleDisplayStringChangedEntry)));
        }
        // connection
        else if (eventLogEntry instanceof ConnectionCreatedEntry) {
            ConnectionCreatedEntry gateCreatedEntry = eventLogEntry instanceof ConnectionCreatedEntry ? (ConnectionCreatedEntry)eventLogEntry : null;
            moduleIdAndGateIdToConnectionCreatedEntryMap.put(new Pair<Integer, Integer>(gateCreatedEntry.sourceModuleId, gateCreatedEntry.sourceGateId), gateCreatedEntry);
        }
        else if (eventLogEntry instanceof ConnectionDeletedEntry) {
            ConnectionDeletedEntry gateDeletedEntry = eventLogEntry instanceof ConnectionDeletedEntry ? (ConnectionDeletedEntry)eventLogEntry : null;
            moduleIdAndGateIdToConnectionDeletedEntryMap.put(new Pair<Integer, Integer>(gateDeletedEntry.sourceModuleId, gateDeletedEntry.sourceGateId), gateDeletedEntry);
        }
        else if (eventLogEntry instanceof ConnectionDisplayStringChangedEntry) {
            ConnectionDisplayStringChangedEntry gateDisplayStringChangedEntry = eventLogEntry instanceof ConnectionDisplayStringChangedEntry ? (ConnectionDisplayStringChangedEntry)eventLogEntry : null;
            ArrayList<ConnectionDisplayStringChangedEntry> jt = moduleIdAndGateIdToConnectionDisplayStringChangedVectorMap.get(new Pair<Integer, Integer>(gateDisplayStringChangedEntry.sourceModuleId, gateDisplayStringChangedEntry.sourceGateId));
            if (jt != null)
                jt.add(gateDisplayStringChangedEntry);
            else
                moduleIdAndGateIdToConnectionDisplayStringChangedVectorMap.put(new Pair<Integer, Integer>(gateDisplayStringChangedEntry.sourceModuleId, gateDisplayStringChangedEntry.sourceGateId), new ArrayList<ConnectionDisplayStringChangedEntry>(List.of(gateDisplayStringChangedEntry)));
        }
        // gate
        else if (eventLogEntry instanceof GateCreatedEntry) {
            GateCreatedEntry gateCreatedEntry = eventLogEntry instanceof GateCreatedEntry ? (GateCreatedEntry)eventLogEntry : null;
            moduleIdAndGateIdToGateCreatedEntryMap.put(new Pair<Integer, Integer>(gateCreatedEntry.moduleId, gateCreatedEntry.gateId), gateCreatedEntry);
        }
        else if (eventLogEntry instanceof GateDeletedEntry) {
            GateDeletedEntry gateDeletedEntry = eventLogEntry instanceof GateDeletedEntry ? (GateDeletedEntry)eventLogEntry : null;
            moduleIdAndGateIdToGateDeletedEntryMap.put(new Pair<Integer, Integer>(gateDeletedEntry.moduleId, gateDeletedEntry.gateId), gateDeletedEntry);
        }
        else if (eventLogEntry instanceof GateDisplayStringChangedEntry) {
            GateDisplayStringChangedEntry gateDisplayStringChangedEntry = eventLogEntry instanceof GateDisplayStringChangedEntry ? (GateDisplayStringChangedEntry)eventLogEntry : null;
            ArrayList<GateDisplayStringChangedEntry> jt = moduleIdAndGateIdToGateDisplayStringChangedVectorMap.get(new Pair<Integer, Integer>(gateDisplayStringChangedEntry.moduleId, gateDisplayStringChangedEntry.gateId));
            if (jt != null)
                jt.add(gateDisplayStringChangedEntry);
            else
                moduleIdAndGateIdToGateDisplayStringChangedVectorMap.put(new Pair<Integer, Integer>(gateDisplayStringChangedEntry.moduleId, gateDisplayStringChangedEntry.gateId), new ArrayList<GateDisplayStringChangedEntry>(List.of(gateDisplayStringChangedEntry)));
        }
        // custom
        else if (eventLogEntry instanceof CustomCreatedEntry) {
            CustomCreatedEntry customCreatedEntry = eventLogEntry instanceof CustomCreatedEntry ? (CustomCreatedEntry)eventLogEntry : null;
            typeAndKeyToCustomCreatedEntryMap.put(new Pair<String, Long>(customCreatedEntry.type, customCreatedEntry.key), customCreatedEntry);
        }
        else if (eventLogEntry instanceof CustomDeletedEntry) {
            CustomDeletedEntry customDeletedEntry = eventLogEntry instanceof CustomDeletedEntry ? (CustomDeletedEntry)eventLogEntry : null;
            typeAndKeyToCustomDeletedEntryMap.put(new Pair<String, Long>(customDeletedEntry.type, customDeletedEntry.key), customDeletedEntry);
        }
        else if (eventLogEntry instanceof CustomChangedEntry) {
            CustomChangedEntry customChangedEntry = eventLogEntry instanceof CustomChangedEntry ? (CustomChangedEntry)eventLogEntry : null;
            ArrayList<CustomChangedEntry> jt = typeAndKeyToCustomChangedVectorMap.get(new Pair<String, Long>(customChangedEntry.type, customChangedEntry.key));
            if (jt != null)
                jt.add(customChangedEntry);
            else
                typeAndKeyToCustomChangedVectorMap.put(new Pair<String, Long>(customChangedEntry.type, customChangedEntry.key), new ArrayList<CustomChangedEntry>(List.of(customChangedEntry)));
        }
    }

    public final ModuleDescriptionEntry getModuleDescriptionEntry(int moduleId) {
        ensureCacheBuilt();
        if (moduleIdToModuleDescriptionEntryMap != null) {
            ModuleDescriptionEntry it = moduleIdToModuleDescriptionEntryMap.get(moduleId);
            if (it != null)
                return it;
        }
        else {
            for (EventLogEntry eventLogEntry : eventLogEntries) {
                if (eventLogEntry instanceof ModuleDescriptionEntry) {
                    ModuleDescriptionEntry moduleDescriptionEntry = (ModuleDescriptionEntry)eventLogEntry;
                    if (moduleDescriptionEntry != null && moduleDescriptionEntry.moduleId == moduleId)
                        return moduleDescriptionEntry;
                }
            }
        }
        return null;
    }

    public final ArrayList<ModuleDescriptionEntry> getModuleDescriptionEntries() {
        ensureCacheBuilt();
        // TODO: linear search when the map is not filled
        return new ArrayList<ModuleDescriptionEntry>(moduleIdToModuleDescriptionEntryMap.values());
    }

    public final int getNumModuleDescriptionEntries() {
        ensureCacheBuilt();
        return moduleIdToModuleDescriptionEntryMap.size();
    }

    public final ModuleCreatedEntry getModuleCreatedEntry(int moduleId) {
        ensureCacheBuilt();
        if (moduleIdToModuleCreatedEntryMap != null) {
            var it = moduleIdToModuleCreatedEntryMap.get(moduleId);
            if (it != null)
                return it;
        }
        else {
            for (EventLogEntry eventLogEntry : eventLogEntries) {
                if (eventLogEntry instanceof ModuleCreatedEntry) {
                    ModuleCreatedEntry moduleCreatedEntry = (ModuleCreatedEntry)eventLogEntry;
                    if (moduleCreatedEntry != null && moduleCreatedEntry.moduleId == moduleId)
                        return moduleCreatedEntry;
                }
            }
        }
        return null;
    }

    public final ModuleDeletedEntry getModuleDeletedEntry(int moduleId) {
        ensureCacheBuilt();
        if (moduleIdToModuleDeletedEntryMap != null) {
            var it = moduleIdToModuleDeletedEntryMap.get(moduleId);
            if (it != null)
                return it;
        }
        else {
            for (EventLogEntry eventLogEntry : eventLogEntries) {
                if (eventLogEntry instanceof ModuleDeletedEntry) {
                    ModuleDeletedEntry moduleDeletedEntry = (ModuleDeletedEntry)eventLogEntry;
                    if (moduleDeletedEntry != null && moduleDeletedEntry.moduleId == moduleId)
                        return moduleDeletedEntry;
                }
            }
        }
        return null;
    }

    public final ModuleDisplayStringChangedEntry getModuleDisplayStringChangedEntry(int moduleId, int entryIndex) {
        ensureCacheBuilt();
        if (moduleIdToModuleDisplayStringChangedVectorMap != null) {
            var it = moduleIdToModuleDisplayStringChangedVectorMap.get(moduleId);
            if (it != null) {
                ArrayList<ModuleDisplayStringChangedEntry> moduleDisplayStringChangedVector = it;
                for (ModuleDisplayStringChangedEntry jt : moduleDisplayStringChangedVector) {
                    ModuleDisplayStringChangedEntry moduleDisplayStringChangedEntry = jt;
                    if (moduleDisplayStringChangedEntry.getEntryIndex() >= entryIndex)
                        return moduleDisplayStringChangedEntry;
                }
            }
        }
        else {
            for (int index = entryIndex; index < eventLogEntries.size(); index++) {
                EventLogEntry eventLogEntry = eventLogEntries.get(index);
                if (eventLogEntry instanceof ModuleDisplayStringChangedEntry) {
                    ModuleDisplayStringChangedEntry moduleDisplayStringChangedEntry = (ModuleDisplayStringChangedEntry)eventLogEntry;
                    if (moduleDisplayStringChangedEntry != null && moduleDisplayStringChangedEntry.moduleId == moduleId)
                        return moduleDisplayStringChangedEntry;
                }
            }
        }
        return null;
    }

    public final ConnectionDescriptionEntry getConnectionDescriptionEntry(int sourceModuleId, int sourceGateId) {
        ensureCacheBuilt();
        if (moduleIdAndGateIdToConnectionDescriptionEntryMap != null) {
            var it = moduleIdAndGateIdToConnectionDescriptionEntryMap.get(new Pair<Integer, Integer>(sourceModuleId, sourceGateId));
            if (it != null)
                return it;
        }
        else {
            for (EventLogEntry eventLogEntry : eventLogEntries) {
                if (eventLogEntry instanceof ConnectionDescriptionEntry) {
                    ConnectionDescriptionEntry connectionDescriptionEntry = (ConnectionDescriptionEntry)eventLogEntry;
                    if (connectionDescriptionEntry != null && connectionDescriptionEntry.sourceModuleId == sourceModuleId && connectionDescriptionEntry.sourceGateId == sourceGateId)
                        return connectionDescriptionEntry;
                }
            }
        }
        return null;
    }

    public final ConnectionCreatedEntry getConnectionCreatedEntry(int sourceModuleId, int sourceGateId) {
        ensureCacheBuilt();
        if (moduleIdAndGateIdToConnectionCreatedEntryMap != null) {
            var it = moduleIdAndGateIdToConnectionCreatedEntryMap.get(new Pair<Integer, Integer>(sourceModuleId, sourceGateId));
            if (it != null)
                return it;
        }
        else {
            for (EventLogEntry eventLogEntry : eventLogEntries) {
                if (eventLogEntry instanceof ConnectionCreatedEntry) {
                    ConnectionCreatedEntry connectionCreatedEntry = (ConnectionCreatedEntry)eventLogEntry;
                    if (connectionCreatedEntry != null && connectionCreatedEntry.sourceModuleId == sourceModuleId && connectionCreatedEntry.sourceGateId == sourceGateId)
                        return connectionCreatedEntry;
                }
            }
        }
        return null;
    }

    public final ConnectionDeletedEntry getConnectionDeletedEntry(int sourceModuleId, int sourceGateId) {
        ensureCacheBuilt();
        if (moduleIdAndGateIdToConnectionDeletedEntryMap != null) {
            var it = moduleIdAndGateIdToConnectionDeletedEntryMap.get(new Pair<Integer, Integer>(sourceModuleId, sourceGateId));
            if (it != null)
                return it;
        }
        else {
            for (EventLogEntry eventLogEntry : eventLogEntries) {
                if (eventLogEntry instanceof ConnectionDeletedEntry) {
                    ConnectionDeletedEntry connectionDeletedEntry = (ConnectionDeletedEntry)eventLogEntry;
                    if (connectionDeletedEntry != null && connectionDeletedEntry.sourceModuleId == sourceModuleId && connectionDeletedEntry.sourceGateId == sourceGateId)
                        return connectionDeletedEntry;
                }
            }
        }
        return null;
    }

    public final ConnectionDisplayStringChangedEntry getConnectionDisplayStringChangedEntry(int sourceModuleId, int sourceGateId, int entryIndex) {
        ensureCacheBuilt();
        if (moduleIdAndGateIdToConnectionDisplayStringChangedVectorMap != null) {
            var it = moduleIdAndGateIdToConnectionDisplayStringChangedVectorMap.get(new Pair<Integer, Integer>(sourceModuleId, sourceGateId));
            if (it != null) {
                ArrayList<ConnectionDisplayStringChangedEntry> gateDisplayStringChangedVector = it;
                for (ConnectionDisplayStringChangedEntry jt : gateDisplayStringChangedVector) {
                    ConnectionDisplayStringChangedEntry connectionDisplayStringChangedEntry = jt;
                    if (connectionDisplayStringChangedEntry.getEntryIndex() >= entryIndex)
                        return connectionDisplayStringChangedEntry;
                }
            }
        }
        else {
            for (int index = entryIndex; index < eventLogEntries.size(); index++) {
                EventLogEntry eventLogEntry = eventLogEntries.get(index);
                if (eventLogEntry instanceof ConnectionDisplayStringChangedEntry) {
                    ConnectionDisplayStringChangedEntry connectionDisplayStringChangedEntry = (ConnectionDisplayStringChangedEntry)eventLogEntry;
                    if (connectionDisplayStringChangedEntry != null && connectionDisplayStringChangedEntry.sourceModuleId == sourceModuleId && connectionDisplayStringChangedEntry.sourceGateId == sourceGateId)
                        return connectionDisplayStringChangedEntry;
                }
            }
        }
        return null;
    }

    public final GateDescriptionEntry getGateDescriptionEntry(int moduleId, int gateId) {
        ensureCacheBuilt();
        if (moduleIdAndGateIdToGateDescriptionEntryMap != null) {
            var it = moduleIdAndGateIdToGateDescriptionEntryMap.get(new Pair<Integer, Integer>(moduleId, gateId));
            if (it != null)
                return it;
        }
        else {
            for (EventLogEntry eventLogEntry : eventLogEntries) {
                if (eventLogEntry instanceof GateDescriptionEntry) {
                    GateDescriptionEntry gateDescriptionEntry = (GateDescriptionEntry)eventLogEntry;
                    if (gateDescriptionEntry != null && gateDescriptionEntry.moduleId == moduleId && gateDescriptionEntry.gateId == gateId)
                        return gateDescriptionEntry;
                }
            }
        }
        return null;
    }

    public final GateCreatedEntry getGateCreatedEntry(int moduleId, int gateId) {
        ensureCacheBuilt();
        if (moduleIdAndGateIdToGateCreatedEntryMap != null) {
            var it = moduleIdAndGateIdToGateCreatedEntryMap.get(new Pair<Integer, Integer>(moduleId, gateId));
            if (it != null)
                return it;
        }
        else {
            for (EventLogEntry eventLogEntry : eventLogEntries) {
                if (eventLogEntry instanceof GateCreatedEntry) {
                    GateCreatedEntry gateCreatedEntry = (GateCreatedEntry)eventLogEntry;
                    if (gateCreatedEntry != null && gateCreatedEntry.moduleId == moduleId && gateCreatedEntry.gateId == gateId)
                        return gateCreatedEntry;
                }
            }
        }
        return null;
    }

    public final GateDeletedEntry getGateDeletedEntry(int moduleId, int gateId) {
        ensureCacheBuilt();
        if (moduleIdAndGateIdToGateDeletedEntryMap != null) {
            var it = moduleIdAndGateIdToGateDeletedEntryMap.get(new Pair<Integer, Integer>(moduleId, gateId));
            if (it != null)
                return it;
        }
        else {
            for (EventLogEntry eventLogEntry : eventLogEntries) {
                if (eventLogEntry instanceof GateDeletedEntry) {
                    GateDeletedEntry gateDeletedEntry = (GateDeletedEntry)eventLogEntry;
                    if (gateDeletedEntry != null && gateDeletedEntry.moduleId == moduleId && gateDeletedEntry.gateId == gateId)
                        return gateDeletedEntry;
                }
            }
        }
        return null;
    }

    public final GateDisplayStringChangedEntry getGateDisplayStringChangedEntry(int moduleId, int gateId, int entryIndex) {
        ensureCacheBuilt();
        if (moduleIdAndGateIdToGateDisplayStringChangedVectorMap != null) {
            var it = moduleIdAndGateIdToGateDisplayStringChangedVectorMap.get(new Pair<Integer, Integer>(moduleId, gateId));
            if (it != null) {
                ArrayList<GateDisplayStringChangedEntry> gateDisplayStringChangedVector = it;
                for (GateDisplayStringChangedEntry jt : gateDisplayStringChangedVector)
                {
                    GateDisplayStringChangedEntry gateDisplayStringChangedEntry = jt;
                    if (gateDisplayStringChangedEntry.getEntryIndex() >= entryIndex)
                    {
                        return gateDisplayStringChangedEntry;
                    }
                }
            }
        }
        else {
            for (int index = entryIndex; index < eventLogEntries.size(); index++) {
                EventLogEntry eventLogEntry = eventLogEntries.get(index);
                if (eventLogEntry instanceof GateDisplayStringChangedEntry) {
                    GateDisplayStringChangedEntry gateDisplayStringChangedEntry = (GateDisplayStringChangedEntry)eventLogEntry;
                    if (gateDisplayStringChangedEntry != null && gateDisplayStringChangedEntry.moduleId == moduleId && gateDisplayStringChangedEntry.gateId == gateId)
                        return gateDisplayStringChangedEntry;
                }
            }
        }
        return null;
    }

    public final CustomDescriptionEntry getCustomDescriptionEntry(String type, int key) {
        ensureCacheBuilt();
        if (typeAndKeyToCustomDescriptionEntryMap != null) {
            var it = typeAndKeyToCustomDescriptionEntryMap.get(new Pair<String, Integer>(type, key));
            if (it != null)
                return it;
        }
        else {
            for (EventLogEntry eventLogEntry : eventLogEntries) {
                if (eventLogEntry instanceof CustomDescriptionEntry) {
                    CustomDescriptionEntry customDescriptionEntry = (CustomDescriptionEntry)eventLogEntry;
                    if (customDescriptionEntry != null && type.equals(customDescriptionEntry.type) && customDescriptionEntry.key == key)
                        return customDescriptionEntry;
                }
            }
        }
        return null;
    }

    public final CustomCreatedEntry getCustomCreatedEntry(String type, int key) {
        ensureCacheBuilt();
        if (typeAndKeyToCustomCreatedEntryMap != null) {
            var it = typeAndKeyToCustomCreatedEntryMap.get(new Pair<String, Integer>(type, key));
            if (it != null)
                return it;
        }
        else {
            for (EventLogEntry eventLogEntry : eventLogEntries) {
                if (eventLogEntry instanceof CustomCreatedEntry) {
                    CustomCreatedEntry customCreatedEntry = (CustomCreatedEntry)eventLogEntry;
                    if (customCreatedEntry != null && type.equals(customCreatedEntry.type) && customCreatedEntry.key == key)
                        return customCreatedEntry;
                }
            }
        }
        return null;
    }

    public final CustomDeletedEntry getCustomDeletedEntry(String type, int key) {
        ensureCacheBuilt();
        if (typeAndKeyToCustomDeletedEntryMap != null) {
            var it = typeAndKeyToCustomDeletedEntryMap.get(new Pair<String, Integer>(type, key));
            if (it != null)
                return it;
        }
        else {
            for (EventLogEntry eventLogEntry : eventLogEntries) {
                if (eventLogEntry instanceof CustomCreatedEntry) {
                    CustomDeletedEntry customDeletedEntry = (CustomDeletedEntry)eventLogEntry;
                    if (customDeletedEntry != null && type.equals(customDeletedEntry.type) && customDeletedEntry.key == key)
                        return customDeletedEntry;
                }
            }
        }
        return null;
    }

    public final CustomChangedEntry getCustomChangedEntry(String type, int key, int entryIndex) {
        ensureCacheBuilt();
        if (typeAndKeyToCustomChangedVectorMap != null) {
            var it = typeAndKeyToCustomChangedVectorMap.get(new Pair<String, Integer>(type, key));
            if (it != null) {
                ArrayList<CustomChangedEntry> customDisplayStringChangedVector = it;
                for (CustomChangedEntry jt : customDisplayStringChangedVector) {
                    CustomChangedEntry customChangedEntry = jt;
                    if (customChangedEntry.getEntryIndex() >= entryIndex)
                        return customChangedEntry;
                }
            }
        }
        else {
            for (int index = entryIndex; index < eventLogEntries.size(); index++) {
                EventLogEntry eventLogEntry = eventLogEntries.get(index);
                if (eventLogEntry instanceof CustomCreatedEntry) {
                    CustomChangedEntry customChangedEntry = (CustomChangedEntry)eventLogEntry;
                    if (customChangedEntry != null && type.equals(customChangedEntry.type) && customChangedEntry.key == key)
                        return customChangedEntry;
                }
            }
        }
        return null;
    }

    protected final void allocateCache() {
        // module
        moduleIdToModuleDescriptionEntryMap = new TreeMap<Integer, ModuleDescriptionEntry>();
        moduleIdToModuleCreatedEntryMap = new TreeMap<Integer, ModuleCreatedEntry>();
        moduleIdToModuleDeletedEntryMap = new TreeMap<Integer, ModuleDeletedEntry>();
        moduleIdToModuleDisplayStringChangedVectorMap = new TreeMap<Integer, ArrayList<ModuleDisplayStringChangedEntry>>();
        // connection
        moduleIdAndGateIdToConnectionDescriptionEntryMap = new HashMap<Pair<Integer, Integer>, ConnectionDescriptionEntry>();
        moduleIdAndGateIdToConnectionCreatedEntryMap = new HashMap<Pair<Integer, Integer>, ConnectionCreatedEntry>();
        moduleIdAndGateIdToConnectionDeletedEntryMap = new HashMap<Pair<Integer, Integer>, ConnectionDeletedEntry>();
        moduleIdAndGateIdToConnectionDisplayStringChangedVectorMap = new HashMap<Pair<Integer, Integer>, ArrayList<ConnectionDisplayStringChangedEntry>>();
        // gate
        moduleIdAndGateIdToGateDescriptionEntryMap = new HashMap<Pair<Integer, Integer>, GateDescriptionEntry> ();
        moduleIdAndGateIdToGateCreatedEntryMap = new HashMap<Pair<Integer, Integer>, GateCreatedEntry>();
        moduleIdAndGateIdToGateDeletedEntryMap = new HashMap<Pair<Integer, Integer>, GateDeletedEntry>();
        moduleIdAndGateIdToGateDisplayStringChangedVectorMap = new HashMap<Pair<Integer, Integer>, ArrayList<GateDisplayStringChangedEntry>>();
        // custom
        typeAndKeyToCustomDescriptionEntryMap = new HashMap<Pair<String, Long>, CustomDescriptionEntry>();
        typeAndKeyToCustomCreatedEntryMap = new HashMap<Pair<String, Long>, CustomCreatedEntry>();
        typeAndKeyToCustomDeletedEntryMap = new HashMap<Pair<String, Long>, CustomDeletedEntry>();
        typeAndKeyToCustomChangedVectorMap = new HashMap<Pair<String, Long>, ArrayList<CustomChangedEntry>> ();
    }

    protected final void buildCache() {
        ensureCacheAllocated();
        for (EventLogEntry eventLogEntry : eventLogEntries)
            cacheEventLogEntry(eventLogEntry);
    }

    protected final void ensureCacheAllocated() {
        if (moduleIdToModuleCreatedEntryMap == null)
            allocateCache();
    }

    protected final void ensureCacheBuilt() {
        if (moduleIdToModuleCreatedEntryMap == null)
            buildCache();
    }
}
