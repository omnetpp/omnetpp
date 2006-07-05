package org.omnetpp.common.displaymodel;


// TODO fill in the defaults
public class ConnectionDisplayString extends DisplayString {
    // contains the default fallback values for the different tags if a variable is used in that position
    public static final DisplayString VARIABLE_DEFAULTS 
        = new DisplayString("ls=black,1,s,l");
    // contains the default fallback values for the different tags if it is empty
    public static final DisplayString EMPTY_DEFAULTS 
        = new DisplayString("ls=black,1,s,l");

    
    public ConnectionDisplayString(IDisplayStringProvider owner, IDisplayStringProvider ancestor,
    								String value) {
        super(owner, ancestor, value);
        variableDefaults = VARIABLE_DEFAULTS;
        emptyDefaults = EMPTY_DEFAULTS;
    }

}
