package org.omnetpp.ned.editor.graph.properties;

import java.util.EnumSet;

import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.PropertyDescriptor;
import org.eclipse.ui.views.properties.TextPropertyDescriptor;
import org.omnetpp.common.displaymodel.DisplayString;
import org.omnetpp.ned2.model.SimpleModuleNodeEx;

public class SimpleModulePropertySource extends NamedNedPropertySource {

    protected IPropertyDescriptor[] descriptors;
    protected SimpleModuleNodeEx model;
    protected SimpleModuleDisplayPropertySource simpleModuleDisplayPropertySource;

    public static class SimpleModuleDisplayPropertySource extends DisplayPropertySource {
        protected static IPropertyDescriptor[] propertyDescArray;
        protected SimpleModuleNodeEx model;

        public SimpleModuleDisplayPropertySource(SimpleModuleNodeEx model) {
            super(model);
            this.model = model;
            setDisplayString(model.getDisplayString());
            supportedProperties = EnumSet.range(DisplayString.Prop.WIDTH, 
                                                DisplayString.Prop.IMAGESIZE);
        }

        @Override
        public void modelChanged() {
            if(model != null)
                setDisplayString(model.getDisplayString());
        }

    }

    public enum Prop { Name, Display }
    
    public SimpleModulePropertySource(SimpleModuleNodeEx simpleModuleNodeModel) {
        super(simpleModuleNodeModel);
        model = simpleModuleNodeModel;
        // create a nested displayPropertySource
        simpleModuleDisplayPropertySource = 
            new SimpleModuleDisplayPropertySource(model);
        
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
            return simpleModuleDisplayPropertySource; 
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
