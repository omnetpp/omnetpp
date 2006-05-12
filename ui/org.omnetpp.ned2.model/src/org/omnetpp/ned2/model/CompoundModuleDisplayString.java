package org.omnetpp.ned2.model;

public class CompoundModuleDisplayString extends DisplayString {
    // contains the default fallback values for the different tags if a variable is used in that position
    public static final DisplayString VARIABLE_DEFAULTS 
        = new DisplayString("i=,,30;i2=,,30;is=40;b=40,40,rect,#8080ff,black,2;t=,t,blue;r=100,,black,1;bgb=-1,-1,grey75,black,2");
    // contains the default fallback values for the different tags if it is empty
    public static final DisplayString EMPTY_DEFAULTS 
        = new DisplayString("i=,,30;i2=,,30;is=40;b=-1,-1,rect,#8080ff,black,2;t=,t,blue;r=,,black,1;bgg=,,,grey;bgb=-1,-1,grey75,black,2");

    
    public CompoundModuleDisplayString(IDisplayStringProvider owner, IDisplayStringProvider ancestor,
    									String value) {
    	// a compound module display string does not have a container
        super(owner, ancestor, null, value);
        variableDefaults = VARIABLE_DEFAULTS;
        emptyDefaults = EMPTY_DEFAULTS;
        // use BGP instead of P and use BGB instead of B in compound modules to define the position and size
        XPosProp = Prop.MODULE_X;
        YPosProp = Prop.MODULE_Y;
        WidthProp = Prop.MODULE_WIDTH;
        HeightProp = Prop.MODULE_HEIGHT;
    }

	/* (non-Javadoc)
	 * @see org.omnetpp.ned2.model.DisplayString#getScale()
	 * compound module scaling
	 */
	@Override
	public float getScale() {
		float scaleFactor = getAsFloatDef(Prop.MODULE_SCALE, 1.0f);
		return scaleFactor;
	}
}
