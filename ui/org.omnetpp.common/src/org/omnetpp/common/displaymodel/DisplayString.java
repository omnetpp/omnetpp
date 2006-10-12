package org.omnetpp.common.displaymodel;

import java.lang.ref.WeakReference;
import java.util.LinkedHashMap;
import java.util.Map;
import java.util.Scanner;
import java.util.Vector;

import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;

/**
 * This class is responsible for parsing and creating display strings in the correct format.
 * Defines all possible properties that can be used and also adds meta information to the 
 * properties
 * @author rhornig
 *
 */
// FIXME default handling is still not ok. Block size etc is not defaults to -1 if left empty
public class DisplayString implements IDisplayString {
    
    // contains the default fallback values for the different tags if a variable is used in that position
    public static final DisplayString VARIABLE_DEFAULTS 
        = new DisplayString("i=,,30;i2=,,30;is=40;b=40,40,rect,#8080ff,black,2;t=,t,blue;r=100,,black,1;bgb=-1,-1,grey75,black,2;bgg=100,1,grey50");
    // contains the default fallback values for the different tags if it is empty
    public static final DisplayString EMPTY_DEFAULTS 
        = new DisplayString("i=,,30;i2=,,30;is=40;b=-1,-1,rect,#8080ff,black,2;t=,t,blue;r=,,black,1;bgb=-1,-1,grey75,black,2;bgg=,1,grey50");

    // hold default values (if specified in derived calsses)
    // first look in 'this' then in 'defaults' then in 'variableDefaults' or 'emptyDefaults' 
    protected DisplayString variableDefaults = null; 
    protected DisplayString emptyDefaults = null;
    // use only a week reference so if the referenced fallback displaystring is deleted
    // along with the containing model element, it will not be held in the memory
    protected WeakReference<DisplayString> defaults = null;
    // stores a reference to the container object's (ie. compound module) display string
    // it is used to access values from there (example scale value)
//    protected WeakReference<IDisplayStringProvider> containerDpsProvider = null;
    // whether notification is enabled or not
    protected boolean notifyEnabled = true;
    
    // the owner of the displaystring
    protected IDisplayStringProvider owner = null;
    // listener for the display string changes (usually the controller)
    protected IDisplayStringChangeListener changeListener = null;
    
    // map that stores the currently available tag instances 
    private Map<String, TagInstance> tagMap = new LinkedHashMap<String, TagInstance>();

    public class TagInstance {
        public final static String EMPTY_VALUE = "";
        public final static String NONE_VALUE = "-";
        private String name = EMPTY_VALUE;
        private Vector<String> args = new Vector<String>(1);

        /**
         * Creates a tag by parsing the given string
         * @param tagString
         */
        public TagInstance(String tagString) {
            if (tagString != null)
                parseTag(tagString);
        }
        
        /**
         * @return The tagname
         */
        public String getName() {
            return name;   
        }
        
        /**
         * Returns a tag argument at the given position
         * @param pos
         * @return The value os that position or <code>EMPTY_VALUE</code> if does not exist 
         */
        public String getArg(int pos) {
            if (pos < 0 || pos >= args.size() || args.get(pos) == null) 
                return EMPTY_VALUE;
            return args.get(pos);
        }

        /**
         * Sets the value at a given position.
         * @param pos
         * @param newValue
         */
        public void setArg(int pos, String newValue) {
            // extend the vector if necessary
            args.setSize(Math.max(args.size(), pos+1));
            args.set(pos, newValue);
        }
        
        /**
         * Check if the tag's value is default
         * @return <code>true</code> if ALL tag values are missing or have the default value
         */
        public boolean isDefault() {
            for(String val : args)
                if(val != null && !EMPTY_VALUE.equals(val)) return false; 
            return true;
        }
        
        /**
         * @return the values part of the tag
         */
        public String getArgString() {
            StringBuffer sb = new StringBuffer(20);
            // check for the last non default value
            int endPos;
            for(endPos = args.size() - 1; endPos>0; endPos--)
                if(args.get(endPos) != null && !EMPTY_VALUE.equals(args.get(endPos))) break;
            // if there are unnecessary default values at the end, throw them away
            if(endPos < args.size() - 1) args.setSize(endPos + 1);
            
            boolean firstArg = true;
            for (String val : args) {
                if(firstArg) firstArg = false;
                    else sb.append(','); 
                
                if (val != null) sb.append(val);
            }
            return sb.toString();
        }
        
