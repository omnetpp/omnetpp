package org.omnetpp.ned.editor.graph.properties;

import java.util.EnumSet;

import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.PropertyDescriptor;
import org.eclipse.ui.views.properties.TextPropertyDescriptor;
import org.omnetpp.common.displaymodel.DisplayString;
import org.omnetpp.ned2.model.ChannelNodeEx;

public class ChannelPropertySource extends AbstractNedPropertySource {

    protected IPropertyDescriptor[] descriptors;
    protected ChannelNodeEx model;
    protected ChannelDisplayPropertySource channelDisplayPropertySource;

    public enum Prop { Name, Display }

    public static class ChannelDisplayPropertySource extends DisplayPropertySource {
        protected static IPropertyDescriptor[] propertyDescArray;
        protected ChannelNodeEx model;


        public ChannelDisplayPropertySource(ChannelNodeEx model) {
            super(model);
            this.model = model;
            setDisplayString(model.getDisplayString());
            // define which properties should be displayed in the property sheet
//            supportedProperties = EnumSet.range(DisplayString.Prop.ROUTING_MODE, 
//                                                DisplayString.Prop.BENDPOINTS);
            // we do not support all properties currently, just colow, width ans style
            supportedProperties = EnumSet.range(DisplayString.Prop.CONNECTION_COL, 
                    DisplayString.Prop.CONNECTION_STYLE);
            
            supportedProperties.addAll(EnumSet.range(DisplayString.Prop.TEXT, DisplayString.Prop.TEXTPOS));
            supportedProperties.add(DisplayString.Prop.TOOLTIP);
        }

        @Override
        public void modelChanged() {
            if(model != null)
                setDisplayString(model.getDisplayString());
        }

    }

    
    public ChannelPropertySource(ChannelNodeEx channelNodeModel) {
        super(channelNodeModel);
        model = channelNodeModel;
        // create a nested displayPropertySource
        channelDisplayPropertySource = 
            new ChannelDisplayPropertySource(model);
        
        // set up property descriptors
        PropertyDescriptor nameProp = new TextPropertyDescriptor(Prop.Name, "Name");
        PropertyDescriptor displayProp = new TextPropertyDescriptor(Prop.Display, "Display");
        descriptors = new IPropertyDescriptor[] { nameProp, displayProp };
    }

    @Override
    public Object getEditableValue() {
        // we don't need this if we don't want to embed this property source into an other propertysource
        return model.getName();
    }

    @Override
    public IPropertyDescriptor[] getPropertyDescriptors() {
        
        return descriptors;
    }

    @Override
    public Object getPropertyValue(Object propName) {
        if (Prop.Name.equals(propName)) { 
            return model.getName(); 
        }
        if (Prop.Display.equals(propName)) { 
            return channelDisplayPropertySource; 
        }
        return null;
    }

    @Override
    public void setPropertyValue(Object propName, Object value) {
        if (Prop.Name.equals(propName)) {
            model.setName(value.toString());
        }
        if (Prop.Display.equals(propName)) {
            model.getDisplayString().set(value.toString());
        }
    }

    @Override
    public boolean isPropertySet(Object propName) {
        return Prop.Name.equals(propName) || Prop.Display.equals(propName);
    }

    @Override
    public void resetPropertyValue(Object propName) {
        if (Prop.Display.equals(propName)) {
            model.getDisplayString().set(null);
        }
    }

    @Override
    public boolean isPropertyResettable(Object propName) {
        return Prop.Display.equals(propName);
    }

}
