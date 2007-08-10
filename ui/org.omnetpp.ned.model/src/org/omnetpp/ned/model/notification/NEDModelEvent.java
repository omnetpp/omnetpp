package org.omnetpp.ned.model.notification;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.ned.model.INEDElement;

/**
 * Defines a generic model change
 *
 * @author rhornig
 */
public abstract class NEDModelEvent {
    private static long staticSerial = 0;
    protected long serial;
    protected INEDElement source;

    /**
     * @param source The NED model element generating the event
     */
    protected NEDModelEvent(INEDElement source) {
        super();
        Assert.isNotNull(source);
        this.source = source;
        serial = ++staticSerial;
    }

    /**
     * Returns which element caused the change notification
     */
    public INEDElement getSource() {
        return source;
    }

    @Override
    public String toString() {
        String sourceString = source.getAttribute("name");

        return "EVENT: "+serial+" FROM: "+source.getTagName() +
                ((sourceString != null) ? " "+sourceString : "");
    }

    /**
     * Returns the unique serial number of the event. (Serial numbers increase
     * as new events are created.)
     */
    public long getSerial() {
        return serial;
    }
}
