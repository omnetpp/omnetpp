package org.omnetpp.ned.editor.graph.properties;

import java.util.ArrayList;
import java.util.Collections;
import java.util.EnumSet;
import java.util.List;

import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.IPropertySource2;
import org.omnetpp.common.displaymodel.DisplayString;
import org.omnetpp.common.properties.EditableComboBoxPropertyDescriptor;
import org.omnetpp.ned2.model.SubmoduleNodeEx;
import org.omnetpp.resources.NEDResourcesPlugin;

public class SubmodulePropertySource extends DelegatingPropertySource {

	// submodule specific display property desc
    protected static class SubmoduleDisplayPropertySource extends DisplayPropertySource {
        protected SubmoduleNodeEx model;

        public SubmoduleDisplayPropertySource(SubmoduleNodeEx model) {
            super(model);
            this.model = model;
            setDisplayString(model.getDisplayString());
            supportedProperties.addAll( EnumSet.range(DisplayString.Prop.X, 
            										  DisplayString.Prop.TOOLTIP));
        }

        @Override
        public void modelChanged() {
            if(model != null)
                setDisplayString(model.getDisplayString());
        }
    }

    // Submodule specific properties
    protected static class BasePropertySource implements IPropertySource2 {
        public enum Prop { Type  }
        protected IPropertyDescriptor[] descriptors;
        protected SubmoduleNodeEx model;
        EditableComboBoxPropertyDescriptor typeProp;

        public BasePropertySource(SubmoduleNodeEx connectionNodeModel) {
            model = connectionNodeModel;
            
            // set up property descriptors
			typeProp = new EditableComboBoxPropertyDescriptor(Prop.Type, "type");
            typeProp.setCategory("Base");
            typeProp.setDescription("The type of the submodule");
            
            descriptors = new IPropertyDescriptor[] { typeProp };
        }

        public Object getEditableValue() {
            return model.getType();
        }

        public IPropertyDescriptor[] getPropertyDescriptors() {
        	//fill the connection combobox with types
            List<String> moduleNames = new ArrayList<String>(NEDResourcesPlugin.getNEDResources().getModuleNames());
            Collections.sort(moduleNames);
            typeProp.setItems(moduleNames);
            return descriptors;
        }

        public Object getPropertyValue(Object propName) {
            if (Prop.Type.equals(propName))  
                return model.getType(); 
            
            return null;
        }

        public void setPropertyValue(Object propName, Object value) {
            if (Prop.Type.equals(propName)) 
                model.setType((String)value);
        }

        public boolean isPropertySet(Object propName) {
            if (Prop.Type.equals(propName)) 
            	return !"".equals(model.getType()) && (model.getType() != null);

            return false;
        }

        public void resetPropertyValue(Object propName) {
            if (Prop.Type.equals(propName)) 
            	model.setType(null);
        }

        public boolean isPropertyResettable(Object propName) {
            return true;
        }
    }

    public SubmodulePropertySource(SubmoduleNodeEx submoduleNodeModel) {
        super(submoduleNodeModel);
        // create a nested displayPropertySource

        addPropertySource(new NamePropertySource(submoduleNodeModel));
        addPropertySource(new BasePropertySource(submoduleNodeModel));
        addPropertySource(new SubmoduleDisplayPropertySource(submoduleNodeModel));
        
    }

}
