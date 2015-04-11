/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.properties.util;

import java.util.List;

import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.omnetpp.common.properties.EditableComboBoxPropertyDescriptor;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.model.ex.NedElementUtilEx;
import org.omnetpp.ned.model.interfaces.INedTypeElement;

/**
 * A property source for displaying Extends info (using an editable combobox). The content of the
 * dropdown combo should be provided by overriding the getPossibleValues method.
 *
 * @author rhornig
 */
public class ExtendsPropertySource extends NedBasePropertySource {
        public enum Prop { Extends }
        protected IPropertyDescriptor[] descriptors;
        EditableComboBoxPropertyDescriptor extendsProp;
        private List<String> possibleValues;

        public ExtendsPropertySource(INedTypeElement nodeModel, List<String> possibleValues) {
            super(nodeModel);
            this.possibleValues = possibleValues;

            // set up property descriptors
            extendsProp = new EditableComboBoxPropertyDescriptor(Prop.Extends, "extends");
            extendsProp.setCategory(MergedPropertySource.BASE_CATEGORY);
            extendsProp.setValidator(new ExistingTypeNameValidator(possibleValues));
            extendsProp.setDescription("Which component is extended by this component");

            descriptors = new IPropertyDescriptor[] { extendsProp };
        }

        public Object getEditableValue() {
            return getNedTypeModel().getFirstExtends();
        }

        public IPropertyDescriptor[] getPropertyDescriptors() {
            //fill the connection combobox with types
            extendsProp.setItems(possibleValues);
            return descriptors;
        }

        public Object getPropertyValue(Object propName) {
            if (Prop.Extends.equals(propName)) {
                String result = lookupFullyQualifiedName(getNedTypeModel().getEnclosingLookupContext(), getNedTypeModel().getFirstExtends());
                return convertQNameToDisplayName(result);
            }
            return null;
        }

        public void setPropertyValue(Object propName, Object value) {
            if (Prop.Extends.equals(propName))
                getNedTypeModel().setFirstExtends(NedElementUtilEx.friendlyTypeNameToQName((String)value));

            NedElementUtilEx.addImportForExtends(getNedTypeModel()); // note: overwrites the first extends
            // note that this will add an import statement if needed, but WILL not remove when undo is invoked
            // there is no way to distinguish here between do and undo operations
        }

        public boolean isPropertySet(Object propName) {
            if (Prop.Extends.equals(propName))
                return StringUtils.isNotEmpty(getNedTypeModel().getFirstExtends());

            return false;
        }

        public void resetPropertyValue(Object propName) {
            if (Prop.Extends.equals(propName))
                getNedTypeModel().setFirstExtends(null);
        }

        public boolean isPropertyResettable(Object propName) {
            return true;
        }

        public INedTypeElement getNedTypeModel() {
            return (INedTypeElement)getModel();
        }
}