        /**
         * @return the string representation of the tag or <code>EMPTY_VALUE</code>
         * if all the args are default
         */
        public String toString() {
            // return an empty string if all the values are default
            if (isDefault()) return EMPTY_VALUE;
            return getName()+"="+getArgString();
        }
        
        // parse a single tag and its values into a string vector
        private void parseTag(String tagStr) {
            args = new Vector<String>(2);
            Scanner scr = new Scanner(tagStr);
            // TODO string literal and escaping must be correctly handled
            // StreamParser can handle comments too, maybe it would be a better choice
            scr.useDelimiter("=|,");
            // parse for the tagname
            if (scr.hasNext()) name = scr.next().trim();
            // parse for the tag values with a new tokenizer
            while (scr.hasNext())
                args.add(scr.next().trim());
        }
    }

    
    /**
     * Create a display string tokenizer class only derived classes alowed to be created
     * @param owner owner of the displaystring object (who has this displaystring) owner 
     * 		  will be notified about changes 
     * @param value The string to be parsed
     */
    public DisplayString(IDisplayStringProvider owner, String value) {
    	this(value);
    	this.owner = owner;
    }

    public DisplayString(String value) {
    	if (value != null)
    		set(value);

    	variableDefaults = VARIABLE_DEFAULTS;
        emptyDefaults = EMPTY_DEFAULTS;
    }

    /**
     * @return The currently set default display string
     */
    public DisplayString getDefaults() {
        if (defaults != null)
            return defaults.get();
        return null;
    }

    /**
     * Sets the default display string used if the current object has empty or null value for a
     * requested property
     * @param defaults
     */
    public void setDefaults(DisplayString defaults) {
        this.defaults = new WeakReference<DisplayString>(defaults);
    }

    /**
     * @return The currently set container display string
     */
//    public IDisplayStringProvider getContainerDisplayString() {
//    	if (containerDpsProvider != null)
//    		return containerDpsProvider.get();
//    	return null;
//	}

	/**
	 * Sets the container objects (for accessing properties like scale factor)
	 * requested property
	 * @param containerDpsProv
	 */
//	public void setContainerDisplayString(IDisplayStringProvider containerDpsProv) {
//		this.containerDpsProvider = new WeakReference<IDisplayStringProvider>(containerDpsProv);
//	}

	/**
     * Returns the value of the given tag on the given position
     * @param tag TagInstance to be checked
     * @param pos Position (0 based)
     * @return TagInstance arg's value or <code>EMPTY_VALUE</code> if empty 
     * or <code>null</code> if tag does not exist at all 
     */
    protected String getTagArg(Tag tag, int pos) {
        TagInstance tagInst = getTag(tag);
        // if tag doesn't exsist return null
        if (tagInst == null) return null;
        // check for the value itself
        String val = tagInst.getArg(pos);
        if(val == null) val = TagInstance.EMPTY_VALUE;
        return val;
    }
    
    /**
     * @param tagName
     * @param pos
     * @param defaultDspStr generic defaults 
     * @param defaultVariableDspStr defaults used if a variable is present at the location
     * @return TagInstance arg's value or <code>EMPTY_VALUE</code> if empty and no default is defined 
     * or <code>null</code> if tag does not exist at all. If a default diplay string was
     * provided with <code>setDefaults</code> it tries to look up the property from there
     *  
     */
    protected String getTagArgUsingDefs(Tag tagName, int pos) {
        TagInstance tag = getTag(tagName);
        // if the tag does'nt exist do not apply any defaults 
        if (tag == null) {
        	// if there is a default diplay string delegate the request there
        	if (getDefaults() != null)
        		return getDefaults().getTagArgUsingDefs(tagName, pos);
        	// no default display string so return NULL to signal that the whole tag is missing
            return null;
        }
        // get the value
        String value = tag.getArg(pos);
        if (variableDefaults !=null && value.startsWith("$"))
            value = variableDefaults.getTagArg(tagName, pos);
        // if tag was present, but the argument was empty, look for default values
        if (TagInstance.EMPTY_VALUE.equals(value) && getDefaults() != null) 
        	value = getDefaults().getTagArgUsingDefs(tagName, pos);
        // if the value is still empty or null get the local default values  if any
        if (emptyDefaults!=null && (value == null || TagInstance.EMPTY_VALUE.equals(value)))
            value = emptyDefaults.getTagArg(tagName, pos);
        // if no default was defined for this tag/argument return empty value
        if (value == null)
            return TagInstance.EMPTY_VALUE;
        
        return value;
    }

