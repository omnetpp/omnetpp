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
        Base, Position, Style, Image, Range, Text, Connection, Misc, Background
    }

    /**
     * describes ALL possible tag values and arguments and adds additional info, 
     * including type, tagname, position inside the tag and
     * user readable name and description
     */
    public enum Prop {
    	// PROPERTY representing the whole display string in a single line
    	DISPLAY(null, 0, PropType.STRING, PropGroup.Base, "display", "Display properties of the object"),
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
        SHAPE(Tag.b, 2, PropType.STRING, PropGroup.Style, "shape", "Shape of object (rect / rect2 / rrect / oval / tri / tri2 / hex / hex2). Default: rect"),
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
        MODULE_IMAGE(Tag.bgi, 0, PropType.IMAGE, PropGroup.Background, "background image", "An image to be displayed as a module background"),
        MODULE_IMAGEARRANGEMENT(Tag.bgi, 1, PropType.STRING, PropGroup.Background, "background arrangement", "How to arrange the module's background image (fix, tile, stretch, center) Default: fix"),
        // BGG tag
        MODULE_TICKDISTANCE(Tag.bgg, 0, PropType.UNIT, PropGroup.Background, "background grid tick distance", "Distance between two major ticks measured in units"),
        MODULE_TICKNUMBER(Tag.bgg, 1, PropType.INTEGER, PropGroup.Background, "background grid tick number", "Number of minor ticks between two major one"),
        MODULE_GRIDCOL(Tag.bgg, 3, PropType.COLOR, PropGroup.Background, "background grid color", "Color of grid"),
        // module layouting (how to layout the freemoving submodules)
        MODULE_LAYOUT_SEED(Tag.bgl, 0, PropType.INTEGER, PropGroup.Misc, "layout seed","Seed value for layout algorithm"),
        MODULE_LAYOUT_ALGORITHM(Tag.bgl, 1, PropType.STRING, PropGroup.Misc, "layout algorithm","Algorithm for child layouting. Default: spring"),
        // module scaling mixel per unit
        MODULE_SCALE(Tag.bgs, 0, PropType.UNIT, PropGroup.Background, "background scaling", "Number of pixels per unit"),
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
        CONNECTION_STYLE(Tag.ls, 2, PropType.STRING, PropGroup.Style, "line style", "Connection line style ([s]olid, [d]otted, [da]shed). Default: solid"),
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
	 * @return The location property (in pixel)
	 */
	public Point getLocation();

	/**
	 * @return The size of submodule (in pixels)
	 */
	public Dimension getSize();

	/**
	 * @return The size of compound module (in pixels)
	 */
	public Dimension getCompoundSize();

	/**
     * Returns the range converted to pixels
     * @return
     */
    public int getRange();

    /**
	 * Converts the provided value (in pixel) to unit
	 * @param pixel 
	 * @return Value in units
	 */
	public float pixel2unit(int pixel);
	
	/**
	 * Converts the provided value (in unit) to pixel
	 * @param unit
	 * @return
	 */
	public int unit2pixel(float unit);
}