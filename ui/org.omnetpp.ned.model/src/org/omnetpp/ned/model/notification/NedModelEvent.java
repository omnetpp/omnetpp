/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.model.notification;

import org.omnetpp.ned.model.INedElement;

/**
 * Defines a generic model change
 *
 * @author rhornig
 */
public class NedModelEvent {
    private static long staticSerial = 0;
    protected long serial;
    protected INedElement source;

    /**
     * @param source The NED model element generating the event, or null if unknown.
     */
    public NedModelEvent(INedElement source) {
        this.source = source;
        serial = ++staticSerial;
    }

    /**
     * Returns which element caused the change notification.
     * Null means the source is unknown and cannot be relied on.
     */
    public INedElement getSource() {
        return source;
    }

    /**
     * Returns the unique serial number of the event. (Serial numbers increase
     * as new events are created.)
     */
    public long getSerial() {
        return serial;
    }

    @Override
    public String toString() {
        return getClass().getSimpleName() + " #" + serial + " from: " + source;
    }
}
