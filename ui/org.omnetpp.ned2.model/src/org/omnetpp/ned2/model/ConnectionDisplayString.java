package org.omnetpp.ned2.model;

// TODO fill in the defaults
public class ConnectionDisplayString extends DisplayString {
    // contains the default fallback values for the different tags if a variable is used in that position
    public static final DisplayString VARIABLE_DEFAULTS 
        = new DisplayString("");
    // contains the default fallback values for the different tags if it is empty
    public static final DisplayString EMPTY_DEFAULTS 
        = new DisplayString("");

    
    public ConnectionDisplayString(IDisplayStringProvider owner, IDisplayStringProvider ancestor, IDisplayStringProvider container,
    								String value) {
        super(owner, ancestor, container, value);
        variableDefaults = VARIABLE_DEFAULTS;
        emptyDefaults = EMPTY_DEFAULTS;
    }

}
