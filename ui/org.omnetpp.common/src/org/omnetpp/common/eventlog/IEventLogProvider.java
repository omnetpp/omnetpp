package org.omnetpp.common.eventlog;

import org.omnetpp.eventlog.engine.IEventLog;

public interface IEventLogProvider {
    public IEventLog getEventLog();
}
