package org.omnetpp.ned.editor.graph.properties;

import java.util.EnumSet;

import org.eclipse.swt.graphics.RGB;
import org.eclipse.ui.views.properties.ColorPropertyDescriptor;
import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.PropertyDescriptor;
import org.eclipse.ui.views.properties.TextPropertyDescriptor;
import org.omnetpp.ned.editor.graph.misc.ColorFactory;
import org.omnetpp.ned2.model.DisplayString;
import org.omnetpp.ned2.model.NEDElement;

//TODO Colors cannot be edited by hand. A derived ColorCellEditor is required
//TODO Some property needs a combo box cell editor
//TODO an icon selector / editorcell must be implemented
//TODO multi line text cell editor must be implemented
//TODO implement getColorProperty/Def too so unknown colors will be displayed as default
//TODO we need a constructor where we can set a default DisplayPropertySource which is used as
//   default if a tag is empty (it is coming from the ancestor type)
abstract public class DisplayPropertySource extends AbstractNedPropertySource {

    // string parser for handling the parsing of the display string tags 
    protected DisplayString displayString = null;
    // property descriptor for the sheet (could be static if pushed uper)
    protected IPropertyDescriptor[] propertyDescArray = null;
    // by default supports all possible properties defined in DisplayString 
    protected EnumSet<DisplayString.Prop> supportedProperties 
                  = EnumSet.allOf(DisplayString.Prop.class);

    public DisplayPropertySource(NEDElement model) {
        super(model);
    }
    
    /**
     * @param prop The property we want a descriptor for
     * @return The property descriptor used for the property
     */
    protected IPropertyDescriptor getPropertyDescriptor(DisplayString.Prop prop) {
        // TODO we need separate property descriptors for integers and the Images
        PropertyDescriptor pdesc;
        if(prop.getType() == DisplayString.PropType.String)
            pdesc = new TextPropertyDescriptor(prop, prop.getVisibleName());
        else if(prop.getType() == DisplayString.PropType.Integer)
            pdesc = new TextPropertyDescriptor(prop, prop.getVisibleName());
        else if(prop.getType() == DisplayString.PropType.Image)
            pdesc = new TextPropertyDescriptor(prop, prop.getVisibleName());
        else if(prop.getType() == DisplayString.PropType.Color)
            pdesc = new ColorPropertyDescriptor(prop, prop.getVisibleName());
        else 
            pdesc = new PropertyDescriptor(prop, prop.getVisibleName());  // read only editor

        // set the other parameters
        pdesc.setCategory(prop.getGroup().name());
        pdesc.setDescription(prop.getVisibleDesc());
        
        return pdesc;
    }
    
    protected void createPropertyDescriptors() {
        int i = 0;
        propertyDescArray = new IPropertyDescriptor[supportedProperties.size()];
        // put all supported properties into the descriptor array
        for (DisplayString.Prop prop : supportedProperties) {
            propertyDescArray[i] = getPropertyDescriptor(prop);
            i++;
        }
    }

    public IPropertyDescriptor[] getPropertyDescriptors() {
        if (propertyDescArray == null)
            createPropertyDescriptors();
        
        return propertyDescArray;
    }

    public DisplayString getDisplayString() {
        return displayString;
    }

    public void setDisplayString(DisplayString displayString) {
        this.displayString = displayString;
    } 

    @Override
    public Object getEditableValue() {
        return this.toString();
    }

    /**
     * @param propName
     * @return Null if tag does not exist, TagInstance.EMPTY_VALUE if the value is empty or the property value itself
     */
//    public String getStringProperty(String propName) {
//        IntDesc pd = getPropDescMap().get(propName);
//        // unknown tag
//        if(pd == null) return null;
//
//        return dispString.getTagArg(pd.tagName, pd.tagPos );
//    }


    /**
     * Returns the property value as an Integer
     * @param propName
     * @return The value as Integer or 0 if value was empty or non number, 
     *  and <code>null</code> if the tag was not present at all
     */
//    public Integer getIntegerProperty(String propName) {
//        String strVal = getStringProperty(propName);
//        // if tag not present at all
//        if(strVal == null) return null;
//        try {
//            return Integer.valueOf(strVal);
//        } catch (NumberFormatException e) { }
//        return new Integer(0);
//    }

    // returns the tag value as string or the default value 
//    public String getStringPropertyDef(String propName) {
//        IntDesc pd = getPropDescMap().get(propName);
//        // unknown tag
//        if(pd == null) return null;
//
//        return dispString.getTagArgUsingDefs(pd.tagName, pd.tagPos );
//    }
    
    /**
     * @param prop
     * @return The value of property as Integer / 
     * the variableDefault if it is a variable ($var) / the global default value / 0 if no 
     * defaults were specified / <code>null</code> if the tag itself does not exist 
     */
    // TODO move this to the DisplayString class (getTagArgAsIntUsingDef etc)
//    public Integer getIntegerPropertyDef(String propName) {
//        String strVal = getStringProperty(propName);
//        // if tag not present at all
//        if(strVal == null) return null;
//        // check if this is a variable (starting with $) and use the variable deault set
////        if(strVal.startsWith("$"))
////            strVal = getVariableDefaults().getStringProperty(propName);
//        
//        // otherwise use the original one
//        if (!DisplayString.TagInstance.EMPTY_VALUE.equals(strVal))
//            try {
//                return Integer.valueOf(strVal);
//            } catch (NumberFormatException e) { 
//                return new Integer(0); 
//            }
//        // look for 'empty' default value if no values were specified either in dispstring or as variable default
////        strVal = getEmptyDefaults().getStringProperty(propName);
//        // return 0 if no default defined
//        if(strVal == null) return new Integer(0);
//        try {
//            return Integer.valueOf(strVal);
//        } catch (NumberFormatException e) { }
//        // return 0 if conversion was again unsuccessful
//        return new Integer(0);
//    }
    

//    public int getIntPropertyDef(String propName, int defValue) {
//        int val = defValue;
//        try {
//            val = getIntegerPropertyDef(propName).intValue();
//        } catch (Exception e) {
//        }
//        return val;
//    }
    
    // property request for property sheet
    public Object getPropertyValue(Object propObj) {
        DisplayString.Prop prop = (DisplayString.Prop)propObj;
        String tagVal = displayString.getAsString(prop);
        // if the property does not exists yet, return default empty value
        if (tagVal == null) 
            tagVal = DisplayString.TagInstance.EMPTY_VALUE; 
        
        if(prop.getType() == DisplayString.PropType.Color)
            return ColorFactory.asRGB(tagVal);
        
        return tagVal;
    }

    public void setPropertyValue(Object propObj, Object value) {
        DisplayString.Prop prop = (DisplayString.Prop)propObj;
        // if it is a color, convert it to string
        if(value instanceof RGB)
            value = ColorFactory.asString((RGB)value);
        
        if (value != null)        
            displayString.set(prop, value.toString());
        else 
            displayString.set(prop, null);
        
        fireDisplayStringChanged();
    }

    public void resetPropertyValue(Object prop) {
        displayString.set((DisplayString.Prop)prop, null);
        fireDisplayStringChanged();
    }

    public boolean isPropertySet(Object prop) {
        String val = displayString.getAsString((DisplayString.Prop)prop);
        return val != null && !DisplayString.TagInstance.EMPTY_VALUE.equals(val);
    }

    public String toString() {
        return displayString.toString();
    }

    public boolean isPropertyResettable(Object id) {
        return true;
    }
    
    abstract protected void fireDisplayStringChanged();

}