    /**
     * Sets the value of a tag at a given position. Extends the tag vector if necessary
     * @param tag Name of the tag
     * @param pos Position to be updated (0 based)
     * @param newValue New value to be stored
     */
    protected void setTagArg(Tag tag, int pos, String newValue) {
        TagInstance tagInstance = getTag(tag);
        // if the value is empty or null and the tag does not exist, do nothing
        if (tagInstance == null && (newValue == null || TagInstance.EMPTY_VALUE.equals(newValue)))
        	return;
        // if the tag does not exist add it to the map
        if (tagInstance == null) {
            tagInstance = new TagInstance(tag.name());
            tagMap.put(tag.name(), tagInstance);
        }
        tagInstance.setArg(pos, newValue);
    }
    
    /**
     * Returns the tag with a given name
     * @param tag equested tagname
     * @return The requested tag or <code>NULL</code> if does not exist
     */
    protected TagInstance getTag(Tag tag) {
        return tagMap.get(tag.name());
    }
    
    // parse the display string into tags along ;
    /**
     * Set the displayString and parse it from the given string
     * @param str Display string to be parsed
     */
    public void set(String str) {
    	tagMap.clear();
    	if (str != null) {
    		// tokenize along ;
    		Scanner scr = new Scanner(str);
    		scr.useDelimiter(";");
    		while (scr.hasNext()) {
    			TagInstance parsedTag = new TagInstance(scr.next().trim());
    			tagMap.put(parsedTag.getName(), parsedTag);
    		}
    	}
    	fireDisplayStringChanged(null);
    }

    /**
     * @return the full display string
     */
    public String toString() {
        StringBuffer sb = new StringBuffer(50);
        boolean firstTag = true;
        for(TagInstance tag : tagMap.values()) {
            String tagVal = tag.toString();
            if(!tagVal.equals("")) {
                if(firstTag) firstTag = false;
                    else sb.append(';');

                sb.append(tagVal);
            }
        }
        
        return sb.toString();
    }

    /**
     * @param property
     * @return Null if tag does not exist, TagInstance.EMPTY_VALUE if the value is empty or the property value itself
     */
    public String getAsString(Prop property) {
        return getTagArg(property.getTag(), property.getPos());
    }

    /**
     * Returns the property value as an Integer
     * @param property
     * @return The value as Integer or 0 if value was empty or non number, 
     *  and <code>null</code> if the tag was not present at all
     */
    public Integer getAsInteger(Prop property) {
        String strVal = getAsString(property);
        // if tag not present at all
        if(strVal == null || TagInstance.EMPTY_VALUE.equals(strVal)) return null;
        try {
            return Integer.valueOf(strVal);
        } catch (NumberFormatException e) { }
        return new Integer(0);
    }

    public Float getAsFloat(Prop property) {
        String strVal = getAsString(property);
        // if tag not present at all
        if(strVal == null || TagInstance.EMPTY_VALUE.equals(strVal)) return null;
        try {
            return Float.valueOf(strVal);
        } catch (NumberFormatException e) { }
        return new Float(0);
    }

    /* (non-Javadoc)
	 * @see org.omnetpp.ned2.model.IDisplayString#getAsStringDef(org.omnetpp.ned2.model.DisplayString.Prop)
	 */
    public String getAsStringDef(Prop property) {
        return getTagArgUsingDefs(property.getTag(), property.getPos());
    }

    /* (non-Javadoc)
	 * @see org.omnetpp.ned2.model.IDisplayString#getAsIntegerDef(org.omnetpp.ned2.model.DisplayString.Prop)
	 */
    public Integer getAsIntegerDef(Prop property) {
        String strVal = getAsStringDef(property);
        // if tag not present at all
        if(strVal == null || TagInstance.EMPTY_VALUE.equals(strVal)) 
        	return null;
        
        try {
            return Integer.valueOf(strVal);
        } catch (NumberFormatException e) { }
        return new Integer(0);
    }
    
