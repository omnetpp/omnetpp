package org.omnetpp.common.displaymodel;

import java.util.ArrayList;
import java.util.LinkedHashMap;
import java.util.Map;

import org.apache.commons.lang.ArrayUtils;
import org.eclipse.core.runtime.Assert;
import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;
import org.omnetpp.common.util.StringUtils;

/**
 * FIXME undocumented class
 *
 * @author rhornig
 */
public interface IDisplayString {

	/**
     *	Property types used in the display string
     */
    public enum PropType { READONLY, STRING, INTEGER, UNIT, COLOR, IMAGE }

    /**
     * Tag names used in the display string
     */
    public enum Tag { p, b, i, is, i2, r, q, t, tt,  // submodule tags
                      bgp, bgb, bgi, bgtt, bgg, bgl, bgs, // compound module background tags
                      m, a, ls, bp }                 // connection tags

    // default value used if the tag exists but the property contains no value
    public static final String EMPTY_DEFAULTS_STR = "i=,,30;i2=,,30;b=40,24,rect,#8080ff,black,2;t=,t,blue;r=,,black,1;bgb=,,grey82,black,2;bgg=,1,grey;bgi=,fixed;ls=black,1,solid";

    // example values used if the property contains a $variable
    public static final String VARIABLE_DEFAULTS_STR = "i=abstract/server,red,50;i2=status/execute,red,50;b=40,24,rect,#8080ff,black,2;t=Sample text,t,blue;r=100,white,black,3;bgb=300,200,white,black,5;bgg=100,2,black;bgi=,center;ls=green,3,dashed";

    /**
     * Defines all tag groups
     */
    public enum PropGroup {
        Base, Position, Polygon, Icon, Range, Text, Connection, Line, BackgroundLayout, Background, BackgroundPosition, BackgroundStyle
    }
    
    /**
     * Utility class for handling enumerated display string values, like "dashed/dotted/solid"
     * or "left/right/top". A value needs to be recognized in the display string ("das" ==> "dashed"),
     * combo box selection has to be offered (with options "dashed", "dotted" and "solid"),
     * and the value has to be written back into the display string in a short form ("dashed" as "da").
     * This class facilitates the above tasks.
     * 
     * @author andras
     */
    public class EnumSpec {
    	private static class Item {
    		String name; // full name ("dashed")
    		String shorthandRegex;  // for recognizing the value ("da.*")
    		String shorthand; // for representing the value in the display string ("da")
    	}
    	private Map<String,Item> specs = new LinkedHashMap<String, Item>();
    	private Item[] specsReversed;
    	
    	/**
    	 * Format of the specification string: "name=shorthandRegex,shorthand;...".
    	 * Spaces are allowed. The order of items is significant, because shorthand regexes 
    	 * will be matched in REVERSE order (last-to-first).
    	 * 
    	 * Example: "solid=s.*,s; dotted=d.*,d; dashed=da.*,da". Note that "d" will be 
    	 * recognized as "dotted", because of reverse-order matching. 
    	 */
    	public EnumSpec(String specString) {
        	for (String specText : specString.split(";")) {
        		Assert.isTrue(specText.matches("^[^=,]+=[^=,]+,[^=,]*$"), "enum spec is in wrong format");
        		Item spec = new Item();
        		spec.name = StringUtils.substringBefore(specText, "=").trim();
        		spec.shorthandRegex = StringUtils.substringBetween(specText, "=", ",").trim();
        		spec.shorthand = StringUtils.substringAfter(specText, ",").trim();
        		Assert.isTrue(spec.name.matches(spec.shorthandRegex), "enum name must match its own regex");
        		Assert.isTrue(spec.shorthand.matches(spec.shorthandRegex), "enum shorthand must match its own regex");
        		specs.put(spec.name, spec);
        	}
        	specsReversed = specs.values().toArray(new Item[]{});
        	ArrayUtils.reverse(specsReversed);
    	}
    	
    	/**
    	 * Returns an array of all names ("dotted", "dashed", etc).
    	 */
    	public String[] getNames() {
    		return specs.keySet().toArray(new String[]{});
    	}

    	/**
		 * Returns the list of all shorthands (standard abbreviations)
		 */
    	public String[] getShorthands() {
			ArrayList<String> result = new ArrayList<String>();
			for (Item spec : specs.values())
				result.add(spec.shorthand);
			return result.toArray(new String[]{});
		}

