package org.omnetpp.common.displaymodel;

import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;

public interface IDisplayString {

	String ATT_DISPLAYSTRING = "DisplayString"; // end of TagArg enum definition
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

    /**
     * Defines all tag groups
     *
     */
    public enum PropGroup {
        Base, Position, Polygon, Icon, Range, Text, Connection, Line, BackgroundLayout, Background, BackgroundPosition, BackgroundStyle
    }

    /**
     * describes ALL possible tag values and arguments and adds additional info,
     * including type, tag-name, position inside the tag and
     * user readable name and description
     */
    public enum Prop {
    	// PROPERTY representing the whole display string in a single line
    	DISPLAY(null, 0, PropType.STRING, PropGroup.Base, "display", "Display properties of the object"),
        // SUBMODULE / SIMPLEMODULE properties
        // do not change the first and last element of the property group
        X(Tag.p, 0, PropType.UNIT , PropGroup.Position , "x", "X position of the module"),
        Y(Tag.p, 1, PropType.UNIT, PropGroup.Position, "y", "Y position of the module"),
        LAYOUT(Tag.p, 2, PropType.STRING, PropGroup.Position, "vector layout", "Arrangement of submodule vectors. (row,r / column,col,c / matrix,m / ring,ri / exact,e,x)"),
        LAYOUT_PAR1(Tag.p, 3, PropType.STRING, PropGroup.Position, "vector layout par1", "1st layout parameter"),
        LAYOUT_PAR2(Tag.p, 4, PropType.STRING, PropGroup.Position, "vector layout par2", "2nd layout parameter"),
        LAYOUT_PAR3(Tag.p, 5, PropType.STRING, PropGroup.Position, "vector layout par3", "3rd layout parameter"),
        // B tag
        WIDTH(Tag.b, 0, PropType.UNIT, PropGroup.Polygon, "width", "Width of object. Default: match the object height, or the icon width (-1)"),
        HEIGHT(Tag.b, 1, PropType.UNIT, PropGroup.Polygon, "height", "Height of object. Default: match the object width, or the icon height (-1)"),
        // SHAPE(Tag.b, 2, PropType.STRING, PropGroup.Polygon, "shape", "Shape of object (rect / rect2 / rrect / oval / tri / tri2 / hex / hex2). Default: rect"),
        // TODO enumerated types should have their own constructor taking a string array of possible values (or other enum)
        SHAPE(Tag.b, 2, PropType.STRING, PropGroup.Polygon, "shape", "Shape of object (rect / oval). Default: rect"),

        // former O tag
        FILLCOL(Tag.b, 3, PropType.COLOR, PropGroup.Polygon, "fill color", "Fill color of the object (colorname or #RRGGBB or @HHSSBB). Default: light blue"),
        BORDERCOL(Tag.b, 4, PropType.COLOR, PropGroup.Polygon, "border color", "Border color of the object (colorname or #RRGGBB or @HHSSBB). Default: black"),
        BORDERWIDTH(Tag.b, 5, PropType.INTEGER, PropGroup.Polygon, "border width", "Border width of the object. Default: 2"),
        // I tag
        IMAGE(Tag.i, 0, PropType.IMAGE, PropGroup.Icon, "icon", "An icon representing the object"),
        IMAGECOLOR(Tag.i, 1, PropType.COLOR, PropGroup.Icon, "icon color", "A color to colorize the icon (colorname or #RRGGBB or @HHSSBB)."),
        IMAGECOLORPCT(Tag.i, 2, PropType.INTEGER, PropGroup.Icon, "icon colorization %", "Amount of colorization in percent. Default: 30"),
        // IS tag
        IMAGESIZE(Tag.is, 0, PropType.STRING, PropGroup.Icon, "icon size", "The size of the image (vs / s / l / vl)"),
        // I2 tag
        OVIMAGE(Tag.i2, 0, PropType.IMAGE, PropGroup.Icon, "overlay icon", "An icon added to the upper right corner of the original image"),
        OVIMAGECOLOR(Tag.i2, 1, PropType.COLOR, PropGroup.Icon, "overlay icon color", "A color to colorize the overlay icon (colorname or #RRGGBB or @HHSSBB)."),
        OVIMAGECOLORPCT(Tag.i2, 2, PropType.INTEGER, PropGroup.Icon, "overlay icon colorization %", "Amount of colorization in percent. Default: 30"),
        // R tag
        RANGE(Tag.r, 0, PropType.UNIT, PropGroup.Range, "range", "Radius of the range indicator"),
        RANGEFILLCOL(Tag.r, 1, PropType.COLOR, PropGroup.Range, "range fill color", "Fill color of the range indicator (colorname or #RRGGBB or @HHSSBB)."),
        RANGEBORDERCOL(Tag.r, 2, PropType.COLOR, PropGroup.Range, "range border color", "Border color of the range indicator (colorname or #RRGGBB or @HHSSBB). Default: black"),
        RANGEBORDERWIDTH(Tag.r, 3, PropType.INTEGER, PropGroup.Range, "range border width", "Border width of the range indicator. Default: 1"),
        // Q tag
        QUEUE(Tag.q, 0, PropType.STRING, PropGroup.Text, "queue object", "Displays the length of the named queue object"),
        // T tag
        TEXT(Tag.t, 0, PropType.STRING, PropGroup.Text, "text", "Additional text to display"),
        TEXTPOS(Tag.t, 1, PropType.STRING, PropGroup.Text, "text position", "Position of the text (l / r / t). Default: t"),
        TEXTCOLOR(Tag.t, 2, PropType.COLOR, PropGroup.Text, "text color", "Color of the displayed text (colorname or #RRGGBB or @HHSSBB). Default: blue"),
        // TT tag
        TOOLTIP(Tag.tt, 0, PropType.STRING, PropGroup.Text, "tooltip", "Tooltip to be displayed over the object"),
        // END of SUB/SIMPLE MODULE properties

