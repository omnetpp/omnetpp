package org.omnetpp.ned2.model;

import java.util.LinkedHashMap;
import java.util.Map;
import java.util.Scanner;
import java.util.Vector;

import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;
import org.omnetpp.ned2.model.pojo.ModuleInterfaceNode;

/**
 * This class is responsible for parsing and creating display strings in the correct format.
 * Defines all possible properties that can be used and also adds meta information to the 
 * properties
 * @author rhornig
 *
 */
// FIXME default handling is still not ok. Block size etc is not defaults to -1 if left empty
public class DisplayString {

    // the argument types supported by the display string
    public enum PropType { STRING, INTEGER, UNIT, COLOR, IMAGE }

    public enum Tag { p, b, i, is, i2, r, q, t, tt,  // submodule tags 
                      bgp, bgb, bgi, bgtt, bgg, bgs, // compound module background tags
                      m, a, ls, bp }                 // connection tags
    
    // tag grouping 
    public enum PropGroup {
        Position, Shape, Style, Image, Range, Text, Connection, Misc
    }
    // describes ALL possible tag values and arguments and adds additional info, 
    // including type, tagname, position inside the tag and
    // user readable name and description
    public enum Prop {
        // SUBMODULE / SIMPLEMODULE propeties
        // do not change the first and last element of the property group
        X(Tag.p, 0, PropType.UNIT , PropGroup.Position , "x", "X position of the module"),
        Y(Tag.p, 1, PropType.UNIT, PropGroup.Position, "y", "Y position of the module"),
        LAYOUT(Tag.p, 2, PropType.STRING, PropGroup.Position, "layout", "Layouting algorithm. (row,r / column,col,c / matrix,m / ring,ri / exact,e,x)"),
        LAYOUT_PAR1(Tag.p, 3, PropType.STRING, PropGroup.Position, "layout par1", "1st layout parameter"),
        LAYOUT_PAR2(Tag.p, 4, PropType.STRING, PropGroup.Position, "layout par2", "2nd layout parameter"),
        LAYOUT_PAR3(Tag.p, 5, PropType.STRING, PropGroup.Position, "layout par3", "3rd layout parameter"),
        // B tag
        WIDTH(Tag.b, 0, PropType.UNIT, PropGroup.Position, "shape width", "Width of shape. Default: match the image size or shape height (-1)"),
        HEIGHT(Tag.b, 1, PropType.UNIT, PropGroup.Position, "shape height", "Height of shape. Default: match the image size or shape width (-1)"),
        SHAPE(Tag.b, 2, PropType.STRING, PropGroup.Shape, "shape", "Shape of object (rect / rect2 / rrect / oval / tri / tri2 / hex / hex2). Default: rect"),
        // former O tag
        FILLCOL(Tag.b, 3, PropType.COLOR, PropGroup.Style, "shape fill color", "Fill color of the shape. Default: light blue"),
        BORDERCOL(Tag.b, 4, PropType.COLOR, PropGroup.Style, "shape border color", "Border color of the shape. Default: black"),
        BORDERWIDTH(Tag.b, 5, PropType.INTEGER, PropGroup.Style, "shape border width", "Border width of the shape. Default: 2"),
        // I tag
        IMAGE(Tag.i, 0, PropType.IMAGE, PropGroup.Image, "image", "An icon or image representing the object"),
        IMAGECOLOR(Tag.i, 1, PropType.COLOR, PropGroup.Image, "image color", "A color to colorize the image"),
        IMAGECOLORPCT(Tag.i, 2, PropType.INTEGER, PropGroup.Image, "image colorization", "Amount of colorization in percent Default: 30%"),
        // IS tag
        IMAGESIZE(Tag.is, 0, PropType.STRING, PropGroup.Image, "image size", "The size of the image (vs / s / l / vl)"),
        // I2 tag
        OVIMAGE(Tag.i2, 0, PropType.IMAGE, PropGroup.Image, "overlay image", "An icon or image added to the upper rigt corner of the original image"),
        OVIMAGECOLOR(Tag.i2, 1, PropType.COLOR, PropGroup.Image, "overlay image color", "A color to colorize the overlay image"),
        OVIMAGECOLORPCT(Tag.i2, 2, PropType.INTEGER, PropGroup.Image, "overlay image colorization", "Amount of colorization in percent Default: 30%"),
        // R tag
        RANGE(Tag.r, 0, PropType.UNIT, PropGroup.Range, "range", "Radius of the range indicator"),
        RANGEFILLCOL(Tag.r, 1, PropType.COLOR, PropGroup.Range, "range fill color", "Fill color of the range indicator"),
        RANGEBORDERCOL(Tag.r, 2, PropType.COLOR, PropGroup.Range, "range border color", "Border color of the range indicator. Default: black"),
        RANGEBORDERWIDTH(Tag.r, 3, PropType.INTEGER, PropGroup.Range, "range border width", "Border width of the range indicator. Default: 1"),
        // Q tag
        QUEUE(Tag.q, 0, PropType.STRING, PropGroup.Text, "queue", "Queue name to display"),
        // T tag
        TEXT(Tag.t, 0, PropType.STRING, PropGroup.Text, "text", "Additional text to display"),
        TEXTPOS(Tag.t, 1, PropType.STRING, PropGroup.Text, "text position", "Position of the text (l / r / t). Default: t"),
        TEXTCOLOR(Tag.t, 2, PropType.COLOR, PropGroup.Text, "text color", "Color of the displayed text. Default: blue"),
        // TT tag
        TOOLTIP(Tag.tt, 0, PropType.STRING, PropGroup.Text, "tooltip", "Tooltip to be displayed over the object"),
        // END of SUB/SIMPLE MODULE properties