    	/**
    	 * Return the name whose shorthandRegex matches the given string,
    	 * or null if none matches. Note: matching is done in reverse order.
    	 */
    	public String getNameFor(String text) {
    		for (Item spec : specsReversed)
    			if (text.matches(spec.shorthandRegex))
    				return spec.name;
    		return null;
    	}

    	/**
    	 * Return the shorthand (standard abbreviation) whose shorthandRegex 
    	 * matches the given string, or null if none matches. Note: matching 
    	 * is done in reverse order.
    	 */
    	public String getShorthandFor(String text) {
    		for (Item spec : specsReversed)
    			if (text.matches(spec.shorthandRegex))
    				return spec.shorthand;
    		return null;
    	}

    	/**
    	 * Returns the shorthand (standard abbreviation) for the given name, 
    	 * e.g. returns "da" for "dashed". It is an error if the name does not exist.
    	 */
    	public String getShorthandForName(String name) {
    		Assert.isTrue(specs.containsKey(name), "invalid enum value");
    		return specs.get(name).shorthand;
    	}

    }
    
    /**
     * Describes ALL possible tag values and arguments and adds additional info,
     * including type, tag-name, position inside the tag and
     * user readable name and description
     */
    public enum Prop {
    	// PROPERTY representing the whole display string in a single line
    	DISPLAY(null, 0, PropType.STRING, PropGroup.Base, "display", "Display properties of the object", null),
        // SUBMODULE / SIMPLEMODULE properties
        // do not change the first and last element of the property group
        X(Tag.p, 0, PropType.UNIT , PropGroup.Position , "x", "X position of the module", null),
        Y(Tag.p, 1, PropType.UNIT, PropGroup.Position, "y", "Y position of the module", null),
        LAYOUT(Tag.p, 2, PropType.STRING, PropGroup.Position, "vector layout", "Arrangement of submodule vectors", "row=r.*,r; column=c.*,c; matrix=m.*,m; ring=ri.*,ri; exact=e.*|x.*,x"),
        LAYOUT_PAR1(Tag.p, 3, PropType.STRING, PropGroup.Position, "vector layout par1", "1st layout parameter", null),
        LAYOUT_PAR2(Tag.p, 4, PropType.STRING, PropGroup.Position, "vector layout par2", "2nd layout parameter", null),
        LAYOUT_PAR3(Tag.p, 5, PropType.STRING, PropGroup.Position, "vector layout par3", "3rd layout parameter", null),
        // B tag
        WIDTH(Tag.b, 0, PropType.UNIT, PropGroup.Polygon, "width", "Width of object. Default: match the object height, or the icon width", null),
        HEIGHT(Tag.b, 1, PropType.UNIT, PropGroup.Polygon, "height", "Height of object. Default: match the object width, or the icon height", null),
        // SHAPE(Tag.b, 2, PropType.STRING, PropGroup.Polygon, "shape", "Shape of object", "todo: rect / rect2 / rrect / oval / tri / tri2 / hex / hex2"),
        // TODO enumerated types should have their own constructor taking a string array of possible values (or other enum)
        SHAPE(Tag.b, 2, PropType.STRING, PropGroup.Polygon, "shape", "Shape of object", "rectangle=r.*,rect; oval=o.*,oval"),