        // START of COMPOUNDMODULE properties
        // BGP tag
        // do not change the first and last element of the property group
        MODULE_X(Tag.bgp, 0, PropType.UNIT, PropGroup.BackgroundPosition, "bg x", "Module background horizontal offset"),
        MODULE_Y(Tag.bgp, 1, PropType.UNIT, PropGroup.BackgroundPosition, "bg y", "Module background vertical offset"),
        // BGB tag
        MODULE_WIDTH(Tag.bgb, 0, PropType.UNIT, PropGroup.BackgroundPosition, "bg width", "Width of the module background rectangle. Default: match contents (-1)"),
        MODULE_HEIGHT(Tag.bgb, 1, PropType.UNIT, PropGroup.BackgroundPosition, "bg height", "Height of the module background rectangle. Default: match contents (-1)"),
        MODULE_FILLCOL(Tag.bgb, 2, PropType.COLOR, PropGroup.BackgroundStyle, "bg fill color", "Background fill color (colorname or #RRGGBB or @HHSSBB). Default: grey75"),
        MODULE_BORDERCOL(Tag.bgb, 3, PropType.COLOR, PropGroup.BackgroundStyle, "bg border color", "Border color of the module background rectangle (colorname or #RRGGBB or @HHSSBB). Default: black"),
        MODULE_BORDERWIDTH(Tag.bgb, 4, PropType.INTEGER, PropGroup.BackgroundStyle, "bg border width", "Border width of the module background rectangle. Default: 2"),
        // BGTT tag
        MODULE_TOOLTIP(Tag.bgtt, 0, PropType.STRING, PropGroup.Background, "bg tooltip", "Tooltip to be displayed over the module's background"),
        // BGI tag
        MODULE_IMAGE(Tag.bgi, 0, PropType.IMAGE, PropGroup.Background, "bg image", "An image to be displayed as a module background"),
        MODULE_IMAGEARRANGEMENT(Tag.bgi, 1, PropType.STRING, PropGroup.Background, "bg image mode", "How to arrange the module's background image (fix, tile, stretch, center). Default: fix"),
        // BGG tag
        MODULE_TICKDISTANCE(Tag.bgg, 0, PropType.UNIT, PropGroup.Background, "grid tick distance", "Distance between two major ticks measured in units"),
        MODULE_TICKNUMBER(Tag.bgg, 1, PropType.INTEGER, PropGroup.Background, "grid minor ticks", "Minor ticks per major ticks. Default: 1"),
        MODULE_GRIDCOL(Tag.bgg, 2, PropType.COLOR, PropGroup.Background, "grid color", "Color of the grid lines (colorname or #RRGGBB or @HHSSBB). Default: grey50"),
        // module layouting (how to layout the freemoving submodules)
        MODULE_LAYOUT_SEED(Tag.bgl, 0, PropType.INTEGER, PropGroup.BackgroundLayout, "layout seed","Seed value for layout algorithm"),
        MODULE_LAYOUT_ALGORITHM(Tag.bgl, 1, PropType.STRING, PropGroup.BackgroundLayout, "layout algorithm","Algorithm for child layouting. Default: spring"),
        // module scaling mixel per unit
        MODULE_SCALE(Tag.bgs, 0, PropType.UNIT, PropGroup.Background, "pixels per unit", "Number of pixels per distance unit. Coordinates are measured in units. Default: 1"),
        MODULE_UNIT(Tag.bgs, 1, PropType.STRING, PropGroup.Background, "unit name", "Name of distance unit"),
        // END of COMPOUNDMODULE properties