        // START of COMPOUNDMODULE properties
        // BGP tag
        // do not change the first and last element of the property group
        MODULE_X(Tag.bgp, 0, PropType.UNIT, PropGroup.Position, "background x", "Module background horizontal displacement"),
        MODULE_Y(Tag.bgp, 1, PropType.UNIT, PropGroup.Position, "background y", "Module background vertical displacement"),
        // BGB tag
        MODULE_WIDTH(Tag.bgb, 0, PropType.UNIT, PropGroup.Position, "background width", "Width of shape. Default: match the image size or shape height (-1)"),
        MODULE_HEIGHT(Tag.bgb, 1, PropType.UNIT, PropGroup.Position, "background height", "Height of shape. Default: match the image size or shape width (-1)"),
        MODULE_FILLCOL(Tag.bgb, 2, PropType.COLOR, PropGroup.Style, "background fill color", "Fill color of the shape. Default: light blue"),
        MODULE_BORDERCOL(Tag.bgb, 3, PropType.COLOR, PropGroup.Style, "background border color", "Border color of the shape. Default: black"),
        MODULE_BORDERWIDTH(Tag.bgb, 4, PropType.INTEGER, PropGroup.Style, "background border width", "Border width of the shape. Default: 2"),
        // BGTT tag
        MODULE_TOOLTIP(Tag.bgtt, 0, PropType.STRING, PropGroup.Text, "background tooltip", "Tooltip to be displayed over the module's background"),
        // BGI tag
        MODULE_IMAGE(Tag.bgi, 0, PropType.IMAGE, PropGroup.Image, "background image", "An image to be displayed as a module background"),
        MODULE_IMAGEARRANGEMENT(Tag.bgi, 1, PropType.STRING, PropGroup.Image, "background arrangement", "How to arrange the module's background image (fix, tile, strech, center) Default: fix"),
        // BGG tag
        MODULE_TICKDISTANCE(Tag.bgg, 0, PropType.UNIT, PropGroup.Misc, "background grid tick distance", "Distance between two major ticks measured in units"),
        MODULE_TICKNUMBER(Tag.bgg, 1, PropType.INTEGER, PropGroup.Misc, "background grid tick number", "Number of minor ticks between two major one"),
        MODULE_GRIDCOL(Tag.bgg, 3, PropType.COLOR, PropGroup.Style, "background grid color", "Color of grid"),
        // module scaling mixel per unit
        MODULE_SCALE(Tag.bgs, 0, PropType.UNIT, PropGroup.Misc, "background scaling", "Number of pixels per unit"),
        MODULE_UNIT(Tag.bgs, 1, PropType.STRING, PropGroup.Text, "background unit", "Name of measurement unit"),
        // END of COMPOUNDMODULE properties
        
