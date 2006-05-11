package org.omnetpp.ned2.model;

import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;

// TODO specify the defaults correctly (module and background tags are missing
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
    }

    // overrida all location and size setter to work with module propertie tags bgb bgp instean of b and p
    public Point getLocation() {
        Float x = getAsFloat(Prop.MODULE_X);
        Float y = getAsFloat(Prop.MODULE_Y);
        // if it's unspecified in any direction we should return a NULL constraint
        if (x == null || y == null)
            return null;
        
        return new Point (unit2pixel(x), unit2pixel(y));
    }

    public void setLocation(Point location) {
        
        // if location is not specified, remove the constraint from the display string
        if (location == null) {
            set(Prop.MODULE_X, null);
            set(Prop.MODULE_Y, null);
        } else { 
            set(Prop.MODULE_X, String.valueOf(pixel2unit(location.x)));
            set(Prop.MODULE_Y, String.valueOf(pixel2unit(location.y)));
        }
    }

    public Dimension getSize() {
        int width = unit2pixel(getAsFloatDef(Prop.MODULE_WIDTH, -1.0f));
        width = width > 0 ? width : -1; 
        int height = unit2pixel(getAsFloatDef(Prop.MODULE_HEIGHT, -1.0f));
        height = height > 0 ? height : -1; 
        
        return new Dimension(width, height);
    }

    public void setSize(Dimension size) {
        // if the size is unspecified, remove the size constraint from the model
        if (size == null || size.width < 0 ) 
            set(Prop.MODULE_WIDTH, null);
        else
            set(Prop.MODULE_WIDTH, String.valueOf(pixel2unit(size.width)));

        // if the size is unspecified, remove the size constraint from the model
        if (size == null || size.height < 0) 
            set(Prop.MODULE_HEIGHT, null);
        else
            set(Prop.MODULE_HEIGHT, String.valueOf(pixel2unit(size.height)));
    }
}
