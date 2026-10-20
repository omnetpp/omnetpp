package org.omnetpp.ned.editor.graph.properties;

import java.util.EnumSet;
import java.util.List;

import org.apache.commons.lang3.ArrayUtils;
import org.apache.commons.lang3.StringUtils;
import org.eclipse.core.resources.IProject;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.jface.viewers.ICellEditorValidator;
import org.eclipse.swt.graphics.FontData;
import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.PropertyDescriptor;
import org.eclipse.ui.views.properties.TextPropertyDescriptor;
import org.omnetpp.common.properties.CheckboxPropertyDescriptor;
import org.omnetpp.common.properties.ColorPropertyDescriptor;
import org.omnetpp.common.properties.EnumComboboxPropertyDescriptor;
import org.omnetpp.common.properties.FontPropertyDescriptor;
import org.omnetpp.common.properties.ImagePropertyDescriptor;
import org.omnetpp.common.properties.NumberPropertyDescriptor;
import org.omnetpp.common.util.Converter;
import org.omnetpp.common.util.EnumSpec;
import org.omnetpp.figures.misc.TransformDescription;
import org.omnetpp.ned.core.NedCanvasFigureValidator;
import org.omnetpp.ned.editor.graph.misc.CanvasFigureUtils;
import org.omnetpp.ned.editor.graph.properties.util.AnchorPropertyDescriptor;
import org.omnetpp.ned.editor.graph.properties.util.NedBasePropertySource;
import org.omnetpp.ned.editor.graph.properties.util.PathPropertyDescriptor;
import org.omnetpp.ned.editor.graph.properties.util.PointListPropertyDescriptor;
import org.omnetpp.ned.editor.graph.properties.util.TransformPropertyDescriptor;
import org.omnetpp.ned.model.ex.PropertyElementEx;

import static org.omnetpp.common.canvas.CanvasFigureConstants.*;

// XXX consider using MergedPropertySource, with lots of small composing sources
public class CanvasFigurePropertySource extends NedBasePropertySource {

    // the bounding rectangle properties are delegated to this where applicable
    private AnchoredRectanglePropertySource rectSource;

    // just a cache, since it is queried much more frequently than updated
    private IPropertyDescriptor[] descriptors;

    private boolean readOnly = false;

    public CanvasFigurePropertySource(PropertyElementEx model) {
        super(model);
        rectSource = new AnchoredRectanglePropertySource(model, "Location and size");

        updateDescriptors();
    }

    @Override
    protected PropertyElementEx getModel() {
        return (PropertyElementEx)super.getModel();
    }

    protected IProject getProject() {
        return getModel().getSelfOrEnclosingTypeElement().getNedTypeInfo().getProject();
    }

    private static EnumSpec typeSpec = new EnumSpec(
            "line=^li.*,line; polyline=^polyl.*,polyline; polygon=^polyg.*,polygon; rectangle=^r.*,rectangle;"
            + "oval=^o.*,oval; pieslice=^pi.*,pieslice; arc=^a.*,arc; group=^g.*,group; text=^t.*,text;"
            + "label=^la.*,label; path=^pa.*,path; image=^i.*,image");

    private static EnumSpec arrowHeadSpec = new EnumSpec("none=^n.*,none; simple=^s.*,simple; triangle=^t.*,triangle; barbed=^b.*,barbed");
    private static EnumSpec lineStyleSpec = new EnumSpec("solid=^s.*,solid; dashed=^da.*,dashed; dotted=^do.*,dotted");
    private static EnumSpec joinStyleSpec = new EnumSpec("bevel=^b.*,bevel; miter=^m.*,miter; round=^r.*,round");
    private static EnumSpec capStyleSpec = new EnumSpec("butt=^b.*,butt; square=^s.*,square; round=^r.*,round");
    private static EnumSpec fillRuleSpec = new EnumSpec("nonzero=^n.*,nonzero; evenodd=^e.*,evenodd");
    private static EnumSpec interpRuleSpec = new EnumSpec("none=^n.*,none; fast=^f.*,fast; best=^b.*,best");
    //private static EnumSpec alignmentSpec = new EnumSpec("left=^l.*,left; center=^c.*,center; right=^r.*,right");

    private enum PropType { Enum, Anchor, Number, Color, String, Boolean, Image, Custom }

    private enum Prop {
        Index(PropType.Custom, "index", "Hierarchical name", "Common",
            "The hierarchical name of the figure, separated by periods. Also the index of the property element in the source."),

