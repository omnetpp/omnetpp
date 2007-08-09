package org.omnetpp.common.displaymodel;

import java.lang.ref.WeakReference;
import java.util.LinkedHashMap;
import java.util.Map;
import java.util.Scanner;
import java.util.Vector;

import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;
import org.omnetpp.common.util.StringUtils;

/**
 * This class is responsible for parsing and creating display strings in the correct format.
 * Defines all possible properties that can be used and also adds meta information to the
 * properties
 *
 * @author rhornig
 */
public class DisplayString implements IDisplayString {

    // contains the default fallback values for the different tags if a variable is used in that position
    public static final DisplayString VARIABLE_DEFAULTS = new DisplayString(VARIABLE_DEFAULTS_STR);

    // contains the default fallback values for the different tags if it is empty
    public static final DisplayString EMPTY_DEFAULTS = new DisplayString(EMPTY_DEFAULTS_STR);

    // use only a weak reference, so if the referenced fallback display string is deleted
    // along with the containing model element, it will not be held in memory
    protected WeakReference<DisplayString> fallbackDisplayStringRef = null;

    // whether notification is enabled or not
    protected boolean notifyEnabled = true;

    // the owner of the display string
    protected IHasDisplayString owner = null;

    // map that stores the currently available tag instances
    private final Map<String, TagInstance> tagMap = new LinkedHashMap<String, TagInstance>();

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
         * @return The tag name
         */
        public String getName() {
            return name;
        }

        /**
         * Returns a tag argument at the given position
         * @param pos
         * @return The value of that position or <code>EMPTY_VALUE</code> if does not exist
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
        public boolean isEmpty() {
            for (String val : args)
                if (val != null && !EMPTY_VALUE.equals(val)) return false;
            return true;
        }

        /**
         * @return the values part of the tag
         */
        public String getArgString() {
            StringBuffer sb = new StringBuffer(20);
            // check for the last non default value
            int endPos;
            for (endPos = args.size() - 1; endPos>0; endPos--)
                if (args.get(endPos) != null && !EMPTY_VALUE.equals(args.get(endPos))) break;
            // if there are unnecessary default values at the end, throw them away
            if (endPos < args.size() - 1) args.setSize(endPos + 1);

            boolean firstArg = true;
            for (String val : args) {
                if (firstArg) firstArg = false;
                    else sb.append(',');

                if (val != null) sb.append(val);
            }
            return sb.toString();
        }

        /**
         * @return the string representation of the tag or <code>EMPTY_VALUE</code>
         * if all the args are default
         */
        @Override
        public String toString() {
            // return an empty string if all the values are default
            if (isEmpty()) return EMPTY_VALUE;
            return getName()+"="+getArgString();
        }

