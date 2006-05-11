package org.omnetpp.ned2.model;


/**
 * @author rhornig
 * Interface for model objects that provide a display string (ie has a visual appearence)
 */
public interface IDisplayStringProvider {

    /**
     * Returns a display string object, which can be used to manipulate the properties
     * any change in the returned DisplayString object (is calling the set(...) method)
     * causes displayStringChanged() notification. 
     */
    public DisplayString getDisplayString();

    /**
     * Notifies that a display string change has occured
     */
    public void displayStringChanged();

}