        Type(PropType.Custom, PKEY_TYPE, "type", "Common",
            "Defines the type of the figure. For example: rectangle, polyline, pieslice, etc...", typeSpec),

        ChildZ(PropType.Number, PKEY_ZINDEX, "zIndex", "Common",
            "The drawing order of figures within a parent can be altered with this. Figures with higher childZ value are drawn on top of those with lower childZ."),
        IsVisible(PropType.Boolean, (Boolean)true, PKEY_VISIBLE, "visible", "Common",
            "If set to false, the figure (and its children) are not drawn, and cannot be selected with clicking, only with the rectangle selection."),

        Transform(PropType.Custom, PKEY_TRANSFORM, "transform", "Common", "The affine transformation of the figure."),

        // these are only used for the text type figures, the bounds of rectangles and such are delegated to rectSource
        PositionX(PropType.Number, PKEY_POS, 0, "x", "Location and Size",
            "The X coordinate of the figure's anchored point."),
        PositionY(PropType.Number, PKEY_POS, 1, "y", "Location and Size",
            "The Y coordinate of the figure's anchored point."),
        AnchorPointX(PropType.Number, PKEY_ANCHORPOINT, 0, "anchor point x", "Location and Size",
            "The X coordinate of the point of the panel's coordinate system that is placed at the position."),
        AnchorPointY(PropType.Number, PKEY_ANCHORPOINT, 1, "anchor point y", "Location and Size",
            "The Y coordinate of the point of the panel's coordinate system that is placed at the position."),
        Anchor(PropType.Anchor, PKEY_ANCHOR, "anchor", "Location and Size",
            "Specifies which point of the defining rectangle is positioned with the x and y properties."),

        InnerWidth(PropType.Number, PKEY_INNERSIZE, 0, "inner width", "Location and Size",
            "The width of the ring's inner ellipse."),
        InnerHeight(PropType.Number, PKEY_INNERSIZE, 1, "inner height", "Location and Size",
            "The height of the ring's inner ellipse"),

        Image(PropType.Image, PKEY_IMAGE, "image", "Style", "The image to be displayed."),
        Interpolation(PropType.Enum, PKEY_INTERPOLATION, "interpolation", "Style", "The interpolation method to be used.", interpRuleSpec),
        TintColor(PropType.Color, PKEY_TINT, 0, "tint color", "Style", "The color of the image tint."),
        TintAmount(PropType.Number, PKEY_TINT, 1, "tint amount", "Style", "The amount of tinting to apply (0: none, 1:full)."),


        // XXX once the PathPropertyDescriptor and PathDialog are complete, change the type of this to Custom
        Path(PropType.String, PKEY_PATH, "path", "Style", "The path to be drawn."),
        OffsetX(PropType.Number, PKEY_OFFSET, 0, "x offset", "Location and Size",
            "The X offset to be applied to the path coordinates"),
        OffsetY(PropType.Number, PKEY_OFFSET, 1, "y offset", "Location and Size",
            "The Y offset to be applied to the path coordinates."),

        Point1X(PropType.Number, PKEY_POINTS, 0, "start x", "Points",
            "The X coordinate of the starting point of the line."),
        Point1Y(PropType.Number, PKEY_POINTS, 1, "start y", "Points",
            "The Y coordinate of the starting point of the line."),
        Point2X(PropType.Number, PKEY_POINTS, 2, "end x", "Points",
            "The X coordinate of the ending point of the line."),
        Point2Y(PropType.Number, PKEY_POINTS, 3, "end y", "Points",
            "The Y coordinate of the ending point of the line."),

        PointList(PropType.Custom, PKEY_POINTS, "Points"),

        LineWidth(PropType.Number, PKEY_LINEWIDTH, "line width", "Style",
            "The width of the figure's outline."),
        ZoomLineWidth(PropType.Boolean, (Boolean)false, PKEY_ZOOMLINEWIDTH, "zoom line width", "Style",
            "Whether or not zooming affects the width of the line on the screen."),
        ClipChildren(PropType.Boolean, (Boolean)false, PKEY_CLIPCHILDREN, "clip children", "Common",
            "If set to true, the child figures are clipped to the interior of this shape."),
        LineColor(PropType.Color, PKEY_LINECOLOR, "line color", "Style",
            "The color of the figure's outline."),
        LineStyle(PropType.Enum, PKEY_LINESTYLE, "line style", "Style",
            "The dash pattern of the figure's outline.", lineStyleSpec),
        CapStyle(PropType.Enum, PKEY_CAPSTYLE, "cap style", "Style",
            "The style with which the line is terminated.", capStyleSpec),
        JoinStyle(PropType.Enum, PKEY_JOINSTYLE, "join style", "Style",
            "The style with which the line segments are joined.", joinStyleSpec),
        FillRule(PropType.Enum, PKEY_FILLRULE, "fill rule", "Style",
                "The rule that specifies how overlapping parts are filled.", fillRuleSpec),
        FillColor(PropType.Color, PKEY_FILLCOLOR, "fill color", "Style",
            "The fill color of the figure."),
        Smooth(PropType.Boolean, (Boolean)false, PKEY_SMOOTH, "smooth", "Style",
            "If true, the figure's line is smoothed using quadratic Bézier segments."),