        // former O tag
        FILLCOL(Tag.b, 3, PropType.COLOR, PropGroup.Polygon, "fill color", "Fill color of the object (colorname or #RRGGBB or @HHSSBB)", null),
        BORDERCOL(Tag.b, 4, PropType.COLOR, PropGroup.Polygon, "border color", "Border color of the object (colorname or #RRGGBB or @HHSSBB)", null),
        BORDERWIDTH(Tag.b, 5, PropType.INTEGER, PropGroup.Polygon, "border width", "Border width of the object", null),
        // I tag
        IMAGE(Tag.i, 0, PropType.IMAGE, PropGroup.Icon, "icon", "An icon representing the object", null),
        IMAGECOLOR(Tag.i, 1, PropType.COLOR, PropGroup.Icon, "icon color", "A color to colorize the icon (colorname or #RRGGBB or @HHSSBB)", null),
        IMAGECOLORPERCENTAGE(Tag.i, 2, PropType.INTEGER, PropGroup.Icon, "icon colorization %", "Amount of colorization in percent", null),
        // IS tag
        IMAGESIZE(Tag.is, 0, PropType.STRING, PropGroup.Icon, "icon size", "The size of the image", "very small=v.*s.*,vs; small=s.*,s; normal=n.*,n; large=l.*,l; very large=v.*l.*,vl"),
        // I2 tag
        OVIMAGE(Tag.i2, 0, PropType.IMAGE, PropGroup.Icon, "overlay icon", "An icon added to the upper right corner of the original image", null),
        OVIMAGECOLOR(Tag.i2, 1, PropType.COLOR, PropGroup.Icon, "overlay icon color", "A color to colorize the overlay icon (colorname or #RRGGBB or @HHSSBB)", null),
        OVIMAGECOLORPCT(Tag.i2, 2, PropType.INTEGER, PropGroup.Icon, "overlay icon colorization %", "Amount of colorization in percent", null),
        // R tag
        RANGE(Tag.r, 0, PropType.UNIT, PropGroup.Range, "range", "Radius of the range indicator", null),
        RANGEFILLCOL(Tag.r, 1, PropType.COLOR, PropGroup.Range, "range fill color", "Fill color of the range indicator (colorname or #RRGGBB or @HHSSBB)", null),
        RANGEBORDERCOL(Tag.r, 2, PropType.COLOR, PropGroup.Range, "range border color", "Border color of the range indicator (colorname or #RRGGBB or @HHSSBB)", null),
        RANGEBORDERWIDTH(Tag.r, 3, PropType.INTEGER, PropGroup.Range, "range border width", "Border width of the range indicator", null),
        // Q tag
        QUEUE(Tag.q, 0, PropType.STRING, PropGroup.Text, "queue object", "Displays the length of the named queue object", null),
        // T tag
        TEXT(Tag.t, 0, PropType.STRING, PropGroup.Text, "text", "Additional text to display", null),
        TEXTPOS(Tag.t, 1, PropType.STRING, PropGroup.Text, "text position", "Position of the text", "left=l.*,l; right=r.*,r; top=t.*,t"),
        TEXTCOLOR(Tag.t, 2, PropType.COLOR, PropGroup.Text, "text color", "Color of the displayed text (colorname or #RRGGBB or @HHSSBB)", null),
        // TT tag
        TOOLTIP(Tag.tt, 0, PropType.STRING, PropGroup.Text, "tooltip", "Tooltip to be displayed over the object", null),
        // END of SUB/SIMPLE MODULE properties

        // START of COMPOUNDMODULE properties
        // BGP tag
        // do not change the first and last element of the property group
        MODULE_X(Tag.bgp, 0, PropType.UNIT, PropGroup.BackgroundPosition, "bg x", "Module background horizontal offset", null),
        MODULE_Y(Tag.bgp, 1, PropType.UNIT, PropGroup.BackgroundPosition, "bg y", "Module background vertical offset", null),
        // BGB tag
        MODULE_WIDTH(Tag.bgb, 0, PropType.UNIT, PropGroup.BackgroundPosition, "bg width", "Width of the module background rectangle. Default: match the contents", null),
        MODULE_HEIGHT(Tag.bgb, 1, PropType.UNIT, PropGroup.BackgroundPosition, "bg height", "Height of the module background rectangle. Default: match the contents", null),
        MODULE_FILLCOL(Tag.bgb, 2, PropType.COLOR, PropGroup.BackgroundStyle, "bg fill color", "Background fill color (colorname or #RRGGBB or @HHSSBB)", null),
        MODULE_BORDERCOL(Tag.bgb, 3, PropType.COLOR, PropGroup.BackgroundStyle, "bg border color", "Border color of the module background rectangle (colorname or #RRGGBB or @HHSSBB)", null),
        MODULE_BORDERWIDTH(Tag.bgb, 4, PropType.INTEGER, PropGroup.BackgroundStyle, "bg border width", "Border width of the module background rectangle", null),
        // BGTT tag
        MODULE_TOOLTIP(Tag.bgtt, 0, PropType.STRING, PropGroup.Background, "bg tooltip", "Tooltip to be displayed over the module's background", null),
        // BGI tag
        MODULE_IMAGE(Tag.bgi, 0, PropType.IMAGE, PropGroup.Background, "bg image", "An image to be displayed as a module background", null),
        MODULE_IMAGEARRANGEMENT(Tag.bgi, 1, PropType.STRING, PropGroup.Background, "bg image mode", "How to arrange the module's background image", "fix=f.*,f; tile=t.*,t; stretch=s.*,s; center=c.*,c"),
        // BGG tag
        MODULE_TICKDISTANCE(Tag.bgg, 0, PropType.UNIT, PropGroup.Background, "grid tick distance", "Distance between two major ticks measured in units", null),
        MODULE_TICKNUMBER(Tag.bgg, 1, PropType.INTEGER, PropGroup.Background, "grid minor ticks", "Minor ticks per major ticks", null),
        MODULE_GRIDCOL(Tag.bgg, 2, PropType.COLOR, PropGroup.Background, "grid color", "Color of the grid lines (colorname or #RRGGBB or @HHSSBB)", null),
        // module layouting (how to layout the free moving submodules)
        MODULE_LAYOUT_SEED(Tag.bgl, 0, PropType.INTEGER, PropGroup.BackgroundLayout, "layout seed","Seed value for layout algorithm", null),
        MODULE_LAYOUT_ALGORITHM(Tag.bgl, 1, PropType.STRING, PropGroup.BackgroundLayout, "layout algorithm","Algorithm for child layouting", null),
        // module scaling pixel per unit
        MODULE_SCALE(Tag.bgs, 0, PropType.UNIT, PropGroup.Background, "pixels per unit", "Number of pixels per distance unit. Coordinates are measured in units", null),
        MODULE_UNIT(Tag.bgs, 1, PropType.STRING, PropGroup.Background, "unit name", "Name of distance unit", null),
        // END of COMPOUNDMODULE properties

