package org.omnetpp.eventlog;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.eventlog.entry.ModuleDescriptionEntry;

public class EventLogFacade
{
    protected IEventLog eventLog;

    public EventLogFacade(IEventLog eventLog) {
        Assert.isTrue(eventLog != null);
        this.eventLog = eventLog;
    }

    public final void setEventLog(IEventLog eventLog) {
        Assert.isTrue(eventLog != null);
        this.eventLog = eventLog;
    }

    public void synchronize(int change) {
        // NOTE: we don't call synchronize on eventLog to avoid calling it multiple times from multiple facades
    }

    public final long getFirstEventNumber() {
        IEvent event = eventLog.getFirstEvent();
        return event != null ? event.getEventNumber() : -1;
    }

    public final long getLastEventNumber() {
        IEvent event = eventLog.getLastEvent();
        return event != null ? event.getEventNumber() : -1;
    }

    public final BigDecimal getFirstSimulationTime() {
        IEvent event = eventLog.getFirstEvent();
        return event != null ? event.getSimulationTime() : BigDecimal.getMinusOne();
    }

    public final BigDecimal getLastSimulationTime() {
        IEvent event = eventLog.getLastEvent();
        return event != null ? event.getSimulationTime() : BigDecimal.getMinusOne();
    }

    public String ModuleDescriptionEntry_getModuleFullPath(ModuleDescriptionEntry moduleDescriptionEntry) {
        String fullPath = "";
        while (moduleDescriptionEntry != null) {
            fullPath = moduleDescriptionEntry.fullName + fullPath;
            moduleDescriptionEntry = eventLog.getEventLogEntryCache().getModuleDescriptionEntry(moduleDescriptionEntry.parentModuleId);
            if (moduleDescriptionEntry != null)
                fullPath = "." + fullPath;
        }
        return fullPath;
    }

    public IEvent getNonFilteredEventForEventNumber(long eventNumber) {
        EventLog nonFilteredEventLog = eventLog instanceof EventLog ? (EventLog)eventLog : null;
        if (nonFilteredEventLog != null)
            return nonFilteredEventLog.getEventForEventNumber(eventNumber);
        else
            return ((FilteredEventLog)eventLog).getEventLog().getEventForEventNumber(eventNumber);
    }
}
