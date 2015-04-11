/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.eventlog;

import org.omnetpp.eventlog.engine.IEventLog;

public interface IEventLogProvider {
    public IEventLog getEventLog();
}
