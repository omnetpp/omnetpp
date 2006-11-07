package org.omnetpp.ned.editor.graph.properties.util;

import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.IPropertySource2;
import org.eclipse.ui.views.properties.PropertyDescriptor;

/**
 * @author rhornig
 * A property source that delagates all functions to an other except provides its own
 * descirptor, and as a value provides the other property source
 */
public class DelegatingPropertySource implements IPropertySource2 {
        public enum Prop { ID };
        protected IPropertyDescriptor[] descriptors;
        protected IPropertySource2 delegateTo;

        public DelegatingPropertySource(IPropertySource2 delegateTo, String name, String descr) {
            this.delegateTo = delegateTo;
            

            PropertyDescriptor propDesc = new PropertyDescriptor(this, name);
            propDesc.setCategory(MergedPropertySource.BASE_CATEGORY);
            propDesc.setDescription(descr);

            descriptors = new IPropertyDescriptor[] { propDesc };
        }

        public Object getEditableValue() {
            return null;
        }

        public IPropertyDescriptor[] getPropertyDescriptors() {
            return descriptors;
        }

        public Object getPropertyValue(Object propName) {
            if (this.equals(propName))
                return delegateTo;
            
            return null;
        }

        public void setPropertyValue(Object propName, Object value) {
        }

        public boolean isPropertySet(Object propName) {
            return true;
        }

        public void resetPropertyValue(Object propName) {
        }

        public boolean isPropertyResettable(Object propName) {
            return false;
        }
}
