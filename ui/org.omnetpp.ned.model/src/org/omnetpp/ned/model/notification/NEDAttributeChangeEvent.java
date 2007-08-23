package org.omnetpp.ned.model.notification;

import org.omnetpp.ned.model.INEDElement;

/**
 * TODO add documentation
 *
 * @author rhornig
 */
public class NEDAttributeChangeEvent extends NEDModelChangeEvent {
    protected String attribute;
    protected Object oldValue;
    protected Object newValue;

    /**
     * @param source The model element generating the event
     * @param attribute The attribute that has changed
     * @param newValue The new value of the attribute
     * @param oldValue The old value of the attribute
     */
    public NEDAttributeChangeEvent(INEDElement source, String attribute, Object newValue, Object oldValue) {
        super(source);
        this.attribute = attribute;
        this.oldValue = oldValue;
        this.newValue = newValue;
    }

    /**
     * Returns the attribute that has changed
     */
    public String getAttribute() {
        return attribute;
    }

    /**
     * Returns the new value of the attribute
     */
    public Object getNewValue() {
        return newValue;
    }

    /**
     * Returns the original value of the attribute
     */
    public Object getOldValue() {
        return oldValue;
    }

    @Override
    public String toString() {
        return super.toString()+" ATTR: "+attribute+"=\""+newValue+"\" (old=\""+oldValue+"\")";
    }
}
