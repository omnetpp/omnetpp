package org.omnetpp.ned.editor.graph.model;

import org.omnetpp.ned2.model.DisplayString;

public interface IDisplayString {

    /**
     * Returns display string property, or null 
     */
    public DisplayString getDisplayString();

    /**
     * Sets the display string property 
     */
    public void setDisplayString(DisplayString dspString);

}