    /* (non-Javadoc)
	 * @see org.omnetpp.ned2.model.IDisplayString#getAsIntDef(org.omnetpp.ned2.model.DisplayString.Prop, int)
	 */
    public int getAsIntDef(Prop propName, int defValue) {
        Integer propValue = getAsIntegerDef(propName);
        return propValue == null ? defValue : propValue.intValue();
    }

	/* (non-Javadoc)
	 * @see org.omnetpp.ned2.model.IDisplayString#getAsFloatDef(org.omnetpp.ned2.model.DisplayString.Prop, float)
	 */
	public float getAsFloatDef(Prop propName, float defValue) {
        try {
            String propValue = getAsStringDef(propName);
            return propValue == null ? defValue : Float.valueOf(propValue);
        } catch (NumberFormatException e) { }
        return defValue;
	}
	
	/**
     * Sets the specified property to the given value in the displaystring
     * @param property Property to be set
     * @param value 
     */
    public void set(Prop property, String value) {
        setTagArg(property.getTag(), property.getPos(), value);
        fireDisplayStringChanged(property);
    }

    public void set(Prop property, int value) {
        setTagArg(property.getTag(), property.getPos(), String.valueOf(value));
        fireDisplayStringChanged(property);
    }
    
    /* (non-Javadoc)
	 * @see org.omnetpp.ned2.model.IDisplayString#getScale()
	 */
	public float getScale() {
		return getAsFloatDef(Prop.MODULE_SCALE, 1.0f);
	}
    
    // helper functions for setting and getting the location and size properties
	/**
	 */
	/**
     * Converts the provided value (in pixel) to unit
     * @param pixel 
     * @param overrideScale If not NULL it will be used as scaling factor instead of the stored one
     * @return Value in units
	 */
	public final float pixel2unit(int pixel, Float overrideScale) {
        if (overrideScale != null)
            return pixel / overrideScale;
        
		return  pixel / getScale();
	}

	/**
	 * Converts the provided value (in unit) to pixel
	 * @param unit
     * @param overrideScale If not NULL it will be used as scaling factor instead of the stored one
	 * @return
	 */
	public final int unit2pixel(float unit, Float overrideScale) {
        if (overrideScale != null)
            return (int)(unit * overrideScale);
        
		return (int)(unit * getScale());
	}
	
    /**
     * Returns the range converted to pixels
     * @return
     */
    public int getRange(Float scale) {
    	int range = unit2pixel(getAsFloatDef(DisplayString.Prop.RANGE, -1.0f), scale);
    	if (range <= 0) range = -1;
    	return range;
    }

    /* (non-Javadoc)
	 * @see org.omnetpp.ned2.model.IDisplayString#getLocation()
	 */
    public Point getLocation(Float scale) {
        Float x = getAsFloat(Prop.X);
        Float y = getAsFloat(Prop.Y);
        // if it's unspecified in any direction we should return a NULL constraint
        if (x == null || y == null)
            return null;
        
        return new Point (unit2pixel(x, scale), unit2pixel(y, scale));
    }

    /**
     * @param location Where to place the element (in pixels)
     */
    public void setLocation(Point location, Float scale) {
    	// disable the notification so we will not send two notify for the two coordinate change
    	boolean tempNotifyState = notifyEnabled;
    	// disable the notification so we will not send two notify for the two coordinate change
    	notifyEnabled = false;
        // if location is not specified, remove the constraint from the display string
        if (location == null) {
            set(Prop.X, null);
            set(Prop.Y, null);
        } else { 
            set(Prop.X, String.valueOf(pixel2unit(location.x, scale)));
            set(Prop.Y, String.valueOf(pixel2unit(location.y, scale)));
        }
        // restore original notify state
    	notifyEnabled = tempNotifyState;
    	// we have explicitly disabled the notification, so we have to send it now manually
    	// be aware that location change always generates an X pos change notification regardless
    	// which coordinate has changed
    	fireDisplayStringChanged(Prop.X);
    }

    /* (non-Javadoc)
	 * @see org.omnetpp.ned2.model.IDisplayString#getSize()
	 */
    public Dimension getSize(Float scale) {
    	int width = unit2pixel(getAsFloatDef(Prop.WIDTH, -1.0f), scale);
        width = width > 0 ? width : -1; 
        int height = unit2pixel(getAsFloatDef(Prop.HEIGHT, -1.0f), scale);
        height = height > 0 ? height : -1; 
        
        return new Dimension(width, height);
    }

