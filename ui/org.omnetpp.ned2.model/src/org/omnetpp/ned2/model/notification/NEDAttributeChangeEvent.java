package org.omnetpp.ned2.model.notification;

import org.omnetpp.ned2.model.NEDElement;

public class NEDAttributeChangeEvent extends NEDModelEvent {
    protected String attribute;
    protected Object oldValue;
    protected Object newValue;
    
    /**
     * @param source The model element generating the event
     * @param attribute The attribute that has changed
     * @param newValue The new value of the attribute
     * @param oldValue The old value of the attribute
     */
    public NEDAttributeChangeEvent(NEDElement source, String attribute, Object newValue, Object oldValue) {
        super(source);
        this.attribute = attribute;
        this.oldValue = oldValue;
        this.newValue = newValue;
    }

    /**
     * @return The attribute that has changed 
     */
    public String getAttribute() {
        return attribute;
    }

    /**
     * @return The new value of the attribute
     */
    public Object getNewValue() {
        return newValue;
    }

    /**
     * @return The original value of the attribute
     */
    public Object getOldValue() {
        return oldValue;
    }

    @Override
    public String toString() {
        return super.toString()+" attr: "+attribute+"=\""+newValue+"\" (old=\""+oldValue+"\")";
    }
}
