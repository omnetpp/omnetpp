package org.omnetpp.ned2.model;

// TODO specify the defaults correctly (module and background tags are missing
public class CompoundModuleDisplayString extends DisplayString {
    // contains the default fallback values for the different tags if a variable is used in that position
    public static final DisplayString VARIABLE_DEFAULTS 
        = new DisplayString("i=,,30;i2=,,30;is=40;b=40,40,rect,#8080ff,black,2;t=,t,blue;r=100,,black,1");
    // contains the default fallback values for the different tags if it is empty
    public static final DisplayString EMPTY_DEFAULTS 
        = new DisplayString("i=,,30;i2=,,30;is=40;b=-1,-1,rect,#8080ff,black,2;t=,t,blue;r=,,black,1;bgg=,,,grey;bgb=,,,,2");

    
    public CompoundModuleDisplayString(String value) {
        super(value);
        variableDefaults = VARIABLE_DEFAULTS;
        emptyDefaults = EMPTY_DEFAULTS;
    }

}
