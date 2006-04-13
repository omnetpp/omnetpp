package org.omnetpp.ned.editor.graph.properties;

import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.PropertyDescriptor;
import org.eclipse.ui.views.properties.TextPropertyDescriptor;
import org.omnetpp.ned2.model.NEDElement;
import org.omnetpp.ned2.model.SubmoduleDisplayString;
import org.omnetpp.ned2.model.SubmoduleNodeEx;

public class SubmodulePropertySource extends AbstractNedPropertySource {

    protected static IPropertyDescriptor[] descriptors;
    
    public enum Prop { Name, Type, Display }

    static {
        PropertyDescriptor nameProp = new TextPropertyDescriptor(Prop.Name, "Name");
        PropertyDescriptor typeProp = new TextPropertyDescriptor(Prop.Type, "Type");
        PropertyDescriptor displayProp = new TextPropertyDescriptor(Prop.Display, "Display");
        descriptors = new IPropertyDescriptor[] { nameProp, typeProp, displayProp };
    }

    protected SubmoduleNodeEx model;
    protected SubmoduleDisplayPropertySource submoduleDisplayPropertySource;
    
    public SubmodulePropertySource(SubmoduleNodeEx submoduleNodeModel) {
        super(submoduleNodeModel);
        model = submoduleNodeModel;
        // create a nested displayPropertySource
        submoduleDisplayPropertySource = 
            new SubmoduleDisplayPropertySource(model);
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
        if (Prop.Type.equals(propName)) { 
            return model.getType(); 
        }
        if (Prop.Display.equals(propName)) { 
            return submoduleDisplayPropertySource; 
        }
        return null;
    }

    @Override
    public void setPropertyValue(Object propName, Object value) {
        if (Prop.Name.equals(propName)) {
            model.setName(value.toString());
        }
        if (Prop.Type.equals(propName)) {
            model.setType(value.toString());
        }
        if (Prop.Display.equals(propName)) {
            model.setDisplayString(new SubmoduleDisplayString(value.toString()));
        }
    }

    @Override
    public boolean isPropertySet(Object propName) {
        return Prop.Name.equals(propName) || Prop.Type.equals(propName) ||
            Prop.Display.equals(propName);
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