        // START of CONNECTION properties
        // do not change the first and last element of the property group
        // ROUTING_MODE(Tag.m, 0, PropType.STRING, PropGroup.Connection, "routing", "Routing mode", "todo: [m]anual, manhatta[n], [s]hortestpath"),
        // ROUTING_CONSTRAINT(Tag.m, 1, PropType.STRING, PropGroup.Connection, "Routing constraint", "Routing constraint", "todo: [s]outh, [n]orth, [e]ast, [w]est"),
        // a tag (anchoring)
        // ANCHOR_SRCX(Tag.a, 0, PropType.INTEGER, PropGroup.Position, "source anchor x", "Relative horizontal position of the anchor on the source module side"),
        // ANCHOR_SRCY(Tag.a, 1, PropType.INTEGER, PropGroup.Position, "source anchor y", "Relative vertical position of the anchor on the source module side"),
        // ANCHOR_DSTX(Tag.a, 2, PropType.INTEGER, PropGroup.Position, "destination anchor x", "Relative horizontal position of the anchor on the destination module side"),
        // ANCHOR_DSTY(Tag.a, 3, PropType.INTEGER, PropGroup.Position, "destination anchor y", "Relative vertical position of the anchor on the destination module side"),
        // ls tag (line styling)
        CONNECTION_COL(Tag.ls, 0, PropType.COLOR, PropGroup.Line, "line color", "Connection color (colorname or #RRGGBB or @HHSSBB)", null),
        CONNECTION_WIDTH(Tag.ls, 1, PropType.INTEGER, PropGroup.Line, "line width", "Connection line width", null),
        CONNECTION_STYLE(Tag.ls, 2, PropType.STRING, PropGroup.Line, "line style", "Connection line style", "solid=s.*,s; dotted=d.*,d; dashed=da.*,da");
        // CONNECTION_SEGMENTS(Tag.ls, 3, PropType.STRING, PropGroup.Line, "segments", "Connection segments", "todo: [l]ine, [s]pline"),
        // bp tag (bendpoints)
        // BENDPOINTS(Tag.bp, 0, PropType.UNIT, PropGroup.Connection, "bendpoints", "bendpoint locations");
        // END of CONNECTION properties
        // end of tag definition

        // define additional metadata for the tags
        private final Tag tag;
        private final int pos;
        private final PropType type;
        private final PropGroup group;
        private final String visibleName;
        private final String visibleDesc;
        private final EnumSpec enumSpec;

        Prop(Tag tag, int tagPos, PropType argType, PropGroup tagGroup,
                String visibleName, String visibleDesc, String enumSpecString) {
        	Assert.isTrue(!visibleDesc.trim().endsWith("."));  // dot will be added by this class where needed
            this.tag = tag;
            this.pos = tagPos;
            this.type = argType;
            this.group = tagGroup;
            this.visibleName = visibleName;
            this.visibleDesc = visibleDesc;
            this.enumSpec = enumSpecString == null ? null : new EnumSpec(enumSpecString);
        }

        public PropType getType() {
            return type;
        }

        public Tag getTag() {
            return tag;
        }

        public String getTagName() {
            return tag == null ? null : tag.toString();
        }

        public PropGroup getGroup() {
            return group;
        }

        public int getPos() {
            return pos;
        }