        // START of CONNECTION properties
        // do not change the first and last element of the property group
        // ROUTING_MODE(Tag.m, 0, PropType.STRING, PropGroup.Connection, "routing", "Routing mode ([m]anual, manhatta[n], [s]hortestpath) Default: manual"),
        // ROUTING_CONSTRAINT(Tag.m, 1, PropType.STRING, PropGroup.Connection, "routing constraint", "possible constraints: ([s]outh, [n]orth, [e]ast, [w]est)"),
        // a tag (anchoring)
        // ANCHOR_SRCX(Tag.a, 0, PropType.INTEGER, PropGroup.Position, "source anchor x", "Relative horizontal position of the anchor on the source module side"),
        // ANCHOR_SRCY(Tag.a, 1, PropType.INTEGER, PropGroup.Position, "source anchor y", "Relative vertical position of the anchor on the source module side"),
        // ANCHOR_DSTX(Tag.a, 2, PropType.INTEGER, PropGroup.Position, "destination anchor x", "Relative horizontal position of the anchor on the destination module side"),
        // ANCHOR_DSTY(Tag.a, 3, PropType.INTEGER, PropGroup.Position, "destination anchor y", "Relative vertical position of the anchor on the destination module side"),
        // ls tag (line styling)
        CONNECTION_COL(Tag.ls, 0, PropType.COLOR, PropGroup.Line, "line color", "Connection color (colorname or #RRGGBB or @HHSSBB). Default: black"),
        CONNECTION_WIDTH(Tag.ls, 1, PropType.INTEGER, PropGroup.Line, "line width", "Connection line width. Default: 1"),
        CONNECTION_STYLE(Tag.ls, 2, PropType.STRING, PropGroup.Line, "line style", "Connection line style ([s]olid, [d]otted, [da]shed). Default: solid");
        // CONNECTION_SEGMENTS(Tag.ls, 3, PropType.STRING, PropGroup.Line, "segments", "Connection segments ([l]ine, [s]pline). Default: line"),
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
    }


	/**
	 * @param property The requested property
	 * @return The value of the property. If the property is empty looks for default values and fallback
	 * values in ancestor types.
	 */
	public String getAsStringDef(Prop property);

	/**
	 * @param property The requested property
	 * @return The value of the property. If the property is empty looks for default values and fallback
	 * values in ancestor types.
	 */
	public Integer getAsIntegerDef(Prop property);

	/**
	 * @param property The requested property
	 * @return The value of the property. If the property is empty looks for default values and fallback
	 * values in ancestor types. If nothing found, returns <code>defValue</code>.
	 */
	public int getAsIntDef(Prop propName, int defValue);

	/**
	 * @param property The requested property
	 * @return The value of the property. If the property is empty looks for default values and fallback
	 * values in ancestor types. If nothing found, returns <code>defValue</code>.
	 */
	public float getAsFloatDef(Prop propName, float defValue);

	/**
	 * @return The scaling to be applied on the element. It is coming from the displaystring
	 * for the CompoundModule and coming from the container's displaystring for submodules
	 */
	public float getScale();

	/**
	 * Converts a unit based location to pixel based
     * @param scale A scaling parameter to convert to and from unit. If it's NULL the local scaling
     *              factor stored in the display string will be used
	 * @return The location property (in pixel)
	 */
	public Point getLocation(Float scale);

	/**
	 * @return The size of submodule (in pixels)
	 */
	public Dimension getSize(Float scale);

	/**
	 * @return The size of compound module (in pixels)
	 */
	public Dimension getCompoundSize(Float scale);

	/**
     * Returns the range converted to pixels
     * @return
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
	 * @return
	 */
	public int unit2pixel(float unit, Float overrideScale);
}