        Opacity(PropType.Number, PKEY_OPACITY, 0, 1, "opacity", "Style", "The opacity of the figure. 0 is fully transparent, 1 is fully opaque."),
        FillOpacity(PropType.Number, PKEY_FILLOPACITY, 0, 1, "fill opacity", "Style", "The opacity of the fill of the figure. 0 is fully transparent, 1 is fully opaque."),
        LineOpacity(PropType.Number, PKEY_LINEOPACITY, 0, 1, "line opacity", "Style", "The opacity of the outline of the figure. 0 is fully transparent, 1 is fully opaque."),

        StartArrowHead(PropType.Enum, PKEY_STARTARROWHEAD, "start", "Arrowheads",
            "The style of the line's start.", arrowHeadSpec),
        EndArrowHead(PropType.Enum, PKEY_ENDARROWHEAD, "end", "Arrowheads",
            "The style of the line's end.", arrowHeadSpec),

        StartAngle(PropType.Number, PKEY_STARTANGLE, "start", "Angles",
            "The starting angle of the figure's arc. In degrees, the positive direction is CCW, 0° is to the right."),
        EndAngle(PropType.Number, PKEY_ENDANGLE, "end", "Angles",
            "The ending angle of the figure's arc. In degrees, the positive direction is CCW, 0° is to the right."),

        Text(PropType.String, PKEY_TEXT, "text", "Text",
            "The string which is displayed by the figure."),

        //Align(PropType.Enum, PKEY_ALIGNMENT, "alignment", "Style",
        //    "The alignment of the text's lines. Only meaningful with multiple lines.", alignmentSpec),

        Font(PropType.Custom, PKEY_FONT, "font", "Style",
            "The typeface, style, and point size of the text."),
        Underline(PropType.Custom, "underline", "underline", "Style",
            "Whether or not the text is underlined."),
        TextColor(PropType.Color, PKEY_COLOR, "color", "Style",
            "The color of the text.");

        private PropType type;
        private String name; // displayed in the PropertySheet
        private String key; // used in the NED source in the property KeyElement list
        private int index = 0; // which Literal Element is this under the given Key
        private String category;
        private String description;
        private EnumSpec enumSpec;
        private Object defaultValue;

        private double minNumber = -Double.MAX_VALUE; // used only for Number type
        private double maxNumber = Double.MAX_VALUE;

        private Prop(PropType type) {
            this(type, null);
        }

        private Prop(PropType type, String category) {
            this(type, null, category);
        }

        private Prop(PropType type, String name, String category) {
            this(type, null, name, category, null);
        }

        private Prop(PropType type, String key, String name, String category, String description) {
            this(type, key, 0, name, category, description, null);
        }

        private Prop(PropType type, Object defaultValue, String key, String name, String category, String description) {
            this(type, defaultValue, key, 0, name, category, description, null);
        }

        private Prop(PropType type, String key, String name, String category, String description, EnumSpec enumSpec) {
            this(type, key, 0, name, category, description, enumSpec);
        }

        private Prop(PropType type, String key, double min, double max, String name, String category, String description) {
            this(type, key, 0, name, category, description);
            minNumber = min;
            maxNumber = max;
        }

        private Prop(PropType type, String key, int index, String name, String category, String description) {
            this(type, key, index, name, category, description, null);
        }

        private Prop(PropType type, String key, int index, String name, String category, String description, EnumSpec enumSpec) {
            this(type, null, key, index, name, category, description, enumSpec);
        }

        private Prop(PropType type, Object defaultValue, String key, int index, String name, String category, String description, EnumSpec enumSpec) {
            this.type = type;
            this.defaultValue = defaultValue;
            this.key = key;
            this.index = index;
            this.name = name;
            this.category = category;
            this.description = description;
            this.enumSpec = enumSpec;
        }
    }