        public EnumSpec getEnumSpec() {
			return enumSpec;
		}
        
        public String getVisibleName() {
            return visibleName;
        }

        public String getVisibleDesc() {
        	String enumDesc = "";
        	if (getEnumSpec() != null)
        		for (String name : getEnumSpec().getNames())
        			enumDesc += (enumDesc.equals("") ? "" : ", ") + name + " (" + getEnumSpec().getShorthandForName(name) + ")"; 
            String defaultValue = getTag()==null ? null : getTagArgDefault(getTagName(), getPos());
            return visibleDesc + 
            (enumDesc.equals("") ? "" : ". Values: " + enumDesc) + 
            (StringUtils.isNotEmpty(defaultValue) ? ". Default: "+defaultValue : "");
        }
        
        private String getTagArgDefault(String tagName, int tagIndex) {
        	String displayString = IDisplayString.EMPTY_DEFAULTS_STR;
        	Assert.isTrue(StringUtils.containsNone(displayString, " \t\\"), 
        			"display string defaults cannot contain backslash or whitespace"); // to simplify parsing
        	for (String tag : displayString.split(";"))
        		if (tag.startsWith(tagName+"=")) {
        			String[] elems = StringUtils.removeStart(tag, tagName+"=").split(",");
        			return tagIndex >= elems.length ? null : elems[tagIndex]; 
        		}
        	return null;
        }
    }


    /**
     * True if the property is specified in the display string or in the fallback chain.
     * (except the EMPTY_DEFAULTS)
     */
    public boolean containsProperty(Prop prop);

    /**
     * True if the tag is specified in the display string or in the fallback chain.
     * (except the EMPTY_DEFAULTS)
     */
    public boolean containsTag(Tag tagName);

    /**
     * The value of the property. If the property is empty looks for default values and fallback
     * values in ancestor types. If nothing found, returns <code>NULL</code>.
     */
	public String getAsString(Prop property);

    /**
     * The value of the property. If the property is empty looks for default values and fallback
     * values in ancestor types. If nothing found, returns <code>defValue</code>.
     */
	public int getAsInt(Prop propName, int defValue);

	/**
	 * The value of the property. If the property is empty looks for default values and fallback
	 * values in ancestor types. If nothing found, returns <code>defValue</code>.
	 */
	public float getAsFloat(Prop propName, float defValue);

	/**
	 * The scaling to be applied on the element. Uses fallback if not specified locally.
	 * Returns 1.0 if not specified. NOTE that scaling factor is available ONLY in compound module
	 * display strings.
	 */
	public float getScale();

	/**
	 * Returns the pixel based location for the module. Converts the unit based values of P tag to pixel.
	 * Uses fallback if not specified locally. If either location property is missing it returns NULL
	 * (meaning the module is unpinned)
     * @param scale A scaling parameter to convert to and from unit. If it's NULL the local scaling
     *              factor stored in the display string will be used
	 */
	public Point getLocation(Float scale);

	/**
	 * Returns The size of submodule (B tag) (in pixels). Uses fallback if not specified locally.
     * If width or height is missing returns -1 instead.
     * @param scale A scaling parameter to convert to and from unit. If it's NULL the local scaling
     *              factor stored in the display string will be used
	 */
	public Dimension getSize(Float scale);

    /**
     * The size of module (in pixel) if represented as compound module (BGB tag).
     * Uses fallback if the property is not specified locally.
     * If width or height is missing returns -1 instead.
     * @param scale A scaling parameter to convert to and from unit. If it's NULL the local scaling
     *              factor stored in the display string will be used
     */
	public Dimension getCompoundSize(Float scale);

    /**
     * Returns the range converted to pixels. Uses fallback if the property is not specified locally.
     * Returns -1 if no range was specified.
     * @param scale A scaling parameter to convert to and from unit. If it's NULL the local scaling
     *              factor stored in the display string will be used
     */
    public int getRange(Float scale);

    /**
	 * Converts the provided value (in pixel) to unit
	 * @param pixel
     * @param overrideScale If not NULL it will be used as scaling factor instead of the stored one
	 * @return Value in units
	 */
	public float pixel2unit(int pixel, Float overrideScale);

	/**
	 * Converts the provided value (in unit) to pixel
	 * @param unit
     * @param overrideScale If not NULL it will be used as scaling factor instead of the stored one
	 * @return Value in pixels
	 */
	public int unit2pixel(float unit, Float overrideScale);

    /**
     * Sets the content of the display string.
     */
    public void set(String newValue);

}