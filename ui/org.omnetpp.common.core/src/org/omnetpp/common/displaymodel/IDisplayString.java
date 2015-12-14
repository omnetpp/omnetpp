/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.displaymodel;

import java.util.ArrayList;
import java.util.LinkedHashMap;
import java.util.Map;

import org.apache.commons.lang3.ArrayUtils;
import org.eclipse.core.runtime.Assert;
import org.omnetpp.common.util.StringUtils;

/**
 * Declares the supported display string tags, with types, descriptions,
 * categorization, default values, and so on.
 *
 * @author rhornig, andras
 */
//TODO figures and Tkenv should use EnumSpec as well!
public interface IDisplayString {

    /**
     *  Property types used in the display string
     */
    public enum PropType { STRING, INTEGER, DISTANCE, COORDINATE, COLOR, IMAGE }

    /**
     *  Tag types
     */
    public final static int COMPOUNDMODULE = 1;
    public final static int SUBMODULE = 2;
    public final static int CONNECTION = 4;

    /**
     * Tag names used in the display string
     */
    public enum Tag {
        bgp(COMPOUNDMODULE, "Background position"),
        bgb(COMPOUNDMODULE, "Background box"),
        bgi(COMPOUNDMODULE, "Background image"),
        bgtt(COMPOUNDMODULE, "Background tooltip"),
        bgg(COMPOUNDMODULE, "Background grid"),
        bgl(COMPOUNDMODULE, "Layout parameters"),
        bgu(COMPOUNDMODULE, "Distance unit"),

        p(COMPOUNDMODULE | SUBMODULE, "Position"),
        b(COMPOUNDMODULE | SUBMODULE, "box"),
        i(COMPOUNDMODULE | SUBMODULE, "Icon"),
        is(COMPOUNDMODULE | SUBMODULE, "Icon size"),
        i2(COMPOUNDMODULE | SUBMODULE, "Status icon"),
        r(COMPOUNDMODULE | SUBMODULE, "Radius"),
        q(COMPOUNDMODULE | SUBMODULE, "Queue"),
        t(COMPOUNDMODULE | SUBMODULE | CONNECTION, "Text"),
        tt(COMPOUNDMODULE | SUBMODULE | CONNECTION, "Tooltip"),

        m(CONNECTION, "Routing mode"),
        //TODO: bp(CONNECTION, "Bend points");
        //TODO: a(CONNECTION, "Anchor"),
        ls(CONNECTION, "Line style");

        private final int type;
        private final String description;

        Tag(int type, String description)
        {
            this.type = type;
            this.description = description;
        }

        public int getType() {
            return type;
        }

        public String getDescription() {
            return description;
        }
    }

    // default value used if the tag exists but the property contains no value
    public static final String EMPTY_DEFAULTS_STR = "i=,,30;i2=,,30;b=40,24,rect,#8080ff,black,2;t=,t,blue;r=,,black,1;bgb=,,grey82,black,2;bgg=,1,grey;bgi=,fixed;ls=black,1,solid;bgu=m";

