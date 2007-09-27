package org.omnetpp.ned.editor.graph.properties.util;

import java.util.List;

import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.PropertyDescriptor;
import org.eclipse.ui.views.properties.TextPropertyDescriptor;

import org.omnetpp.common.properties.EditableComboBoxPropertyDescriptor;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.model.interfaces.IHasType;

/**
 * A property source for displaying Type info and Like-Type with its parameter
 * (using an editable combobox). The content of the dropdown combo should be
 * provided by overriding the getPossibleValues method.
 *
 * @author rhornig
 */
public abstract class TypePropertySource extends NedBasePropertySource {
        public enum Prop { Type, Like, LikeParam }
        protected IPropertyDescriptor[] descriptors;
        EditableComboBoxPropertyDescriptor typeProp;
        EditableComboBoxPropertyDescriptor likeProp;

        public TypePropertySource(IHasType nodeModel) {
            super(nodeModel);

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
         * Returns the list of possible values that will be used to fill a dropdown box.
         */
        protected abstract List<String> getPossibleValues();

        public Object getPropertyValue(Object propName) {
            if (Prop.Type.equals(propName))
                return getHasTypeModel().getType();
            if (Prop.Like.equals(propName))
                return getHasTypeModel().getLikeType();
            if (Prop.LikeParam.equals(propName))
                return getHasTypeModel().getLikeParam();

            return null;
        }

        public void setPropertyValue(Object propName, Object value) {
            if (Prop.Type.equals(propName))
                getHasTypeModel().setType((String)value);
            if (Prop.Like.equals(propName))
                getHasTypeModel().setLikeType((String)value);
            if (Prop.LikeParam.equals(propName))
                getHasTypeModel().setLikeParam((String)value);
        }

        public boolean isPropertySet(Object propName) {
            if (Prop.Type.equals(propName))
                return StringUtils.isNotEmpty(getHasTypeModel().getType()); ;
            if (Prop.Like.equals(propName))
                return StringUtils.isNotEmpty(getHasTypeModel().getLikeType());
            if (Prop.LikeParam.equals(propName))
                return StringUtils.isNotEmpty(getHasTypeModel().getLikeParam());

            return false;
        }

        public void resetPropertyValue(Object propName) {
            if (Prop.Type.equals(propName))
                getHasTypeModel().setType(null);
            if (Prop.Like.equals(propName))
                getHasTypeModel().setLikeType(null);
            if (Prop.LikeParam.equals(propName))
                getHasTypeModel().setLikeParam(null);
        }

        public boolean isPropertyResettable(Object propName) {
            return true;
        }
        
        public IHasType getHasTypeModel() {
            return (IHasType)getModel();
        }
}
