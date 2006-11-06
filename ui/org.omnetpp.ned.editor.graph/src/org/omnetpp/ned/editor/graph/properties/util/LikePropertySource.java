package org.omnetpp.ned.editor.graph.properties.util;
 
import java.util.List;

import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.IPropertySource2;
import org.omnetpp.common.properties.EditableComboBoxPropertyDescriptor;
import org.omnetpp.ned2.model.interfaces.ITyped;

/**
 * @author rhornig
 * A property source for displaying Type info (using an editable combobox). The content of the
 * dropdown combo should be provided by overriding the getPossibleValues mathod.
 */
public abstract class LikePropertySource implements IPropertySource2 {
        public enum Prop { Like }
        protected IPropertyDescriptor[] descriptors;
        protected ITyped model;
        EditableComboBoxPropertyDescriptor likeProp;

        public LikePropertySource(ITyped nodeModel) {
            model = nodeModel;
            
            // set up property descriptors
            likeProp = new EditableComboBoxPropertyDescriptor(Prop.Like, "like");
            likeProp.setCategory("Base");
            likeProp.setDescription("The interface type of the component");
            
            descriptors = new IPropertyDescriptor[] { likeProp };
        }

        public Object getEditableValue() {
            return model.getLikeType();
        }

        public IPropertyDescriptor[] getPropertyDescriptors() {
            //fill the connection combobox with types
            likeProp.setItems(getPossibleValues());
            return descriptors;
        }

        /**
         * @return The list of possible values that will be used to fill a dropdown box.
         */
        protected abstract List<String> getPossibleValues(); 
        
        public Object getPropertyValue(Object propName) {
            if (Prop.Like.equals(propName))  
                return model.getLikeType(); 
            
            return null;
        }

        public void setPropertyValue(Object propName, Object value) {
            if (Prop.Like.equals(propName)) 
                model.setLikeType((String)value);
        }

        public boolean isPropertySet(Object propName) {
            if (Prop.Like.equals(propName)) 
                return !"".equals(model.getLikeType()) && (model.getLikeType() != null);

            return false;
        }

        public void resetPropertyValue(Object propName) {
            if (Prop.Like.equals(propName)) 
                model.setLikeType(null);
        }

        public boolean isPropertyResettable(Object propName) {
            return true;
        }
}