        // START of CONNECTION properties
        // do not change the first and last element of the property group
        ROUTING_MODE(Tag.m, 0, PropType.STRING, PropGroup.Connection, "routing", "Routing mode ([m]anual, manhatta[n], [s]hortestpath) Default: manual"),
        ROUTING_CONSTRAINT(Tag.m, 1, PropType.STRING, PropGroup.Connection, "routing constraint", "possible constraints: ([s]outh, [n]orth, [e]ast, [w]est)"),
        // a tag (anchoring)
        ANCHOR_SRCX(Tag.a, 0, PropType.INTEGER, PropGroup.Position, "source anchor x", "Relative horizontal position of the anchor on the source module side"),
        ANCHOR_SRCY(Tag.a, 1, PropType.INTEGER, PropGroup.Position, "source anchor y", "Relative vertical position of the anchor on the source module side"),
        ANCHOR_DSTX(Tag.a, 2, PropType.INTEGER, PropGroup.Position, "destination anchor x", "Relative horizontal position of the anchor on the destination module side"),
        ANCHOR_DSTY(Tag.a, 3, PropType.INTEGER, PropGroup.Position, "destination anchor y", "Relative vertical position of the anchor on the destination module side"),
        // ls tag (line styling)
        CONNECTION_COL(Tag.ls, 0, PropType.COLOR, PropGroup.Style, "color", "Connection color. Default: black"),
        CONNECTION_WIDTH(Tag.ls, 1, PropType.INTEGER, PropGroup.Style, "line width", "Connection line width. Default: 1"),
        CONNECTION_STYLE(Tag.ls, 2, PropType.STRING, PropGroup.Style, "line style", "Connection line style ([s]olid, [d]otted). Default: solid"),
        CONNECTION_SEGMENTS(Tag.ls, 3, PropType.STRING, PropGroup.Style, "segments", "Connection segmenets ([l]ine, [s]pline). Default: line"),
        // bp tag (bendpoints)
        BENDPOINTS(Tag.bp, 0, PropType.UNIT, PropGroup.Connection, "bendpoints", "bendpoint locations");
        // END of CONNECTION properties
        // end of tag definition
        
        // define additional metadata for the tags
        private final Tag tag;
        private final int pos;
        private final PropType type;
        private final PropGroup group;
        private final String visibleName;
        private final String visibleDesc;
        
        Prop(Tag tag, int tagPos, PropType argType, PropGroup tagGroup, 
                String visibleName, String visibleDesc ) {
            this.tag = tag;
            this.pos = tagPos;
            this.type = argType;
            this.group = tagGroup;
            this.visibleName = visibleName;
            this.visibleDesc = visibleDesc;
        }

        public PropType getType() {
            return type;
        }

        public Tag getTag() {
            return tag;
        }

        public PropGroup getGroup() {
            return group;
        }

        public int getPos() {
            return pos;
        }

        public String getVisibleName() {
            return visibleName;
        }

        public String getVisibleDesc() {
            return visibleDesc;
        }
    } // end of TagArg enum definition
    
    // hold default values (if specified in derived calsses)
    protected DisplayString variableDefaults = null; 
    protected DisplayString emptyDefaults = null;
    
    // the owner of the displaystring
    protected IDisplayStringProvider owner = null;
    // the direct ancestor's displaystring
    protected IDisplayStringProvider ancestor = null;
    // the display string of the containing module
    protected IDisplayStringProvider container = null;
    
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
     * @param ancestor The direct ancestor of this module in the type hierarchy (used for display string fallback)
     * @param container The container's display string used to get scaling value
     * @param value The string to be parsed
     */
    protected DisplayString(IDisplayStringProvider owner, IDisplayStringProvider ancestor, IDisplayStringProvider container,
    		             String value) {
    	this(value);
    	this.owner = owner;
    	this.ancestor = ancestor;
    	this.container = container;
    }

