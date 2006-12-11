package org.omnetpp.ned.editor.graph.properties.util;
 
import java.util.List;

import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.IPropertySource2;
import org.eclipse.ui.views.properties.PropertyDescriptor;
import org.eclipse.ui.views.properties.TextPropertyDescriptor;
import org.omnetpp.common.properties.EditableComboBoxPropertyDescriptor;
import org.omnetpp.ned.model.interfaces.IHasType;

/**
 * @author rhornig
 * A property source for displaying Type info and Like-Type with its parameter
 *  (using an editable combobox). The content of the dropdown combo should be 
 *  provided by overriding the getPossibleValues method.
 */
public abstract class TypePropertySource implements IPropertySource2 {
        public enum Prop { Type, Like, LikeParam }
        protected IPropertyDescriptor[] descriptors;
        protected IHasType model;
        EditableComboBoxPropertyDescriptor typeProp;
        EditableComboBoxPropertyDescriptor likeProp;

        public TypePropertySource(IHasType nodeModel) {
            model = nodeModel;
            
            // set up property descriptors
            typeProp = new EditableComboBoxPropertyDescriptor(Prop.Type, "type");
            typeProp.setCategory(MergedPropertySource.BASE_CATEGORY);
            typeProp.setDescription("The type of the component");
            
            likeProp = new EditableComboBoxPropertyDescriptor(Prop.Like, "like");
            likeProp.setCategory(MergedPropertySource.BASE_CATEGORY);
            likeProp.setDescription("The interface type of the component");
            
            PropertyDescriptor likeParamProp = new TextPropertyDescriptor(Prop.LikeParam, "like-param");
            likeParamProp.setCategory(MergedPropertySource.BASE_CATEGORY);
            likeParamProp.setDescription("Parameter for like syntax");
            
            descriptors = new IPropertyDescriptor[] { typeProp, likeProp, likeParamProp };
        }

        public Object getEditableValue() {
            return this;
        }

        public IPropertyDescriptor[] getPropertyDescriptors() {
            //fill the connection combobox with types
            List<String> values = getPossibleValues();
            typeProp.setItems(values);
            likeProp.setItems(values);
            return descriptors;
        }

        /**
         * @return The list of possible values that will be used to fill a dropdown box.
         */
        protected abstract List<String> getPossibleValues(); 
        
        public Object getPropertyValue(Object propName) {
            if (Prop.Type.equals(propName))  
                return model.getType(); 
            if (Prop.Like.equals(propName))  
                return model.getLikeType(); 
            if (Prop.LikeParam.equals(propName))  
                return model.getLikeParam(); 
            
            return null;
        }

        public void setPropertyValue(Object propName, Object value) {
            if (Prop.Type.equals(propName)) 
                model.setType((String)value);
            if (Prop.Like.equals(propName)) 
                model.setLikeType((String)value);
            if (Prop.LikeParam.equals(propName)) 
                model.setLikeParam((String)value);
        }

        public boolean isPropertySet(Object propName) {
            if (Prop.Type.equals(propName)) 
                return !"".equals(model.getType()) && (model.getType() != null);
            if (Prop.Like.equals(propName)) 
                return !"".equals(model.getLikeType()) && (model.getLikeType() != null);
            if (Prop.LikeParam.equals(propName)) 
                return !"".equals(model.getLikeParam()) && (model.getLikeParam() != null);

            return false;
        }

        public void resetPropertyValue(Object propName) {
            if (Prop.Type.equals(propName)) 
                model.setType(null);
            if (Prop.Like.equals(propName)) 
                model.setLikeType(null);
            if (Prop.LikeParam.equals(propName)) 
                model.setLikeParam(null);
        }

        public boolean isPropertyResettable(Object propName) {
            return true;
        }
}
