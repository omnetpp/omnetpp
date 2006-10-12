package org.omnetpp.ned2.model;

/**
 * @author rhornig
 * Elements that have type-like property
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