    // example values used if the property contains a $variable
    public static final String VARIABLE_DEFAULTS_STR = "i=abstract/server,red,50;i2=status/execute,red,50;b=40,24,rect,#8080ff,black,2;t=Sample text,t,blue;r=100,white,black,3;bgb=300,200,white,black,5;bgg=100,2,black;bgi=,center;ls=green,3,dashed;bgu=km";

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
                specs.put(spec.name, spec);
            }
            specsReversed = specs.values().toArray(new Item[]{});
            ArrayUtils.reverse(specsReversed);

            // sanity checks
            for (Item spec : specs.values()) {
                Assert.isTrue(spec.name.equals(getNameFor(spec.name)), "enum name must map to itself");
                Assert.isTrue(spec.name.equals(getNameFor(spec.shorthand)), "enum shorthand must map to itself");
            }
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
     * Describes supported display string tags and arguments, and adds additional info
     * including type, tag name, position inside the tag, and user readable name and description
     */
    public enum Prop {
        // Property that represents the whole display string
        DISPLAY(null, 0, PropType.STRING, PropGroup.Base, "display", "Display properties of the object", null),

        // START submodule / simple module properties
        // NOTE: Do NOT change the first and last elements of property groups!
        // P tag
        X(Tag.p, 0, PropType.COORDINATE , PropGroup.Position , "x", "X position of the center of the icon/shape; defaults to automatic graph layouting", null),
        Y(Tag.p, 1, PropType.COORDINATE, PropGroup.Position, "y", "Y position of the center of the icon/shape; defaults to automatic graph layouting", null),
        LAYOUT(Tag.p, 2, PropType.STRING, PropGroup.Position, "arrangement", "Arrangement of submodule vectors. Defaults to row or ring. (X,Y) coordinates correspond to the upper left corner of the bounding rectangle", "row=r.*,r; column=c.*,c; matrix=m.*,m; ring=ri.*,ri; exact=e.*|x.*,x"),
        LAYOUT_PAR1(Tag.p, 3, PropType.STRING, PropGroup.Position, "arr. par1", "Depends on arrangement: matrix => ncols, ring => rx, exact => dx, row => dx, column => dy", null),
        LAYOUT_PAR2(Tag.p, 4, PropType.STRING, PropGroup.Position, "arr. par2", "Depends on arrangement: matrix => dx, ring => ry, exact => dy", null),
        LAYOUT_PAR3(Tag.p, 5, PropType.STRING, PropGroup.Position, "arr. par3", "Depends on arrangement: matrix => dy", null),

        // B tag and former O tag
        SHAPE_WIDTH(Tag.b, 0, PropType.DISTANCE, PropGroup.Polygon, "width", "Width of object. Default: match the object height, or the icon width", null),
        SHAPE_HEIGHT(Tag.b, 1, PropType.DISTANCE, PropGroup.Polygon, "height", "Height of object. Default: match the object width, or the icon height", null),
        SHAPE(Tag.b, 2, PropType.STRING, PropGroup.Polygon, "shape", "Shape of object", "rectangle=r.*,rect; oval=o.*,oval"),
        SHAPE_FILL_COLOR(Tag.b, 3, PropType.COLOR, PropGroup.Polygon, "fill color", "Fill color of the object (color name, #RRGGBB or @HHSSBB)", null),
        SHAPE_BORDER_COLOR(Tag.b, 4, PropType.COLOR, PropGroup.Polygon, "border color", "Border color of the object (color name, #RRGGBB or @HHSSBB)", null),
        SHAPE_BORDER_WIDTH(Tag.b, 5, PropType.INTEGER, PropGroup.Polygon, "border width", "Border width of the object", null),

        // I tag
        IMAGE(Tag.i, 0, PropType.IMAGE, PropGroup.Icon, "icon", "An icon representing the object", null),
        IMAGE_COLOR(Tag.i, 1, PropType.COLOR, PropGroup.Icon, "icon tint", "A color for tinting the icon (color name, #RRGGBB or @HHSSBB)", null),
        IMAGE_COLOR_PERCENTAGE(Tag.i, 2, PropType.INTEGER, PropGroup.Icon, "icon tint %", "Amount of tinting in percent", null),

        // IS tag
        IMAGE_SIZE(Tag.is, 0, PropType.STRING, PropGroup.Icon, "icon size", "The size of the image", "very small=v.*s.*,vs; small=s.*,s; normal=n.*,n; large=l.*,l; very large=v[^s]*l.*,vl"),

        // I2 tag
        IMAGE2(Tag.i2, 0, PropType.IMAGE, PropGroup.Icon, "overlay icon", "An icon added to the upper right corner of the original image", null),
        IMAGE2_COLOR(Tag.i2, 1, PropType.COLOR, PropGroup.Icon, "overlay icon tint", "A color for tinting the overlay icon (color name, #RRGGBB or @HHSSBB)", null),
        IMAGE2_COLOR_PERCENTAGE(Tag.i2, 2, PropType.INTEGER, PropGroup.Icon, "overlay icon tint %", "Amount of tinting in percent", null),

        // R tag
        RANGE(Tag.r, 0, PropType.DISTANCE, PropGroup.Range, "range", "Radius of the range indicator", null),
        RANGE_FILL_COLOR(Tag.r, 1, PropType.COLOR, PropGroup.Range, "range fill color", "Fill color of the range indicator (color name, #RRGGBB or @HHSSBB)", null),
        RANGE_BORDER_COLOR(Tag.r, 2, PropType.COLOR, PropGroup.Range, "range border color", "Border color of the range indicator (color name, #RRGGBB or @HHSSBB)", null),
        RANGE_BORDER_WIDTH(Tag.r, 3, PropType.INTEGER, PropGroup.Range, "range border width", "Border width of the range indicator", null),

        // Q tag
        QUEUE_NAME(Tag.q, 0, PropType.STRING, PropGroup.Text, "queue object", "Displays the length of the named queue object", null),

        // T tag
        TEXT(Tag.t, 0, PropType.STRING, PropGroup.Text, "text", "Additional text to display", null),
        TEXT_POS(Tag.t, 1, PropType.STRING, PropGroup.Text, "text position", "Position of the text", "left=l.*,l; right=r.*,r; top=t.*,t"),
        TEXT_COLOR(Tag.t, 2, PropType.COLOR, PropGroup.Text, "text color", "Color of the displayed text (color name, #RRGGBB or @HHSSBB)", null),

        // TT tag
        TOOLTIP(Tag.tt, 0, PropType.STRING, PropGroup.Text, "tooltip", "Tooltip to be displayed over the object", null),
        // END of submodule / simple module properties

        // START of compound module properties
        // NOTE: Do NOT change the first and last elements of property groups!
        // BGP tag
        MODULE_X(Tag.bgp, 0, PropType.COORDINATE, PropGroup.BackgroundPosition, "bg x", "Module background horizontal offset", null),
        MODULE_Y(Tag.bgp, 1, PropType.COORDINATE, PropGroup.BackgroundPosition, "bg y", "Module background vertical offset", null),

        // BGB tag
        MODULE_WIDTH(Tag.bgb, 0, PropType.DISTANCE, PropGroup.BackgroundPosition, "bg width", "Width of the module background rectangle. Default: match the contents", null),
        MODULE_HEIGHT(Tag.bgb, 1, PropType.DISTANCE, PropGroup.BackgroundPosition, "bg height", "Height of the module background rectangle. Default: match the contents", null),
        MODULE_FILL_COLOR(Tag.bgb, 2, PropType.COLOR, PropGroup.BackgroundStyle, "bg fill color", "Background fill color (color name, #RRGGBB or @HHSSBB)", null),
        MODULE_BORDER_COLOR(Tag.bgb, 3, PropType.COLOR, PropGroup.BackgroundStyle, "bg border color", "Border color of the module background rectangle (color name, #RRGGBB or @HHSSBB)", null),
        MODULE_BORDER_WIDTH(Tag.bgb, 4, PropType.INTEGER, PropGroup.BackgroundStyle, "bg border width", "Border width of the module background rectangle", null),

        // BGTT tag
        MODULE_TOOLTIP(Tag.bgtt, 0, PropType.STRING, PropGroup.Background, "bg tooltip", "Tooltip to be displayed over the module's background", null),

        // BGI tag
        MODULE_IMAGE(Tag.bgi, 0, PropType.IMAGE, PropGroup.Background, "bg image", "An image to be displayed as a module background", null),
        MODULE_IMAGE_ARRANGEMENT(Tag.bgi, 1, PropType.STRING, PropGroup.Background, "bg image mode", "How to arrange the module's background image", "fix=f.*,f; tile=t.*,t; stretch=s.*,s; center=c.*,c"),

        // BGG tag
        MODULE_GRID_DISTANCE(Tag.bgg, 0, PropType.DISTANCE, PropGroup.Background, "grid distance", "Distance between two major gridlines, in units", null),
        MODULE_GRID_SUBDIVISION(Tag.bgg, 1, PropType.INTEGER, PropGroup.Background, "grid subdivision", "Minor gridlines per major gridlines", null),
        MODULE_GRID_COLOR(Tag.bgg, 2, PropType.COLOR, PropGroup.Background, "grid color", "Color of the grid lines (color name, #RRGGBB or @HHSSBB)", null),

        // BGL tag (compound module layouting)
        MODULE_LAYOUT_SEED(Tag.bgl, 0, PropType.INTEGER, PropGroup.BackgroundLayout, "layout seed","Seed value for layout algorithm", null),
        MODULE_LAYOUT_ALGORITHM(Tag.bgl, 1, PropType.STRING, PropGroup.BackgroundLayout, "layout algorithm","Algorithm for child layouting", null),

        // BGU tag (distance unit)
        MODULE_UNIT(Tag.bgu, 0, PropType.STRING, PropGroup.Background, "distance unit", "Name of distance unit", null),
        // END of compound module properties

        // START of connection properties
        // NOTE: Do NOT change the first and last elements of property groups!
        // ROUTING_MODE(Tag.m, xxx, PropType.STRING, PropGroup.Connection, "routing", "Routing mode", "todo: [m]anual, manhatta[n], [s]hortestpath"),
        ROUTING_CONSTRAINT(Tag.m, 0, PropType.STRING, PropGroup.Connection, "routing constraint", "Connection routing constraint", "auto=a.*,a; south=s.*,s; north=n.*,n; east=e.*,e; west=w.*,w"),

        // A tag (anchoring)
        // ANCHOR_SRCX(Tag.a, 0, PropType.INTEGER, PropGroup.Position, "source anchor x", "Relative horizontal position of the anchor on the source module side"),
        // ANCHOR_SRCY(Tag.a, 1, PropType.INTEGER, PropGroup.Position, "source anchor y", "Relative vertical position of the anchor on the source module side"),
        // ANCHOR_DSTX(Tag.a, 2, PropType.INTEGER, PropGroup.Position, "destination anchor x", "Relative horizontal position of the anchor on the destination module side"),
        // ANCHOR_DSTY(Tag.a, 3, PropType.INTEGER, PropGroup.Position, "destination anchor y", "Relative vertical position of the anchor on the destination module side"),

        // LS tag (line style)
        CONNECTION_COLOR(Tag.ls, 0, PropType.COLOR, PropGroup.Line, "line color", "Connection color (color name, #RRGGBB or @HHSSBB)", null),
        CONNECTION_WIDTH(Tag.ls, 1, PropType.INTEGER, PropGroup.Line, "line width", "Connection line width", null),
        CONNECTION_STYLE(Tag.ls, 2, PropType.STRING, PropGroup.Line, "line style", "Connection line style", "solid=s.*,s; dotted=d.*,d; dashed=da.*,da");
        // CONNECTION_SEGMENTS(Tag.ls, 3, PropType.STRING, PropGroup.Line, "segments", "Connection segments", "todo: [l]ine, [s]pline"),

        // BP tag (bendpoints)
        // BENDPOINTS(Tag.bp, 0, PropType.UNIT, PropGroup.Connection, "bendpoints", "bendpoint locations");

        // END of connection properties

        // define additional metadata for the tags
        private final Tag tag;
        private final int pos;
        private final PropType type;
        private final PropGroup group;
        private final String name;
        private final String description;
        private final EnumSpec enumSpec;

        Prop(Tag tag, int tagPos, PropType argType, PropGroup tagGroup,
                String name, String description, String enumSpecString) {
            Assert.isTrue(!description.trim().endsWith("."));  // dot will be added by this class where needed
            this.tag = tag;
            this.pos = tagPos;
            this.type = argType;
            this.group = tagGroup;
            this.name = name;
            this.description = description;
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

        public String getName() {
            return name;
        }

        public String getDesc() {
            return description;
        }

        /**
         * Returns the first sentence of the description
         */
        public String getShortDesc() {
            return description == null ? null : StringUtils.substringBefore(description, ".");
        }

        /**
         * The description of the default value
         */
        public String getDefaultDesc() {
            return getTag()==null ? null : getTagArgDefault(getTagName(), getPos());
        }

        /**
         * The possible values of the enums in this tag
         */
        public String getEnumDesc() {
            String enumDesc = "";
            if (getEnumSpec() != null)
                for (String name : getEnumSpec().getNames())
                    enumDesc += (enumDesc.equals("") ? "" : ", ") + name + " (" + getEnumSpec().getShorthandForName(name) + ")";
            return enumDesc;
        }

        public static Prop findProp(Tag tag, int pos) {
            for (Prop prop : Prop.values())
                if (prop.tag != null && prop.tag.equals(tag) && prop.pos == pos)
                    return prop;
            return null;
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
     * Returns true if the property is specified in the display string or
     * in the fallback chain (except the EMPTY_DEFAULTS).
     */
    public boolean containsProperty(Prop prop);

    /**
     * Returns true if the tag is specified in the display string or in the
     * fallback chain (except the EMPTY_DEFAULTS).
     */
    public boolean containsTag(Tag tagName);

    /**
     * The value of the property. If the property is empty looks for default values and fallback
     * values in ancestor types. If nothing found, returns <code>null</code>.
     */
    public String getAsString(Prop property);

    /**
     * The value of the property. If the property is empty looks for default values and fallback
     * values in ancestor types. If nothing found, returns defaultValue.
     */
    public int getAsInt(Prop propName, int defaultValue);

    /**
     * The value of the property, which must be of type DISTANCE. The result is guaranteed
     * to be positive, or NaN. If the property is empty, it looks for default values and
     * fallback values in ancestor types. If nothing is found, returns NaN.
     */
    public float getAsDistance(Prop propName);

    /**
     * The value of the property, which must be of type COORDINATE. If the property is empty,
     * it looks for default values and fallback values in ancestor types. If nothing is found,
     * returns NaN.
     */
    public float getAsCoordinate(Prop propName);

    /**
     * Returns the location for the module ("p" tag). Uses fallback if not specified locally.
     * If either location property is missing, it returns null (meaning the module is unpinned).
     */
    public PointF getLocation();

    /**
     * Returns the size of submodule ("b" tag). Uses fallback if not specified locally.
     * Fallback rules apply if either or both dimensions are missing.
     */
    public DimensionF getSize();

    /**
     * Returns the size of module if represented as a compound module ("bgb" tag).
     * Uses fallback if the property is not specified locally.
     * Missing width or height values are represented by NaN.
     */
    public DimensionF getCompoundSize();

    /**
     * Returns the radius of the 1st transmission range indicator ("r" tag). Uses fallback
     * if the property is not specified locally. Returns NaN if no range was specified.
     */
    public float getRange();

    /**
     * Sets the content of the display string.
     */
    public void set(String newValue);

    /**
     * Compute a hash code based on this display string and all fallback display strings.
     */
    public int cumulativeHashCode();
}
