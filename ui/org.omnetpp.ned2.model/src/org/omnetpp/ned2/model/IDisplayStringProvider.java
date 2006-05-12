package org.omnetpp.ned2.model;

import org.omnetpp.ned2.model.DisplayString.Prop;


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
     * @param changedProp The property that changed or NULL if it cannot be identified
     */
    public void displayStringChanged(Prop changedProp);

}