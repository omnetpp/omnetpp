package org.omnetpp.ned.editor.graph.properties.util;
 
import java.util.List;

import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.IPropertySource2;
import org.omnetpp.common.properties.EditableComboBoxPropertyDescriptor;
import org.omnetpp.ned2.model.interfaces.IStringTyped;

/**
 * @author rhornig
 * A property source for displaying Type info (using an editable combobox). The content of the
 * dropdown combo should be provided by overriding the getPossibleValues mathod.
 */
public abstract class TypePropertySource implements IPropertySource2 {
        public enum Prop { Type }
        protected IPropertyDescriptor[] descriptors;
        protected IStringTyped model;
        EditableComboBoxPropertyDescriptor typeProp;

        public TypePropertySource(IStringTyped nodeModel) {
            model = nodeModel;
            
            // set up property descriptors
            typeProp = new EditableComboBoxPropertyDescriptor(Prop.Type, "type");
            typeProp.setCategory("Base");
            typeProp.setDescription("The type of the component");
            
            descriptors = new IPropertyDescriptor[] { typeProp };
        }

        public Object getEditableValue() {
            return model.getType();
        }

        public IPropertyDescriptor[] getPropertyDescriptors() {
            //fill the connection combobox with types
            typeProp.setItems(getPossibleValues());
            return descriptors;
        }

        /**
         * @return The list of possible values that will be used to fill a dropdown box.
         */
        protected abstract List<String> getPossibleValues(); 
        
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
