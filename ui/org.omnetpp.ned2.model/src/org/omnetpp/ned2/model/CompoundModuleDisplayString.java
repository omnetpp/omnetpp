package org.omnetpp.ned2.model;

import org.eclipse.draw2d.geometry.Point;

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
        super(owner, ancestor, value);
        variableDefaults = VARIABLE_DEFAULTS;
        emptyDefaults = EMPTY_DEFAULTS;
        // BGB instead of B in compound modules to define the size
        // we do not need position constraint tough (as we are using a position based toolbar layout)
        XPosProp = null;
        YPosProp = null;
        WidthProp = Prop.MODULE_WIDTH;
        HeightProp = Prop.MODULE_HEIGHT;
    }

	/* (non-Javadoc)
	 * @see org.omnetpp.ned2.model.DisplayString#getScale()
	 * compound module scaling
	 */
	@Override
	public float getScale() {
		return getAsFloatDef(Prop.MODULE_SCALE, 1.0f);
	}
	
	
	/**
	 * Converts tick distance from unit to pixel
	 * @return The tick distance between the major gridlines (in pixels)
	 */
	public int getGridTickDistance() {
		return unit2pixel(getAsIntDef(DisplayString.Prop.MODULE_TICKDISTANCE, -1));
	}
}
