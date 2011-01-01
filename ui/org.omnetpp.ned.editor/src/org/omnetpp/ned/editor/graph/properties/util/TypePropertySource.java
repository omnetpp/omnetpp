/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.properties.util;

import java.util.List;

import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.PropertyDescriptor;
import org.eclipse.ui.views.properties.TextPropertyDescriptor;
import org.omnetpp.common.properties.EditableComboBoxPropertyDescriptor;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.model.ex.NedElementUtilEx;
import org.omnetpp.ned.model.interfaces.ISubmoduleOrConnection;

/**
 * A property source for displaying Type info and Like-Type with its parameter
 * (using an editable combobox). The content of the dropdown combo should be
 * provided by overriding the getPossibleValues method.
 *
 * @author rhornig
 */
public class TypePropertySource extends NedBasePropertySource {
        public enum Prop { Type, Like, LikeParam }
        protected IPropertyDescriptor[] descriptors;
        EditableComboBoxPropertyDescriptor typeProp;
        EditableComboBoxPropertyDescriptor likeProp;
        List<String> typeValues;
        List<String> likeTypeValues;

        public TypePropertySource(ISubmoduleOrConnection nodeModel, List<String> typeValues, List<String> likeTypeValues) {
            super(nodeModel);
            this.typeValues = typeValues;
            this.likeTypeValues = likeTypeValues;

            // set up property descriptors
            typeProp = new EditableComboBoxPropertyDescriptor(Prop.Type, "type");
            typeProp.setCategory(MergedPropertySource.BASE_CATEGORY);
            typeProp.setValidator(new ExistingTypeNameValidator(typeValues));
            typeProp.setDescription("The type of the component");

            likeProp = new EditableComboBoxPropertyDescriptor(Prop.Like, "like");
            likeProp.setCategory(MergedPropertySource.BASE_CATEGORY);
            likeProp.setValidator(new ExistingTypeNameValidator(likeTypeValues));
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
            typeProp.setItems(typeValues);
            likeProp.setItems(likeTypeValues);
            return descriptors;
        }

        public Object getPropertyValue(Object propName) {
            if (Prop.Type.equals(propName)) {
                String result = lookupFullyQualifiedName(getHasTypeModel().getEnclosingLookupContext(), getHasTypeModel().getType());
                return convertQNameToDisplayName(result);
            }
            if (Prop.Like.equals(propName)) {
                String result = lookupFullyQualifiedName(getHasTypeModel().getEnclosingLookupContext(), getHasTypeModel().getLikeType());
                return convertQNameToDisplayName(result);
            }
            if (Prop.LikeParam.equals(propName))
                return getHasTypeModel().getLikeParam();

            return null;
        }

        public void setPropertyValue(Object propName, Object value) {
            if (Prop.Type.equals(propName))
                getHasTypeModel().setType(convertDisplayNameToQName((String)value));
            if (Prop.Like.equals(propName))
                getHasTypeModel().setLikeType(convertDisplayNameToQName((String)value));
            if (Prop.LikeParam.equals(propName))
                getHasTypeModel().setLikeParam((String)value);
            
            NedElementUtilEx.addImportFor(getHasTypeModel()); // note: overwrites "type" (or "like-type") attribute
            // note that this will add an import statement if needed, but WILL not remove when undo is invoked
            // there is no way to distinguish here between do and undo operations
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

        public ISubmoduleOrConnection getHasTypeModel() {
            return (ISubmoduleOrConnection)getModel();
        }
}