    private static EnumSet<Prop> commonFigureProperties = EnumSet.of(Prop.Index, Prop.Type, Prop.ChildZ, Prop.Transform, Prop.IsVisible);
    private static EnumSet<Prop> commonLineProperties = EnumSet.of(Prop.LineWidth, Prop.ZoomLineWidth, Prop.LineColor, Prop.LineOpacity, Prop.LineStyle, Prop.CapStyle);
    private static EnumSet<Prop> commonShapeProperties = EnumSet.of(Prop.LineWidth, Prop.ZoomLineWidth, Prop.LineColor, Prop.FillColor, Prop.FillOpacity, Prop.LineOpacity, Prop.LineStyle, Prop.ClipChildren);
    private static EnumSet<Prop> arrowHeadProperties = EnumSet.of(Prop.StartArrowHead, Prop.EndArrowHead);
    private static EnumSet<Prop> angleProperties = EnumSet.of(Prop.StartAngle, Prop.EndAngle);


    private IPropertyDescriptor[] createPropertyDescriptors(Prop prop) {
        PropertyDescriptor desc = null;

        switch (prop.type) {
        case Boolean:
            desc = new CheckboxPropertyDescriptor(prop, prop.name);
            break;
        case Color:
            desc = new ColorPropertyDescriptor(prop, prop.name);
            break;
        case Enum:
            desc = new EnumComboboxPropertyDescriptor(prop, prop.name, prop.enumSpec);
            break;
        case Anchor:
            desc = new AnchorPropertyDescriptor(prop, prop.name);
            break;
        case Number:
            desc = new NumberPropertyDescriptor(prop, prop.name, prop.minNumber, prop.maxNumber);
            break;
        case String:
            desc = new TextPropertyDescriptor(prop, prop.name);
            break;
        case Image:
            desc = new ImagePropertyDescriptor(prop, prop.name, getProject());
            break;

        case Custom:

            switch (prop) {
            case Type:
                desc = new PropertyDescriptor(prop, prop.name);
                break;
            case Index:
                desc = new TextPropertyDescriptor(prop, prop.name);
                desc.setValidator(new ICellEditorValidator() {
                    @Override
                    public String isValid(Object value) {
                        String string = (String)value;

                        if (string.isEmpty()) {
                            return "The name must not be empty!";
                        }

                        for (char c: string.toCharArray()) {
                            int type = Character.getType(c);

                            if (!StringUtils.contains("*?{}:.-", c)
                                    && (type != Character.LOWERCASE_LETTER)
                                    && (type != Character.UPPERCASE_LETTER)
                                    && (type != Character.DECIMAL_DIGIT_NUMBER)) {
                                return "The given name contains an invalid character: \"" + c + "\".";
                            }
                        }
                        return null;
                    }
                });

                break;
            case Transform:
                desc = new TransformPropertyDescriptor(prop, prop.name);
                break;
            case Underline:
                desc = new CheckboxPropertyDescriptor(prop, prop.name);
                break;
            case Font:
                desc = new FontPropertyDescriptor(prop, prop.name);
                break;
            case Path:
                desc = new PathPropertyDescriptor(prop, prop.name);
                break;
            case PointList:
                desc = new PointListPropertyDescriptor(prop, prop.name);
                break;
            default:
                break;
            }

            break;
        }

        if (desc == null) {
            return null;
        } else {
            desc.setCategory(prop.category);
            desc.setDescription(prop.description);

            return new IPropertyDescriptor[] { desc };
        }
    }

