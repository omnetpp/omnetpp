package org.omnetpp.ned.editor.graph.properties;

import java.util.ArrayList;
import java.util.LinkedHashMap;
import java.util.List;

import org.eclipse.core.runtime.IAdapterFactory;
import org.eclipse.core.runtime.Platform;
import org.eclipse.ui.views.properties.ColorPropertyDescriptor;
import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.IPropertySource;
import org.eclipse.ui.views.properties.PropertyDescriptor;
import org.eclipse.ui.views.properties.TextPropertyDescriptor;
import org.omnetpp.ned.editor.graph.model.SubmoduleNodeEx;
import org.omnetpp.ned.editor.graph.properties.DisplayPropertySource.IntDesc;

//TODO Colors cannot be edited by hand. A derived ColorCellEditor is required
//TODO Some property needs a combo box cell editor
//TODO an icon selector / editorcell must be implemented
//TODO multi line text cell editor must be implemented
//TODO implement getColorProperty/Def too so unknown colors will be displayed as default
//FIXME default handling is still not ok. Block size etc is not defaults to -1 if left empty
public class SubmoduleDisplayPropertySource extends DisplayPropertySource {
    protected static LinkedHashMap<String,IntDesc> propDescMap = new LinkedHashMap<String,IntDesc>();
    protected static IPropertyDescriptor[] propertyDescArray;

    SubmoduleNodeEx linkedModel = null;
    // contains the default fallback values for the different tags if a variable is used in that position
    private static DisplayPropertySource variableDefaults 
        = new SubmoduleDisplayPropertySource("i=,,30;i2=,,30;is=40;b=40,40,rect,#8080ff,black,2;t=,t,blue;r=100,,black,1");
    // contains the default fallback values for the different tags if it is empty
    private static DisplayPropertySource emptyDefaults 
        = new SubmoduleDisplayPropertySource("i=,,30;i2=,,30;is=40;b=-1,-1,rect,#8080ff,black,2;t=,t,blue;r=,,black,1");

    public static final String ID = "Display";
    public static final String CAT_POSITION = "Position";
    public static final String PROP_X = ID + CAT_POSITION + "X";
    public static final String PROP_Y = ID + CAT_POSITION + "Y";
    public static final String PROP_LAYOUT = ID + CAT_POSITION + "Layout";
    public static final String PROP_LAYOUT_PAR1 = PROP_LAYOUT + "Par1";
    public static final String PROP_LAYOUT_PAR2 = PROP_LAYOUT + "Par2";
    public static final String PROP_LAYOUT_PAR3 = PROP_LAYOUT + "Par3";
    
    public static final String CAT_SHAPE = "Shape";
    public static final String PROP_W = ID + CAT_SHAPE + "Width";
    public static final String PROP_H = ID + CAT_SHAPE + "Height";
    public static final String PROP_SHAPE = ID + CAT_SHAPE + "Shape";
    public static final String PROP_FILLCOL = ID + CAT_SHAPE + "FillCol";
    public static final String PROP_BORDERCOL = ID + CAT_SHAPE + "BorderCol";
    public static final String PROP_BORDERWIDTH = ID + CAT_SHAPE + "BorderWidth";

    public static final String CAT_IMAGE = "Image";
    public static final String PROP_IMAGE = ID + CAT_IMAGE + "Image";
    public static final String PROP_IMAGECOLOR = ID + CAT_IMAGE + "ImageColor";
    public static final String PROP_IMAGECOLORPCT = ID + CAT_IMAGE + "ImageColorPct";
    public static final String PROP_IMAGESIZE = ID + CAT_IMAGE + "ImageSize";
    public static final String PROP_OVIMAGE = ID + CAT_IMAGE + "OvImage";
    public static final String PROP_OVIMAGECOLOR = ID + CAT_IMAGE + "OvImageColor";
    public static final String PROP_OVIMAGECOLORPCT = ID + CAT_IMAGE + "OvImageColorPct";

    public static final String CAT_RANGE = "Range";
    public static final String PROP_RANGE = ID + CAT_RANGE + "Range";
    public static final String PROP_RANGEFILLCOL = ID + CAT_RANGE + "RangeFillCol";
    public static final String PROP_RANGEBORDERCOL = ID + CAT_RANGE + "RangeOutCol";
    public static final String PROP_RANGEBORDERWIDTH = ID + CAT_RANGE + "RangeBorderWidth";

    public static final String CAT_TEXT = "Text";
    public static final String PROP_QUEUE = ID + CAT_TEXT + "Queue";
    public static final String PROP_TEXT = ID + CAT_TEXT + "Text";
    public static final String PROP_TEXTPOS = ID + CAT_TEXT + "TextPos";
    public static final String PROP_TEXTCOLOR = ID + CAT_TEXT + "TextColor";
    public static final String PROP_TOOLTIP = ID + CAT_TEXT + "Tooltip";