    protected DisplayString(String value) {
    	if (value != null)
    		set(value);
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
     * or <code>null</code> if tag does not exist at all 
     */
    protected String getTagArgUsingDefs(Tag tagName, int pos) {
        TagInstance tag = getTag(tagName);
        // if the tag does'nt exist do not apply any defaults 
        if (tag == null) 
            return null;
        // get the value
        String value = tag.getArg(pos);
        if (variableDefaults !=null && value.startsWith("$"))
            value = variableDefaults.getTagArg(tagName, pos);
        // if tag was present, but the argument was empty, look for default values
        if (emptyDefaults!=null && TagInstance.EMPTY_VALUE.equals(value))
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
        // if the tag dos not exist add it to the map
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
    	fireDisplayStringChanged();
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
        return getTagArg(property.tag, property.pos);
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
        if(strVal == null) return null;
        try {
            return Integer.valueOf(strVal);
        } catch (NumberFormatException e) { }
        return new Integer(0);
    }

    public Float getAsFloat(Prop property) {
        String strVal = getAsString(property);
        // if tag not present at all
        if(strVal == null) return null;
        try {
            return Float.valueOf(strVal);
        } catch (NumberFormatException e) { }
        return new Float(0);
    }

    public String getAsStringDef(Prop property) {
        return getTagArgUsingDefs(property.tag, property.pos);
    }

    public Integer getAsIntegerDef(Prop property) {
        String strVal = getAsStringDef(property);
        // if tag not present at all
        if(strVal == null) return null;
        try {
            return Integer.valueOf(strVal);
        } catch (NumberFormatException e) { }
        return new Integer(0);
    }
    
    public int getAsIntDef(Prop propName, int defValue) {
        int val = defValue;
        try {
            val = getAsIntegerDef(propName).intValue();
        } catch (Exception e) {
        }
        return val;
    }

	public float getAsFloatDef(Prop propName, float defValue) {
        float val = defValue;
        try {
            val = Float.valueOf(getAsStringDef(propName));
        } catch (Exception e) {
        }
        return val;
	}
	
	/**
     * Sets the specified property to the given value in the displaystring
     * @param property Property to be set
     * @param value 
     */
    public void set(Prop property, String value) {
        setTagArg(property.tag, property.pos, value);
        fireDisplayStringChanged();
    }

    public void set(Prop property, int value) {
        setTagArg(property.tag, property.pos, String.valueOf(value));
        fireDisplayStringChanged();
    }
    
    // helper functions for setting and getting the location and size properties
	/**
	 * Converts the provided value (in pixel) to unit
	 * @param pixel 
	 * @return Value in units
	 */
	public float pixel2unit(int pixel) {
		return  pixel / 2.0f;
	}

	/**
	 * Converts the provided value (in unit) to pixel
	 * @param unit
	 * @return
	 */
	public int unit2pixel(float unit) {
		return (int)(unit * 2);
	}
	
    public Point getLocation() {
        Float x = getAsFloat(Prop.X);
        Float y = getAsFloat(Prop.Y);
        // if it's unspecified in any direction we should return a NULL constraint
        if (x == null || y == null)
            return null;
        
        return new Point (unit2pixel(x), unit2pixel(y));
    }

    public void setLocation(Point location) {
        
        // if location is not specified, remove the constraint from the display string
        if (location == null) {
            set(Prop.X, null);
            set(Prop.Y, null);
        } else { 
            set(Prop.X, String.valueOf(pixel2unit(location.x)));
            set(Prop.Y, String.valueOf(pixel2unit(location.y)));
        }
    }

    public Dimension getSize() {
        int width = unit2pixel(getAsFloatDef(DisplayString.Prop.WIDTH, -1.0f));
        width = width > 0 ? width : -1; 
        int height = unit2pixel(getAsFloatDef(DisplayString.Prop.HEIGHT, -1.0f));
        height = height > 0 ? height : -1; 
        
        return new Dimension(width, height);
    }

    public void setSize(Dimension size) {
        // if the size is unspecified, remove the size constraint from the model
        if (size == null || size.width < 0 ) 
            set(DisplayString.Prop.WIDTH, null);
        else
            set(DisplayString.Prop.WIDTH, String.valueOf(pixel2unit(size.width)));

        // if the size is unspecified, remove the size constraint from the model
        if (size == null || size.height < 0) 
            set(DisplayString.Prop.HEIGHT, null);
        else
            set(DisplayString.Prop.HEIGHT, String.valueOf(pixel2unit(size.height)));
    }
    
    private void fireDisplayStringChanged() {
    	if (owner != null)
    		owner.displayStringChanged();
    }
}