        // parse a single tag and its values into a string vector
        private void parseTag(String tagStr) {
            args = new Vector<String>(2);
            Scanner scr = new Scanner(tagStr);
            // TODO string literal and escaping must be correctly handled
            // StreamParser can handle comments too, maybe it would be a better choice
            scr.useDelimiter("=|,");
            // parse for the tag name
            if (scr.hasNext()) name = scr.next().trim();
            // parse for the tag values with a new tokenizer
            while (scr.hasNext())
                args.add(scr.next().trim());
        }
    }


    /**
     * Create a display string tokenizer class only derived classes allowed to be created
     * @param owner owner of the display string object (who has this display string) owner
     * 		  will be notified about changes
     * @param value The string to be parsed
     */
    public DisplayString(IHasDisplayString owner, String value) {
    	this(value);
    	this.owner = owner;
    }

    public DisplayString(String value) {
    	if (value != null)
    		set(value);
    }

    /**
     * @return The currently set fallback display string
     */
    public DisplayString getFallbackDisplayString() {
        if (fallbackDisplayStringRef != null)
            return fallbackDisplayStringRef.get();
        return null;
    }

    /**
     * Sets the fallback display string used if the current object has empty or null value for a
     * requested property (points to the type or ancestor display string)
     * @param defaults
     */
    public void setFallbackDisplayString(DisplayString defaults) {
        this.fallbackDisplayStringRef = new WeakReference<DisplayString>(defaults);
    }

	/**
     * Returns the value of the given tag on the given position
     * @param tag TagInstance to be checked
     * @param pos Position (0 based)
     * @return TagInstance arg's value or <code>EMPTY_VALUE</code> if empty
     * or <code>null</code> if tag does not exist at all
     */
    protected String getTagArg(Tag tag, int pos) {
        TagInstance tagInst = getTag(tag);
        // if tag doesn't exist return null
        if (tagInst == null) return null;
        // check for the value itself
        String val = tagInst.getArg(pos);
        if (val == null) val = TagInstance.EMPTY_VALUE;
        return val;
    }

    /**
     * Returns TagInstance arg's value or <code>EMPTY_VALUE</code> if empty and no default is defined
     * or <code>null</code> if tag does not exist at all. If a default display string was
     * provided with <code>setFallbackDisplayString</code> it tries to look up the property from there
     */
    protected String getTagArgUsingDefs(Tag tagName, int pos) {
        TagInstance tag = getTag(tagName);
        String value = (tag == null) ? "" : tag.getArg(pos);

        if (value.startsWith("$"))
            return VARIABLE_DEFAULTS.getTagArg(tagName, pos);
        else if (!"".equals(value))
            return value;
        else if (getFallbackDisplayString() != null)
            return getFallbackDisplayString().getTagArgUsingDefs(tagName, pos);
        else
            return EMPTY_DEFAULTS.getTagArg(tagName, pos);
    }

    public boolean containsProperty(Prop prop) {
        TagInstance tag = getTag(prop.getTag());
        String value = (tag == null) ? "" : tag.getArg(prop.getPos());

        if (!"".equals(value))
            return true;
        else if (getFallbackDisplayString() != null)
            return getFallbackDisplayString().containsProperty(prop);
        else
            return false;
    }

    public boolean containsTag(Tag tagName) {
        TagInstance tag = getTag(tagName);
        if (tag != null)
            return true;
        else if (getFallbackDisplayString() != null)
            return getFallbackDisplayString().containsTag(tagName);
        else
            return false;
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
        // remove the tag if every value is empty
        if (tagInstance.isEmpty())
            tagMap.remove(tag.name());
    }

    /**
     * Returns the tag with a given name
     * @param tag requested tag name
     * @return The requested tag or <code>NULL</code> if does not exist
     */
    protected TagInstance getTag(Tag tag) {
        return tagMap.get(tag.name());
    }

    /**
     * Set the displayString and parse it from the given string
     * @param newValue Display string to be parsed
     */
    public void set(String newValue) {
        String oldValue = toString();
    	tagMap.clear();
    	if (newValue != null) {
    	    // parse the display string into tags along ;
    		Scanner scr = new Scanner(newValue);
    		scr.useDelimiter(";");
    		while (scr.hasNext()) {
    			TagInstance parsedTag = new TagInstance(scr.next().trim());
    			tagMap.put(parsedTag.getName(), parsedTag);
    		}
    	}
    	fireDisplayStringChanged(null, newValue, oldValue);
    }

    /**
     * @return the full display string
     */
    @Override
    public String toString() {
        StringBuffer sb = new StringBuffer(50);
        boolean firstTag = true;
        for (TagInstance tag : tagMap.values()) {
            String tagVal = tag.toString();
            if (!tagVal.equals("")) {
                if (firstTag) firstTag = false;
                    else sb.append(';');

                sb.append(tagVal);
            }
        }

        return sb.toString();
    }

    /**
     * The property value. NULL if tag does not exist, TagInstance.EMPTY_VALUE if the value
     * is empty.
     */
    public String getAsStringLocal(Prop property) {
        return getTagArg(property.getTag(), property.getPos());
    }