    // static initializer block to init the property descriptors
    static {
        // create the internal decriptors, hashtable key is the propertyID
        // tagName tagPos propertyID name description 
        // category filters propDescriptorClass propSourceClass
        // P tag
        propDescMap.put(PROP_X, 
                new IntDesc("p", 0, PROP_X, "x", "X position of the module",
                            CAT_POSITION, null, TextPropertyDescriptor.class, null));
        propDescMap.put(PROP_Y,
                new IntDesc("p", 1, PROP_Y, "y", "Y position of the module",
                            CAT_POSITION, null, TextPropertyDescriptor.class, null));
        propDescMap.put(PROP_LAYOUT, 
                new IntDesc("p", 2, PROP_LAYOUT, "layout", "Layouting algorithm. (row,r / column,col,c / matrix,m / ring,ri / exact,e,x)",
                            CAT_POSITION, null, TextPropertyDescriptor.class, null));
        propDescMap.put(PROP_LAYOUT_PAR1,
                new IntDesc("p", 3, PROP_LAYOUT_PAR1, "layout par1", "1st layout parameter",
                            CAT_POSITION, null, TextPropertyDescriptor.class, null));
        propDescMap.put(PROP_LAYOUT_PAR2,
                new IntDesc("p", 4, PROP_LAYOUT_PAR2, "layout par2", "2nd layout parameter",
                            CAT_POSITION, null, TextPropertyDescriptor.class, null));
        propDescMap.put(PROP_LAYOUT_PAR3,
                new IntDesc("p", 5, PROP_LAYOUT_PAR3, "layout par3", "3rd layout parameter",
                            CAT_POSITION, null, TextPropertyDescriptor.class, null));
        // B tag
        propDescMap.put(PROP_W, 
                new IntDesc("b", 0, PROP_W, "shape width", "Width of shape. Default: match the image size or shape height (-1)",
                            CAT_SHAPE, null, TextPropertyDescriptor.class, null));
        propDescMap.put(PROP_H, 
                new IntDesc("b", 1, PROP_H, "shape height", "Height of shape. Default: match the image size or shape width (-1)",
                            CAT_SHAPE, null, TextPropertyDescriptor.class, null));
        propDescMap.put(PROP_SHAPE, 
                new IntDesc("b", 2, PROP_SHAPE, "shape", "Shape of object (rect / rect2 / rrect / oval / tri / tri2 / hex / hex2). Default: rect",
                            CAT_SHAPE, null, TextPropertyDescriptor.class, null));
        
        // former O tag
        propDescMap.put(PROP_FILLCOL, 
                new IntDesc("b", 3, PROP_FILLCOL, "shape fill color", "Fill color of the shape. Default: light blue",
                            CAT_SHAPE, null, ColorPropertyDescriptor.class, null));
        propDescMap.put(PROP_BORDERCOL, 
                new IntDesc("b", 4, PROP_BORDERCOL, "shape border color", "Border color of the shape. Default: black",
                            CAT_SHAPE, null, ColorPropertyDescriptor.class, null));
        propDescMap.put(PROP_BORDERWIDTH, 
                new IntDesc("b", 5, PROP_BORDERWIDTH, "shape border width", "Border width of the shape. Default: 2",
                            CAT_SHAPE, null, TextPropertyDescriptor.class, null));

        // I tag
        propDescMap.put(PROP_IMAGE, 
                new IntDesc("i", 0, PROP_IMAGE, "image", "An icon or image representing the object",
                            CAT_IMAGE, null, TextPropertyDescriptor.class, null));
        propDescMap.put(PROP_IMAGECOLOR, 
                new IntDesc("i", 1, PROP_IMAGECOLOR, "image color", "A color to colorize the image",
                            CAT_IMAGE, null, ColorPropertyDescriptor.class, null));
        propDescMap.put(PROP_IMAGECOLORPCT, 
                new IntDesc("i", 2, PROP_IMAGECOLORPCT, "image colorization", "Amount of colorization in percent",
                            CAT_IMAGE, null, TextPropertyDescriptor.class, null));
        // IS tag
        propDescMap.put(PROP_IMAGESIZE, 
                new IntDesc("is", 0, PROP_IMAGESIZE, "image size", "The size of the image (vs / s / l / vl)",
                            CAT_IMAGE, null, TextPropertyDescriptor.class, null));

        // I2 tag
        propDescMap.put(PROP_OVIMAGE, 
                new IntDesc("i2", 0, PROP_OVIMAGE, "overlay image", "An icon or image added to the upper rigt corner of the original image",
                            CAT_IMAGE, null, TextPropertyDescriptor.class, null));
        propDescMap.put(PROP_OVIMAGECOLOR, 
                new IntDesc("i2", 1, PROP_OVIMAGECOLOR, "overlay image color", "A color to colorize the overlay image",
                            CAT_IMAGE, null, ColorPropertyDescriptor.class, null));
        propDescMap.put(PROP_OVIMAGECOLORPCT, 
                new IntDesc("i2", 2, PROP_OVIMAGECOLORPCT, "overlay image colorization", "Amount of colorization in percent",
                            CAT_IMAGE, null, TextPropertyDescriptor.class, null));
        
        // R tag
        propDescMap.put(PROP_RANGE, 
                new IntDesc("r", 0, PROP_RANGE, "range", "Radius of the range indicator",
                            CAT_RANGE, null, TextPropertyDescriptor.class, null));
        propDescMap.put(PROP_RANGEFILLCOL, 
                new IntDesc("r", 1, PROP_RANGEFILLCOL, "range fill color", "Fill color of the range indicator",
                            CAT_RANGE, null, ColorPropertyDescriptor.class, null));
        propDescMap.put(PROP_RANGEBORDERCOL, 
                new IntDesc("r", 2, PROP_RANGEBORDERCOL, "range border color", "Border color of the range indicator. Default: black",
                            CAT_RANGE, null, ColorPropertyDescriptor.class, null));
        propDescMap.put(PROP_RANGEBORDERWIDTH, 
                new IntDesc("r", 3, PROP_RANGEBORDERWIDTH, "range border width", "Border width of the range indicator. Default: 1",
                            CAT_RANGE, null, TextPropertyDescriptor.class, null));
        // Q tag
        propDescMap.put(PROP_QUEUE, 
                new IntDesc("q", 0, PROP_QUEUE, "queue", "Queue name to display",
                            CAT_TEXT, null, TextPropertyDescriptor.class, null));

        // T tag
        propDescMap.put(PROP_TEXT, 
                new IntDesc("t", 0, PROP_TEXT, "text", "Additional text to display",
                            CAT_TEXT, null, TextPropertyDescriptor.class, null));
        propDescMap.put(PROP_TEXTPOS, 
                new IntDesc("t", 1, PROP_TEXTPOS, "text position", "Position of the text (l / r / t). Default: t",
                            CAT_TEXT, null, TextPropertyDescriptor.class, null));
        propDescMap.put(PROP_TEXTCOLOR, 
                new IntDesc("t", 2, PROP_TEXTCOLOR, "text color", "Color of the displayed text. Default: blue",
                            CAT_TEXT, null, ColorPropertyDescriptor.class, null));

        // TT tag
        propDescMap.put(PROP_TOOLTIP, 
                new IntDesc("tt", 0, PROP_TOOLTIP, "tooltip", "Tooltip to be displayed over the object",
                            CAT_TEXT, null, TextPropertyDescriptor.class, null));

        // ********************************************************************************
        // create a descriptor array from this method
        // TODO this should go into the base class
        List<IPropertyDescriptor> propDescs = new ArrayList<IPropertyDescriptor>();
        for (IntDesc i : propDescMap.values()) {
            PropertyDescriptor pdesc;
            if(i.descriptorClass == TextPropertyDescriptor.class)
                pdesc = new TextPropertyDescriptor(i.id, i.name);
            else if(i.descriptorClass == ColorPropertyDescriptor.class)
                pdesc = new ColorPropertyDescriptor(i.id, i.name);
            else 
                pdesc = new PropertyDescriptor(i.id, i.name);  // read only editor

            // set the other parameters
            pdesc.setCategory(i.category);
            pdesc.setDescription(i.description);
            pdesc.setFilterFlags(i.filters);
            
            // add itt to the property descriptor array
            propDescs.add(pdesc);
        }
        // convert it to descriptor array 
        propertyDescArray = propDescs.toArray(new IPropertyDescriptor[propDescs.size()]);
        
    }
    
    public SubmoduleDisplayPropertySource(String dspString) {
        super(dspString);
    }
    
    public SubmoduleDisplayPropertySource(SubmoduleNodeEx model) {
        linkedModel = model;
        setValue(linkedModel.getDisplayString());
    }

    @Override
    protected DisplayPropertySource getEmptyDefaults() {
        return emptyDefaults;
    }

    @Override
    protected DisplayPropertySource getVariableDefaults() {
        return variableDefaults;
    }
    
    public IPropertyDescriptor[] getPropertyDescriptors() {
        return getStaticPropertyDescriptors();
    }

    public IPropertyDescriptor[] getStaticPropertyDescriptors() {
        return propertyDescArray;
    }

    @Override
    protected LinkedHashMap<String, IntDesc> getPropDescMap() {
        return propDescMap;
    }
    
    @Override
    protected void fireDisplayStringChanged() {
        linkedModel.setDisplayString(getValue());
    }

}
