/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting.properties;

import org.eclipse.draw2d.Graphics;
import org.omnetpp.common.properties.ColorPropertyDescriptor;
import org.omnetpp.common.properties.PropertySource;
import org.omnetpp.scave.ScaveImages;

public class LineVisualProperties {

    public static final String
        PROP_DISPLAY_NAME       = "Line.Name",
        PROP_DISPLAY_LINE       = "Line.Display",
        PROP_SYMBOL_TYPE        = "Symbols.Type",
        PROP_SYMBOL_SIZE        = "Symbols.Size",
        PROP_DRAW_STYLE          = "Line.DrawStyle",
        PROP_LINE_COLOR         = "Line.Color",
        PROP_LINE_STYLE         = "Line.Style",
        PROP_LINE_WIDTH         = "Line.Width";


    public enum SymbolType {
        None("None", ScaveImages.IMG_OBJ16_SYM_NONE),
        Point("Point", ScaveImages.IMG_OBJ16_SYM_POINT),
        Cross("Cross", ScaveImages.IMG_OBJ16_SYM_CROSS),
        Diamond("Diamond", ScaveImages.IMG_OBJ16_SYM_DIAMOND),
        ThinDiamond("thin_diamond", ScaveImages.IMG_OBJ16_SYM_DIAMOND),
        Dot("Dot", ScaveImages.IMG_OBJ16_SYM_DOT),
        Plus("Plus", ScaveImages.IMG_OBJ16_SYM_PLUS),
        HLine("HLine", ScaveImages.IMG_OBJ16_SYM_PLUS),
        VLine("VLine", ScaveImages.IMG_OBJ16_SYM_PLUS),
        Octagon("Octagon", ScaveImages.IMG_OBJ16_SYM_SQUARE),
        Square("Square", ScaveImages.IMG_OBJ16_SYM_SQUARE),
        Pentagon("Pentagon", ScaveImages.IMG_OBJ16_SYM_SQUARE),
        Star("Star", ScaveImages.IMG_OBJ16_SYM_SQUARE),
        Triangle_Up("triangle_up", ScaveImages.IMG_OBJ16_SYM_TRIANGLE),
        Triangle_Down("triangle_down", ScaveImages.IMG_OBJ16_SYM_TRIANGLE),
        Triangle_Left("triangle_left", ScaveImages.IMG_OBJ16_SYM_TRIANGLE),
        Triangle_Right("triangle_right", ScaveImages.IMG_OBJ16_SYM_TRIANGLE),
        Tri_Up("tri_up", ScaveImages.IMG_OBJ16_SYM_TRIANGLE),
        Tri_Down("tri_down", ScaveImages.IMG_OBJ16_SYM_TRIANGLE),
        Tri_Left("tri_left", ScaveImages.IMG_OBJ16_SYM_TRIANGLE),
        Tri_Right("tri_right", ScaveImages.IMG_OBJ16_SYM_TRIANGLE);

        private String name;
        private String imageId;

        private SymbolType(String name, String img) {
            this.name = name;
            imageId = img;
        }

        @Override
        public String toString() {
            return name;
        }

        public String getImageId() {
            return imageId;
        }
    }

    public enum DrawStyle {
        None("none", ScaveImages.IMG_OBJ16_LINE_NONE),
        Linear("linear", ScaveImages.IMG_OBJ16_LINE_LINEAR),
        Pins("pins", ScaveImages.IMG_OBJ16_LINE_PINS),
        StepsPost("steps-post", ScaveImages.IMG_OBJ16_LINE_STEPS_POST),
        StepsPre("steps-pre", ScaveImages.IMG_OBJ16_LINE_STEPS_PRE),
        StepsMid("steps-mid", ScaveImages.IMG_OBJ16_LINE_STEPS_PRE);

        private String name;
        private String imageId;

        private DrawStyle(String name, String img) {
            this.name = name;
            this.imageId = img;
        }

        @Override
        public String toString() {
            return name;
        }

        public String getImageId() {
            return imageId;
        }
    }

    public enum LineStyle {
        None("none", Graphics.LINE_SOLID /*doesn't matter*/, null),
        Solid("solid", Graphics.LINE_SOLID, null),
        Dotted("dotted", Graphics.LINE_DOT, null),
        Dashed("dashed", Graphics.LINE_DASH, null),
        DashDot("dashdot", Graphics.LINE_DASHDOT, null);

        private String name;
        private int draw2dConstant;
        private String imageId;

        private LineStyle(String name, int draw2dConstant, String img) {
            this.name = name;
            this.draw2dConstant = draw2dConstant;
            this.imageId = img;
        }

        @Override
        public String toString() {
            return name;
        }

        public int getDraw2DConstant() {
            return draw2dConstant;
        }

        public String getImageId() {
            return imageId;
        }

    }
}