//    public Float getAsFloatLocal(Prop property) {
//        String strVal = getAsStringLocal(property);
//        // if tag not present at all
//        if (strVal == null || TagInstance.EMPTY_VALUE.equals(strVal)) return null;
//        try {
//            return Float.valueOf(strVal);
//        } catch (NumberFormatException e) { }
//        return new Float(0);
//    }

    /* (non-Javadoc)
	 * @see org.omnetpp.ned2.model.IDisplayString#getAsStringDef(org.omnetpp.ned2.model.DisplayString.Prop)
	 */
    public String getAsString(Prop property) {
        return getTagArgUsingDefs(property.getTag(), property.getPos());
    }

    /* (non-Javadoc)
	 * @see org.omnetpp.ned2.model.IDisplayString#getAsIntegerDef(org.omnetpp.ned2.model.DisplayString.Prop)
	 */
    public Integer getAsInt(Prop property) {
        String strVal = getAsString(property);
        // if tag not present at all
        if (strVal == null || TagInstance.EMPTY_VALUE.equals(strVal))
        	return null;

        try {
            return Integer.valueOf(strVal);
        } catch (NumberFormatException e) { }
        return new Integer(0);
    }

    /* (non-Javadoc)
	 * @see org.omnetpp.ned2.model.IDisplayString#getAsIntDef(org.omnetpp.ned2.model.DisplayString.Prop, int)
	 */
    public int getAsInt(Prop propName, int defValue) {
        Integer propValue = getAsInt(propName);
        return propValue == null ? defValue : propValue.intValue();
    }

	public float getAsFloat(Prop propName, float defValue) {
        try {
            String propValue = getAsString(propName);
            return propValue == null ? defValue : Float.valueOf(propValue);
        } catch (NumberFormatException e) { }
        return defValue;
	}

	/**
     * Sets the specified property to the given value in the display string
     */
    public void set(Prop property, String newValue) {
        String oldValue = getAsStringLocal(property);
        setTagArg(property.getTag(), property.getPos(), newValue);
        fireDisplayStringChanged(property, newValue, oldValue);
    }

    /* (non-Javadoc)
	 * @see org.omnetpp.ned2.model.IDisplayString#getScale()
	 */
	public float getScale() {
		return getAsFloat(Prop.MODULE_SCALE, 1.0f);
	}

    // helper functions for setting and getting the location and size properties
	public final float pixel2unit(int pixel, Float overrideScale) {
        if (overrideScale != null)
            return pixel / overrideScale;

		return  pixel / getScale();
	}

	public final int unit2pixel(float unit, Float overrideScale) {
        if (overrideScale != null)
            return (int)(unit * overrideScale);

		return (int)(unit * getScale());
	}

    public int getRange(Float scale) {
    	int range = unit2pixel(getAsFloat(DisplayString.Prop.RANGE, -1.0f), scale);
    	if (range <= 0) range = -1;
    	return range;
    }

    public Point getLocation(Float scale) {
        Float x = getAsFloat(Prop.X, Integer.MIN_VALUE);
        Float y = getAsFloat(Prop.Y, Integer.MIN_VALUE);
        // if it's unspecified in any direction we should return a NULL constraint
        if (x.equals(Integer.MIN_VALUE) || y.equals(Integer.MIN_VALUE))
            return null;

        return new Point (unit2pixel(x, scale), unit2pixel(y, scale));
    }

    /**
     * Sets the location properties (P tag). If NULL is given both location property is cleared.
     * It fires property change notification for Prop.X
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
        }
        else {
            set(Prop.X, floatToString(pixel2unit(location.x, scale)));
            set(Prop.Y, floatToString(pixel2unit(location.y, scale)));
        }
        // restore original notify state
    	notifyEnabled = tempNotifyState;
    	// we have explicitly disabled the notification, so we have to send it now manually
    	// be aware that location change always generates an X pos change notification regardless
    	// which coordinate has changed
    	fireDisplayStringChanged(Prop.X, null, null);
    }

    public Dimension getSize(Float scale) {
    	int width = unit2pixel(getAsFloat(Prop.WIDTH, -1.0f), scale);
        width = width > 0 ? width : -1;
        int height = unit2pixel(getAsFloat(Prop.HEIGHT, -1.0f), scale);
        height = height > 0 ? height : -1;
        return new Dimension(width, height);
    }

    /**
     * Converts the size given in pixels to unit based size and sets the size of the element (B tag).
     * If negative number is given as width or height the corresponding property is deleted.
     * It fires property change notification for Prop.WIDTH
     */
    public void setSize(Dimension size, Float scale) {
    	// disable the notification so we will not send two notify for the two coordinate change
    	boolean tempNotifyState = notifyEnabled;
    	notifyEnabled = false;
        // if the size is unspecified, remove the size constraint from the model
        if (size == null || size.width < 0 )
            set(Prop.WIDTH, null);
        else
            set(Prop.WIDTH, floatToString(pixel2unit(size.width, scale)));

        // if the size is unspecified, remove the size constraint from the model
        if (size == null || size.height < 0)
            set(Prop.HEIGHT, null);
        else
            set(Prop.HEIGHT, floatToString(pixel2unit(size.height, scale)));

        // restore original notify state
    	notifyEnabled = tempNotifyState;
    	// we have explicitly disabled the notification, so we have to send it now manually
    	// be aware that size change always generates an width change notification regardless
    	// which coordinate has changed
    	fireDisplayStringChanged(Prop.WIDTH, null, null);
    }

    public Dimension getCompoundSize(Float scale) {
    	int width = unit2pixel(getAsFloat(Prop.MODULE_WIDTH, -1.0f), scale);
        width = width > 0 ? width : -1;
        int height = unit2pixel(getAsFloat(Prop.MODULE_HEIGHT, -1.0f), scale);
        height = height > 0 ? height : -1;

        return new Dimension(width, height);
    }

    /**
     * Converts the size given in pixels to unit based size and
     * sets the size of the element (BGB tag). If negative number is given as width or height
     * the corresponding property is deleted.
     * It fires property change notification for Prop.MODULE_WIDTH
     */
    public void setCompoundSize(Dimension size, Float scale) {
    	// disable the notification so we will not send two notify for the two coordinate change
    	boolean tempNotifyState = notifyEnabled;
    	notifyEnabled = false;
        // if the size is unspecified, remove the size constraint from the model
        if (size == null || size.width < 0 )
            set(Prop.MODULE_WIDTH, null);
        else
            set(Prop.MODULE_WIDTH, floatToString(pixel2unit(size.width, scale)));

        // if the size is unspecified, remove the size constraint from the model
        if (size == null || size.height < 0)
            set(Prop.MODULE_HEIGHT, null);
        else
            set(Prop.MODULE_HEIGHT, floatToString(pixel2unit(size.height, scale)));

        // restore original notify state
    	notifyEnabled = tempNotifyState;
    	// we have explicitly disabled the notification, so we have to send it now manually
    	// be aware that size change always generates an width change notification regardless
    	// which coordinate has changed
    	fireDisplayStringChanged(Prop.MODULE_WIDTH, null, null);
    }

    /**
     * Sets both the size and the location (B and P tag), but sends out only a SINGLE X position change property change notification.
     * It fires property change notification for Prop.X
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
    	fireDisplayStringChanged(Prop.X, null, null);
    }

    /**
     * Fire a property change notification
     * @param changedProperty The changed property or NULL if it cannot be identified
     */
    private void fireDisplayStringChanged(Prop changedProperty, Object newValue, Object oldValue) {
    	// notify the owner node (if set)
    	if (owner != null && notifyEnabled)
    		owner.propertyChanged(this, changedProperty, newValue, oldValue);
    }

    /**
     * @param value
     * @return The converted floating pint number (with removed .0 at the end)
     */
    private String floatToString(float value) {
        return StringUtils.chomp(String.valueOf(value), ".0");
    }

    /**
     * Dump all supported tags
     */
    public static void dumpSupportedTags() {
        for (Prop p : Prop.values()) {
            System.out.println(p.getTag()+" "+p.getPos()+" "+p.getGroup()+" - "+p.getVisibleName()
                    +": "+p.getVisibleDesc());
        }
    }

//    static {
//        dumpSupportedTags();
//    }
}

