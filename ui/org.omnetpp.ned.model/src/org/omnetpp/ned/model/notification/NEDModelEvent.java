package org.omnetpp.ned.model.notification;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.ned.model.NEDElement;

/**
 * Defines a generic model change
 * @author rhornig
 */
public abstract class NEDModelEvent {
    private static long staticSerial = 0;
    protected long serial; 
    protected NEDElement source;

    /**
     * @param source The NED model element generating the event
     */
    protected NEDModelEvent(NEDElement source) {
        super();
        Assert.isNotNull(source);
        this.source = source;
        serial = ++staticSerial;
    }

    /**
     * @return Which NED element caused the change notification
     */
    public NEDElement getSource() {
        return source;
    }

    @Override
    public String toString() {
        String sourceString = source.getAttribute("name");
        
        return "EVENT: "+serial+" FROM: "+source.getTagName() + 
                ((sourceString != null) ? " "+sourceString : "");
    }

    /**
     * @return The serial number of the event. Each event has a unique serial number,which
     *         increases as new events are created
     */
    public long getSerial() {
        return serial;
    }
}
