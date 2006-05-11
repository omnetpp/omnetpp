package org.omnetpp.ned.editor.graph.properties;

import java.util.EnumSet;

import org.eclipse.swt.graphics.RGB;
import org.eclipse.ui.views.properties.ColorPropertyDescriptor;
import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.PropertyDescriptor;
import org.eclipse.ui.views.properties.TextPropertyDescriptor;
import org.omnetpp.common.color.ColorFactory;
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
                  = EnumSet.noneOf(DisplayString.Prop.class);

    public DisplayPropertySource(NEDElement model) {
        super(model);
    }
    
    /**
     * @param prop The property we want a descriptor for
     * @return The property descriptor used for the property
     */
    public static IPropertyDescriptor getPropertyDescriptor(DisplayString.Prop prop) {
        // TODO we need separate property descriptors for integers and the Images
        PropertyDescriptor pdesc;
        if(prop.getType() == DisplayString.PropType.STRING)
            pdesc = new TextPropertyDescriptor(prop, prop.getVisibleName());
        else if(prop.getType() == DisplayString.PropType.UNIT)
            pdesc = new TextPropertyDescriptor(prop, prop.getVisibleName());
        else if(prop.getType() == DisplayString.PropType.INTEGER)
            pdesc = new TextPropertyDescriptor(prop, prop.getVisibleName());
        else if(prop.getType() == DisplayString.PropType.IMAGE)
            pdesc = new TextPropertyDescriptor(prop, prop.getVisibleName());
        else if(prop.getType() == DisplayString.PropType.COLOR)
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

    @Override
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

    // property request for property sheet
    @Override
    public Object getPropertyValue(Object propObj) {
        DisplayString.Prop prop = (DisplayString.Prop)propObj;
        String tagVal = displayString.getAsString(prop);
        // if the property does not exists yet, return default empty value
        if (tagVal == null) 
            tagVal = DisplayString.TagInstance.EMPTY_VALUE; 
        
        if(prop.getType() == DisplayString.PropType.COLOR)
            return ColorFactory.asRGB(tagVal);
        
        return tagVal;
    }

    @Override
    public void setPropertyValue(Object propObj, Object value) {
        DisplayString.Prop prop = (DisplayString.Prop)propObj;
        // if it is a color, convert it to string
        if(value instanceof RGB)
            value = ColorFactory.asString((RGB)value);
        
        if (value != null)        
            displayString.set(prop, value.toString());
        else 
            displayString.set(prop, null);
        
    }

    @Override
    public void resetPropertyValue(Object prop) {
        displayString.set((DisplayString.Prop)prop, null);
    }

    @Override
    public boolean isPropertySet(Object prop) {
        String val = displayString.getAsString((DisplayString.Prop)prop);
        return val != null && !DisplayString.TagInstance.EMPTY_VALUE.equals(val);
    }

    @Override
    public String toString() {
        return displayString.toString();
    }

    @Override
    public boolean isPropertyResettable(Object id) {
        return true;
    }
    
}