    private void updateDescriptors() {
        EnumSet<Prop> supportedProperties = EnumSet.noneOf(Prop.class);

        supportedProperties.addAll(commonFigureProperties);

        String type = getModel().getValue(PKEY_TYPE);

        if (type != null) {
            if (type.equals(FTYPE_LINE)) {
                supportedProperties.addAll(commonLineProperties);
                supportedProperties.addAll(arrowHeadProperties);
                supportedProperties.add(Prop.Point1X);
                supportedProperties.add(Prop.Point1Y);
                supportedProperties.add(Prop.Point2X);
                supportedProperties.add(Prop.Point2Y);
            } else if (type.equals(FTYPE_POLYLINE)) {
                supportedProperties.addAll(commonLineProperties);
                supportedProperties.add(Prop.Smooth);
                supportedProperties.add(Prop.JoinStyle);
                supportedProperties.addAll(arrowHeadProperties);
                supportedProperties.add(Prop.PointList);
            } else if (type.equals(FTYPE_POLYGON)) {
                supportedProperties.addAll(commonShapeProperties);
                supportedProperties.add(Prop.Smooth);
                supportedProperties.add(Prop.JoinStyle);
                supportedProperties.add(Prop.FillRule);
                supportedProperties.add(Prop.PointList);
            } else if (type.equals(FTYPE_RECTANGLE)) {
                supportedProperties.addAll(commonShapeProperties);
                supportedProperties.add(Prop.JoinStyle);
            } else if (type.equals(FTYPE_OVAL)) {
                supportedProperties.addAll(commonShapeProperties);
            } else if (type.equals(FTYPE_RING)) {
                supportedProperties.addAll(commonShapeProperties);
                supportedProperties.add(Prop.InnerWidth);
                supportedProperties.add(Prop.InnerHeight);
            } else if (type.equals(FTYPE_PIESLICE)) {
                supportedProperties.addAll(commonShapeProperties);
                supportedProperties.addAll(angleProperties);
                supportedProperties.add(Prop.JoinStyle);
            } else if (type.equals(FTYPE_ARC)) {
                supportedProperties.addAll(commonLineProperties);
                supportedProperties.addAll(angleProperties);
                supportedProperties.addAll(arrowHeadProperties);
            } else if (type.equals(FTYPE_GROUP)) {
                // A group doesn't have anything except the common figure parameters.
            } else if (type.equals(FTYPE_PANEL)) {
                supportedProperties.add(Prop.PositionX);
                supportedProperties.add(Prop.PositionY);
                supportedProperties.add(Prop.AnchorPointX);
                supportedProperties.add(Prop.AnchorPointY);
            } else if (type.equals(FTYPE_TEXT) || type.equals(FTYPE_LABEL)) { // these are essentially the same
                supportedProperties.add(Prop.Text);
                supportedProperties.add(Prop.Font);
                supportedProperties.add(Prop.Underline);
                supportedProperties.add(Prop.TextColor);
                supportedProperties.add(Prop.Opacity);
                supportedProperties.add(Prop.PositionX);
                supportedProperties.add(Prop.PositionY);
                supportedProperties.add(Prop.Anchor);
            } else if (type.equals(FTYPE_IMAGE) || type.equals(FTYPE_ICON)) {
                supportedProperties.add(Prop.Image);
                supportedProperties.add(Prop.Interpolation);
                supportedProperties.add(Prop.Opacity);
                supportedProperties.add(Prop.TintColor);
                supportedProperties.add(Prop.TintAmount);
            } else if (type.equals(FTYPE_PIXMAP)) {
                supportedProperties.add(Prop.FillColor);
                supportedProperties.add(Prop.Interpolation);
                supportedProperties.add(Prop.Opacity);
                supportedProperties.add(Prop.TintColor);
                supportedProperties.add(Prop.TintAmount);
            } else if (type.equals(FTYPE_PATH)) {
                supportedProperties.addAll(commonShapeProperties);
                supportedProperties.add(Prop.Path);
                supportedProperties.add(Prop.OffsetX);
                supportedProperties.add(Prop.OffsetY);
                supportedProperties.add(Prop.FillRule);
            }
        }

        descriptors = new IPropertyDescriptor[0];

        for (Prop prop : supportedProperties) {
            descriptors = ArrayUtils.addAll(descriptors, createPropertyDescriptors(prop));
        }

        if (NedCanvasFigureValidator.getTypeHasBounds(type))
            descriptors = ArrayUtils.addAll(descriptors, rectSource.getPropertyDescriptors());
    }

    public void setReadOnly(boolean readOnly) {
        this.readOnly = readOnly;
    }

    @Override
    public IPropertyDescriptor[] getPropertyDescriptors() {
        if ((descriptors == null) || (descriptors.length == 0)) {
            updateDescriptors();
        }

        if (readOnly) {
            IPropertyDescriptor[] readOnlyDescriptors = new IPropertyDescriptor[descriptors.length];
            for (int i = 0; i < descriptors.length; i++) {
                IPropertyDescriptor pdesc = descriptors[i];
                if (pdesc.getClass() != PropertyDescriptor.class) {
                    PropertyDescriptor readOnlyPDesc = new PropertyDescriptor(pdesc.getId(), pdesc.getDisplayName());
                    readOnlyPDesc.setCategory(pdesc.getCategory());
                    readOnlyPDesc.setDescription(pdesc.getDescription() + " - (read only)");
                    readOnlyDescriptors[i] = readOnlyPDesc;
                } else {
                    readOnlyDescriptors[i] = pdesc;
                }
            }
            return readOnlyDescriptors;
        }

        return descriptors;
    }

