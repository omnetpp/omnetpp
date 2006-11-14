package org.omnetpp.ned2.model.notification;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.ned2.model.NEDElement;

/**
 * @author rhornig
 * Defines a generic model change
 */
public abstract class NEDModelEvent {
    protected NEDElement source;

    /**
     * @param source The NED model element generating the event
     */
    protected NEDModelEvent(NEDElement source) {
        super();
        Assert.isNotNull(source);
        this.source = source;
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
        
        return "FROM: "+source.getTagName() + 
                ((sourceString != null) ? " "+sourceString : "");
    }
}
