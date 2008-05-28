package org.omnetpp.ned.editor.graph.properties.util;

import java.util.EnumSet;

import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.PropertyDescriptor;
import org.eclipse.ui.views.properties.TextPropertyDescriptor;

import org.omnetpp.common.properties.ColorPropertyDescriptor;
import org.omnetpp.common.properties.EnumComboboxPropertyDescriptor;
import org.omnetpp.common.properties.ImagePropertyDescriptor;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.model.DisplayString;
import org.omnetpp.ned.model.interfaces.IHasDisplayString;

/**
 * Base display string property source. Common behavior to all display string property sources
 *
 * @author rhornig
 */
//TODO implement number cell editor
abstract public class DisplayPropertySource extends NedBasePropertySource {

    // property descriptor for the sheet (could be static if pushed upper)
    protected IPropertyDescriptor[] propertyDescArray = null;
    // by default supports all possible properties defined in DisplayString
    protected EnumSet<DisplayString.Prop> supportedProperties = EnumSet.noneOf(DisplayString.Prop.class);

    public DisplayPropertySource(IHasDisplayString model) {
        super(model);
        // by default we provide only  the single line display property editor
        supportedProperties.add(DisplayString.Prop.DISPLAY);
    }

    /**
     * Creates and returns a property descriptor for the given display string property.
     */
    public static IPropertyDescriptor getPropertyDescriptor(DisplayString.Prop prop) {
        PropertyDescriptor pdesc;
        if (prop.getEnumSpec() != null)
        	pdesc = new EnumComboboxPropertyDescriptor(prop, prop.getName(), prop.getEnumSpec());
        else if (prop.getType() == DisplayString.PropType.STRING)
            pdesc = new TextPropertyDescriptor(prop, prop.getName());
        else if (prop.getType() == DisplayString.PropType.UNIT)
            pdesc = new TextPropertyDescriptor(prop, prop.getName());
        else if (prop.getType() == DisplayString.PropType.INTEGER)
            pdesc = new TextPropertyDescriptor(prop, prop.getName());
        else if (prop.getType() == DisplayString.PropType.IMAGE)
            pdesc = new ImagePropertyDescriptor(prop, prop.getName());
        else if (prop.getType() == DisplayString.PropType.COLOR)
            pdesc = new ColorPropertyDescriptor(prop, prop.getName());
        else
            pdesc = new PropertyDescriptor(prop, prop.getName());  // read-only editor

        // set the other parameters
        pdesc.setCategory(prop.getGroup().name());
        pdesc.setDescription(prop.getTag()+"["+prop.getPos()+"]: "+prop.getFullDesc());
        // the one line display string descriptor should be always incompatible, so selecting
        // two or more items will automatically hide this composite property
        if (prop == DisplayString.Prop.DISPLAY)
            pdesc.setAlwaysIncompatible(true);

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
        return ((IHasDisplayString)getModel()).getDisplayString();
    }

    @Override
    public Object getEditableValue() {
        return toString();
    }

    // property request for property sheet
    @Override
    public Object getPropertyValue(Object propObj) {
    	if (!(propObj instanceof DisplayString.Prop))
    		return null;

        // check if we requested the "single line" DISPLAY property
        if (propObj == DisplayString.Prop.DISPLAY)
        	return getEditableValue();

        DisplayString.Prop prop = (DisplayString.Prop)propObj;
        // otherwise look for a single tag/attribute
        String tagVal = getDisplayString().getAsStringLocal(prop);
        // if the property does not exists yet, return default empty value
        if (tagVal == null)
            tagVal = DisplayString.EMPTY_TAG_VALUE;

        return tagVal;
    }

    @Override
    public void setPropertyValue(Object propObj, Object value) {
    	if (!(propObj instanceof DisplayString.Prop))
    		return;

        // check if we requested the "single line" DISPLAY property
        if (propObj == DisplayString.Prop.DISPLAY) {
            getDisplayString().set((String)value);
        	return;
        }

        // otherwise set a single attribute
        DisplayString.Prop prop = (DisplayString.Prop)propObj;

        if (value != null)
            getDisplayString().set(prop, value.toString());
        else
            getDisplayString().set(prop, null);

    }

    @Override
    public void resetPropertyValue(Object prop) {
    	if (!(prop instanceof DisplayString.Prop))
    		return;

        // check if we requested the "single line" DISPLAY property, reset the whole display string
        if (prop == DisplayString.Prop.DISPLAY) {
            getDisplayString().set(null);
        	return;
        }

        // set only a single attribute
        getDisplayString().set((DisplayString.Prop)prop, null);
    }

    @Override
    public boolean isPropertySet(Object prop) {
    	if (!(prop instanceof DisplayString.Prop))
    		return false;

        // check if we requested the "single line" DISPLAY property
        if (prop == DisplayString.Prop.DISPLAY)
            return StringUtils.isNotEmpty(getDisplayString().toString());

        // otherwise check a single attribute
        return StringUtils.isNotEmpty(getDisplayString().getAsStringLocal((DisplayString.Prop)prop));
    }

    @Override
    public String toString() {
        return getDisplayString().toString();
    }

    @Override
    public boolean isPropertyResettable(Object id) {
        return true;
    }

}
