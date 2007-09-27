package org.omnetpp.ned.editor.graph.properties.util;

import java.util.List;

import org.eclipse.ui.views.properties.IPropertyDescriptor;

import org.omnetpp.common.properties.EditableComboBoxPropertyDescriptor;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.model.interfaces.INedTypeElement;

/**
 * A property source for displaying Extends info (using an editable combobox). The content of the
 * dropdown combo should be provided by overriding the getPossibleValues method.
 *
 * @author rhornig
 */
public abstract class ExtendsPropertySource extends NedBasePropertySource {
        public enum Prop { Extends }
        protected IPropertyDescriptor[] descriptors;
        EditableComboBoxPropertyDescriptor extendsProp;

        public ExtendsPropertySource(final INedTypeElement nodeModel) {
            super(nodeModel);

            // set up property descriptors
            extendsProp = new EditableComboBoxPropertyDescriptor(Prop.Extends, "extends");
            extendsProp.setCategory(MergedPropertySource.BASE_CATEGORY);
            extendsProp.setDescription("Which component is extended by this component");

            descriptors = new IPropertyDescriptor[] { extendsProp };
        }

        public Object getEditableValue() {
            return getNedTypeModel().getFirstExtends();
        }

        public IPropertyDescriptor[] getPropertyDescriptors() {
            //fill the connection combobox with types
            extendsProp.setItems(getPossibleValues());
            return descriptors;
        }

        /**
         * Returns the list of possible values that will be used to fill a dropdown box.
         */
        protected abstract List<String> getPossibleValues();

        public Object getPropertyValue(Object propName) {
            if (Prop.Extends.equals(propName))
                return getNedTypeModel().getFirstExtends();

            return null;
        }

        public void setPropertyValue(Object propName, Object value) {
            if (Prop.Extends.equals(propName))
                getNedTypeModel().setFirstExtends((String)value);
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
