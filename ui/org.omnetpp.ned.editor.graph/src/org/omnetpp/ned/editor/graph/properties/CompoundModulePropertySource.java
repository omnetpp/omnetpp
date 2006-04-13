package org.omnetpp.ned.editor.graph.properties;

import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.PropertyDescriptor;
import org.eclipse.ui.views.properties.TextPropertyDescriptor;
import org.omnetpp.ned2.model.CompoundModuleNodeEx;
import org.omnetpp.ned2.model.SubmoduleDisplayString;

public class CompoundModulePropertySource extends AbstractNedPropertySource {

    protected static IPropertyDescriptor[] descriptors;
    
    public enum Prop { Name, Display }
    
    static {
        PropertyDescriptor nameProp = new TextPropertyDescriptor(Prop.Name, "Name");
        PropertyDescriptor displayProp = new TextPropertyDescriptor(Prop.Display, "Display");
        descriptors = new IPropertyDescriptor[] { nameProp, displayProp };
    }

    protected CompoundModuleNodeEx model;
    protected CompoundModuleDisplayPropertySource compoundModuleDisplayPropertySource;
    
    public CompoundModulePropertySource(CompoundModuleNodeEx submoduleNodeModel) {
        super(submoduleNodeModel);
        model = submoduleNodeModel;
        // create a nested displayPropertySource
        compoundModuleDisplayPropertySource = 
            new CompoundModuleDisplayPropertySource(model);
    }

    public Object getEditableValue() {
        // we don't need this if we don't want to embed this property source into an other propertysource
        return model.getName();
    }

    public IPropertyDescriptor[] getPropertyDescriptors() {
        return descriptors;
    }

    @Override
    public Object getPropertyValue(Object propName) {
        if (Prop.Name.equals(propName)) { 
            return model.getName(); 
        }
        if (Prop.Display.equals(propName)) { 
            return compoundModuleDisplayPropertySource; 
        }
        return null;
    }

    @Override
    public void setPropertyValue(Object propName, Object value) {
        if (Prop.Name.equals(propName)) {
            model.setName(value.toString());
        }
        if (Prop.Display.equals(propName)) {
            model.setDisplayString(new SubmoduleDisplayString(value.toString()));
        }
    }

    @Override
    public boolean isPropertySet(Object propName) {
        return Prop.Name.equals(propName) || Prop.Display.equals(propName);
    }

    @Override
    public void resetPropertyValue(Object propName) {
        if (Prop.Display.equals(propName)) {
            model.setDisplayString(null);
        }
    }

    @Override
    public boolean isPropertyResettable(Object propName) {
        return Prop.Display.equals(propName);
    }

}
