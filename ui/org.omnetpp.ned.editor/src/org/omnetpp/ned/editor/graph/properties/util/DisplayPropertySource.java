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
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.interfaces.IHasDisplayString;
import org.omnetpp.ned.model.notification.NEDModelEvent;

/**
 * TODO documentation
 *
 * @author rhornig
 */
//TODO implement number cell editor
abstract public class DisplayPropertySource extends NotifiedPropertySource {

    // string parser for handling the parsing of the display string tags
    protected DisplayString displayString = null;
    // property descriptor for the sheet (could be static if pushed upper)
    protected IPropertyDescriptor[] propertyDescArray = null;
    // by default supports all possible properties defined in DisplayString
    protected EnumSet<DisplayString.Prop> supportedProperties = EnumSet.noneOf(DisplayString.Prop.class);
    // the model that we are attached to
    private final IHasDisplayString model;

    public DisplayPropertySource(IHasDisplayString model) {
        super((INEDElement)model);
        this.model = model;
        // by default we provide only  the single line display property editor
        supportedProperties.add(DisplayString.Prop.DISPLAY);
    }

    @Override
    public void modelChanged(NEDModelEvent event) {
        if (model != null)
            setDisplayString(model.getDisplayString());
    }

    /**
     * Creates and returns a property descriptor for the given display string property.
     */
    public static IPropertyDescriptor getPropertyDescriptor(DisplayString.Prop prop) {
        PropertyDescriptor pdesc;
        if (prop.getEnumSpec() != null)
        	pdesc = new EnumComboboxPropertyDescriptor(prop, prop.getVisibleName(), prop.getEnumSpec());
        else if (prop.getType() == DisplayString.PropType.STRING)
            pdesc = new TextPropertyDescriptor(prop, prop.getVisibleName());
        else if (prop.getType() == DisplayString.PropType.UNIT)
            pdesc = new TextPropertyDescriptor(prop, prop.getVisibleName());
        else if (prop.getType() == DisplayString.PropType.INTEGER)
            pdesc = new TextPropertyDescriptor(prop, prop.getVisibleName());
        else if (prop.getType() == DisplayString.PropType.IMAGE)
            pdesc = new ImagePropertyDescriptor(prop, prop.getVisibleName());
        else if (prop.getType() == DisplayString.PropType.COLOR)
            pdesc = new ColorPropertyDescriptor(prop, prop.getVisibleName());
        else
            pdesc = new PropertyDescriptor(prop, prop.getVisibleName());  // read-only editor

        // set the other parameters
        pdesc.setCategory(prop.getGroup().name());
        pdesc.setDescription(prop.getTag()+"["+prop.getPos()+"]: "+prop.getVisibleDesc());
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
        return displayString;
    }

    public void setDisplayString(DisplayString displayString) {
        this.displayString = displayString;
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
        String tagVal = displayString.getAsStringLocal(prop);
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
        	displayString.set((String)value);
        	return;
        }

        // otherwise set a single attribute
        DisplayString.Prop prop = (DisplayString.Prop)propObj;

        if (value != null)
            displayString.set(prop, value.toString());
        else
            displayString.set(prop, null);

    }

    @Override
    public void resetPropertyValue(Object prop) {
    	if (!(prop instanceof DisplayString.Prop))
    		return;

        // check if we requested the "single line" DISPLAY property, reset the whole display string
        if (prop == DisplayString.Prop.DISPLAY) {
        	displayString.set(null);
        	return;
        }

        // set only a single attribute
        displayString.set((DisplayString.Prop)prop, null);
    }

    @Override
    public boolean isPropertySet(Object prop) {
    	if (!(prop instanceof DisplayString.Prop))
    		return false;

        // check if we requested the "single line" DISPLAY property
        if (prop == DisplayString.Prop.DISPLAY)
            return StringUtils.isNotEmpty(displayString.toString());

        // otherwise check a single attribute
        return StringUtils.isNotEmpty(displayString.getAsStringLocal((DisplayString.Prop)prop));
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
