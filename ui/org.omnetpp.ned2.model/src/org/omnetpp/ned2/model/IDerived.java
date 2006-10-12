package org.omnetpp.ned2.model;

/**
 * @author rhornig
 * Objects that can extend other objects, ie they are derived objects.
 */
public interface IDerived {
    
    /**
     * 
     * @return The base object's name
     */
    public String getExtends();
    
    /**
     * @param ext The object's name that is extended
     */
    public void setExtends(String ext);
}
