package org.omnetpp.ned.editor.graph.properties;

import java.util.Set;

import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.PropertyDescriptor;
import org.eclipse.ui.views.properties.TextPropertyDescriptor;
import org.omnetpp.common.properties.EditableComboBoxPropertyDescriptor;
import org.omnetpp.ned2.model.SubmoduleNodeEx;
import org.omnetpp.resources.NEDResourcesPlugin;

public class SubmodulePropertySource extends AbstractNedPropertySource {

    protected IPropertyDescriptor[] descriptors;
    
    public enum Prop { Name, Type, Display }

    protected SubmoduleNodeEx model;
    protected SubmoduleDisplayPropertySource submoduleDisplayPropertySource;
    protected EditableComboBoxPropertyDescriptor typeProp;

    
    public SubmodulePropertySource(SubmoduleNodeEx submoduleNodeModel) {
        super(submoduleNodeModel);
        model = submoduleNodeModel;
        // create a nested displayPropertySource
        submoduleDisplayPropertySource = 
            new SubmoduleDisplayPropertySource(model);
        
        // set up property descriptors
        PropertyDescriptor nameProp = new TextPropertyDescriptor(Prop.Name, "Name");
        PropertyDescriptor displayProp = new TextPropertyDescriptor(Prop.Display, "Display");
        typeProp = new EditableComboBoxPropertyDescriptor(Prop.Type, "Type");
        descriptors = new IPropertyDescriptor[] { nameProp, typeProp, displayProp };
    }

    @Override
    public Object getEditableValue() {
        // we don't need this if we don't want to embed this property source into an other propertysource
        return model.getName();
    }

    @Override
    public IPropertyDescriptor[] getPropertyDescriptors() {
        // fill in the type combobox
        Set<String> moduleNames = NEDResourcesPlugin.getNEDResources().getModuleNames();
        // TODO sort the types alphabetically
        typeProp.setItems(moduleNames);
        
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
            model.getDisplayString().set(value.toString());
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
            model.getDisplayString().set(null);
        }
    }

    @Override
    public boolean isPropertyResettable(Object propName) {
        return Prop.Display.equals(propName);
    }

}