    /**
     * Converts the size given in pixels to unit based sorage
     * Sets the size of the element (in pixels)
     * @param size
     */
    public void setSize(Dimension size, Float scale) {
    	// disable the notification so we will not send two notify for the two coordinate change
    	boolean tempNotifyState = notifyEnabled;
    	notifyEnabled = false;
        // if the size is unspecified, remove the size constraint from the model
        if (size == null || size.width < 0 ) 
            set(Prop.WIDTH, null);
        else
            set(Prop.WIDTH, String.valueOf(pixel2unit(size.width, scale)));

        // if the size is unspecified, remove the size constraint from the model
        if (size == null || size.height < 0) 
            set(Prop.HEIGHT, null);
        else
            set(Prop.HEIGHT, String.valueOf(pixel2unit(size.height, scale)));

        // restore original notify state
    	notifyEnabled = tempNotifyState;
    	// we have explicitly disabled the notification, so we have to send it now manually
    	// be aware that size change always generates an width change notification regardless
    	// which coordinate has changed
    	fireDisplayStringChanged(Prop.HEIGHT);
    }
    
    /**
     * @return The size of module (pixel) if represented as compound (bgb tag)
     */
    public Dimension getCompoundSize(Float scale) {
    	int width = unit2pixel(getAsFloatDef(Prop.MODULE_WIDTH, -1.0f), scale);
        width = width > 0 ? width : -1; 
        int height = unit2pixel(getAsFloatDef(Prop.MODULE_HEIGHT, -1.0f), scale);
        height = height > 0 ? height : -1; 
        
        return new Dimension(width, height);
    }

    /**
     * Converts the size given in pixels to unit based sorage
     * Sets the size of the element (in pixels)
     * @param size
     */
    public void setCompoundSize(Dimension size, Float scale) {
    	// disable the notification so we will not send two notify for the two coordinate change
    	boolean tempNotifyState = notifyEnabled;
    	notifyEnabled = false;
        // if the size is unspecified, remove the size constraint from the model
        if (size == null || size.width < 0 ) 
            set(Prop.MODULE_WIDTH, null);
        else
            set(Prop.MODULE_WIDTH, String.valueOf(pixel2unit(size.width, scale)));

        // if the size is unspecified, remove the size constraint from the model
        if (size == null || size.height < 0) 
            set(Prop.MODULE_HEIGHT, null);
        else
            set(Prop.MODULE_HEIGHT, String.valueOf(pixel2unit(size.height, scale)));

        // restore original notify state
    	notifyEnabled = tempNotifyState;
    	// we have explicitly disabled the notification, so we have to send it now manually
    	// be aware that size change always generates an width change notification regardless
    	// which coordinate has changed
    	fireDisplayStringChanged(Prop.MODULE_WIDTH);
    }

    /**
     * Sets both the size and the location, but sends out only a SINGLE X position change message.
     * @param loc
     * @param size
     */
    public void setConstraint(Point loc, Dimension size, Float scale) {
    	// disable the notification so we will not send two notify for the two coordinate change
    	boolean tempNotifyState = notifyEnabled;
    	notifyEnabled = false;
    	
    	setLocation(loc, scale);
    	setSize(size, scale);
    	
        // restore original notify state
    	notifyEnabled = tempNotifyState;
    	// we have explicitly disabled the notification, so we have to send it now manually
    	// be aware that size change always generates an width change notification regardless
    	// which coordinate has changed
    	
    	// if the layout constraint has changed we send out an X coordinate changed event
    	fireDisplayStringChanged(Prop.X);
    }
    
    /**
     * Fire a property change notification 
     * @param changedProperty The changed property or NULL if it cannot be identified
     */
    private void fireDisplayStringChanged(Prop changedProperty) {
    	// notify the owner node (if set)
    	if (owner != null && notifyEnabled)
    		owner.propertyChanged(changedProperty);
    	// and notify the registered listener (if any)
    	if (changeListener != null && notifyEnabled)
    		changeListener.propertyChanged(changedProperty);
    }

	public IDisplayStringChangeListener getChangeListener() {
		return changeListener;
	}

	public void setChangeListener(IDisplayStringChangeListener listener) {
		this.changeListener = listener;
	}
}
