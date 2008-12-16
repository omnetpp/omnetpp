/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.eventlog;

import org.omnetpp.common.ui.ViewWithMessagePart;
import org.omnetpp.eventlog.engine.IEventLog;
import org.omnetpp.eventlog.engine.SimulationBeginEntry;

/**
 * Serves as a base class for views which show an event log file.
 */
public abstract class EventLogView extends ViewWithMessagePart implements IEventLogProvider {
    public abstract IEventLog getEventLog();

    protected void updateTitleToolTip() {
        IEventLog eventLog = getEventLog();

        if (eventLog == null)
            setTitleToolTip("");
        else {
            SimulationBeginEntry entry = eventLog.getSimulationBeginEntry();

            if (entry == null)
                setTitleToolTip("");
            else
                setTitleToolTip(entry.getRunId());
        }
    }
}
