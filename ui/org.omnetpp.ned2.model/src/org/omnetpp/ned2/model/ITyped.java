package org.omnetpp.ned2.model;

/**
 * @author rhornig
 * Elemets that has a type - like property
 */
public interface ITyped {
    /**
     * @return The type of the object
     */
    public String getType();

    /**
     * @param type
     */
    public void setType(String type);
}
