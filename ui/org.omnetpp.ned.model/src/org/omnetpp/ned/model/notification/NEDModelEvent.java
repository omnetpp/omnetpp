package org.omnetpp.ned.model.notification;

import org.omnetpp.ned.model.INEDElement;

/**
 * Defines a generic model change
 *
 * @author rhornig
 */
public class NEDModelEvent {
    private static long staticSerial = 0;
    protected long serial;
    protected INEDElement source;

    /**
     * @param source The NED model element generating the event or null if unknown.
     */
    public NEDModelEvent(INEDElement source) {
        this.source = source;
        serial = ++staticSerial;
    }
    
    /**
     * Returns which element caused the change notification.
     * Null means the source is unknown and cannot be relied on.
     */
    public INEDElement getSource() {
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