    @Override
    public Object getPropertyValue(Object id) {
        if (id instanceof Prop) {
            Prop prop = (Prop)id;

            List<String> values = (prop.key == null) ? null : getModel().getValueAsList(prop.key);

            switch (prop.type) {
            case Boolean:
                return ((values != null) && (values.size() > prop.index)) ?
                        Boolean.parseBoolean(values.get(prop.index)) :
                        prop.defaultValue;
            case Number:
                return ((values != null) && (values.size() > prop.index)) ?
                        Converter.stringToOptionalDouble(values.get(prop.index)) :
                        null;
            case Color:
            case Enum:
            case String:
            case Image:
                return ((values != null) && (values.size() > prop.index)) ?
                        values.get(prop.index) :
                        null;
            case Anchor:
                return ((values != null) && (values.size() > prop.index)) ?
                        AnchorPropertyDescriptor.getAnchor(values.get(prop.index)) :
                        null;
            case Custom:

                switch (prop) {
                case Type:
                    return getModel().getValue("type");
                case Index:
                    return getModel().getIndex();
                case Transform:
                    return CanvasFigureUtils.parseTransforms(getModel(), PKEY_TRANSFORM);
                case Underline:
                    return CanvasFigureUtils.getFontDataFlag(
                            CanvasFigureUtils.parseFont(getModel(), PKEY_FONT), CanvasFigureUtils.UNDERLINE);
                case Font:
                    return CanvasFigureUtils.parseFont(getModel(), PKEY_FONT);
                case Path:
                    return getModel().getValue(PKEY_PATH);
                case PointList:
                    return CanvasFigureUtils.parsePointList(getModel(), PKEY_POINTS);
                default:
                    return null;
                }

            default:
                return null;
            }
        } else {
            return rectSource.getPropertyValue(id);
        }
    }

    @Override
    @SuppressWarnings("unchecked")
    public void setPropertyValue(Object id, Object value) {
        if (readOnly)
            return;
        if (id instanceof Prop) {
            Prop prop = (Prop)id;

            switch (prop.type) {
            case Boolean:
                getModel().setValue(prop.key, prop.index, Converter.booleanToString((Boolean)value));
                break;
            case Enum:
                getModel().setValue(prop.key, prop.index, prop.enumSpec.getShorthandFor((String)value));
                break;
            case Anchor:
                getModel().setValue(prop.key, prop.index, AnchorPropertyDescriptor.getAnchor((String)value).toString());
                break;
            case Number:
                getModel().setValue(prop.key, prop.index, Converter.doubleToString((Double)value));
                break;
            case Color:
            case String:
            case Image:
                getModel().setValue(prop.key, prop.index, (String)value);
                break;

            case Custom:

                switch (prop) {
                case Type: // this is never actually used, because the type descriptor doesn't create a cell editor
                    getModel().setValue("type", (String)value);
                    break;
                case Index:
                    getModel().setIndex((String)value);
                    break;
                case Transform:
                    CanvasFigureUtils.setTransforms(getModel(), PKEY_TRANSFORM, (List<TransformDescription>)value);
                    break;
                case Underline:
                    FontData fontData = CanvasFigureUtils.parseFont(getModel(), PKEY_FONT);
                    CanvasFigureUtils.setFontDataFlag(fontData, CanvasFigureUtils.UNDERLINE, (Boolean)value);
                    CanvasFigureUtils.setFont(getModel(), PKEY_FONT, fontData);
                    break;
                case Font:
                    CanvasFigureUtils.setFont(getModel(), PKEY_FONT, (FontData)value);
                    break;
                case Path:
                    getModel().setValue(PKEY_PATH, (String)value);
                    break;
                case PointList:
                    CanvasFigureUtils.setPointList(getModel(), PKEY_POINTS, (List<Point>)value);
                    break;
                default:
                    break;
                }

                break;

            default:
                break;
            }

            // this is no longer effectively needed, since type is read-only
            if (prop == Prop.Type) { // different type figures have different properties
                updateDescriptors();
                CanvasFigureUtils.removeInvalidParameters(getModel());
            }
        } else {
            rectSource.setPropertyValue(id, value);
        }
    }
}
