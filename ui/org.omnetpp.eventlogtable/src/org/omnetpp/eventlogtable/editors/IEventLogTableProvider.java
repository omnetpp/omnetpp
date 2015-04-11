/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.eventlogtable.editors;

import org.omnetpp.eventlogtable.widgets.EventLogTable;

public interface IEventLogTableProvider {
    public EventLogTable getEventLogTable();
}
