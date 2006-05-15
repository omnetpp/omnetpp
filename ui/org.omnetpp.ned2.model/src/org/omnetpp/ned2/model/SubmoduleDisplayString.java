package org.omnetpp.ned2.model;

import org.omnetpp.ned2.model.DisplayString.Prop;


public class SubmoduleDisplayString extends DisplayString {
    // contains the default fallback values for the different tags if a variable is used in that position
    public static final DisplayString VARIABLE_DEFAULTS 
        = new DisplayString("i=,,30;i2=,,30;is=40;b=40,40,rect,#8080ff,black,2;t=,t,blue;r=100,,black,1");
    // contains the default fallback values for the different tags if it is empty
    public static final DisplayString EMPTY_DEFAULTS 
        = new DisplayString("i=,,30;i2=,,30;is=40;b=-1,-1,rect,#8080ff,black,2;t=,t,blue;r=,,black,1");

    
    public SubmoduleDisplayString(IDisplayStringProvider owner, IDisplayStringProvider ancestor,
    								String value) {
        super(owner, ancestor, value);
        variableDefaults = VARIABLE_DEFAULTS;
        emptyDefaults = EMPTY_DEFAULTS;
    }
    
    public int getRange() {
    	int range = unit2pixel(getAsFloatDef(DisplayString.Prop.RANGE, -1.0f));
    	if (range <= 0) range = -1;
    	return range;
    }

	@Override
	public float getScale() {
		float scaleFactor = 1.0f;
		
		if (owner != null) {
			CompoundModuleNodeEx cm = ((SubmoduleNodeEx)owner).getCompoundModule();
			if (cm != null) {
				scaleFactor = ((CompoundModuleDisplayString)cm.getDisplayString()).getScale();
			}
		}
		return scaleFactor;
	}

}
