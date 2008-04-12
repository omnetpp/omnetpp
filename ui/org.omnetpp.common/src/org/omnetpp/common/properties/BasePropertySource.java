package org.omnetpp.common.properties;

import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.IPropertySource2;

/**
 * Helper class for implementing a property source by giving 
 * default implementations of <code>IPropertySource2</code>.
 *
 * @author tomi
 */
public class BasePropertySource implements IPropertySource2 {
	
	IPropertyDescriptor[] descriptors;
	
	public BasePropertySource() {
		this.descriptors = new IPropertyDescriptor[0];
	}
	
	public BasePropertySource(IPropertyDescriptor[] descriptors) {
		this.descriptors = descriptors;
	}

	public boolean isPropertyResettable(Object id) {
		return false;
	}

	public boolean isPropertySet(Object id) {
		return true;
	}

	public Object getEditableValue() {
		return null;
	}

	public IPropertyDescriptor[] getPropertyDescriptors() {
		return descriptors;
	}

	public Object getPropertyValue(Object id) {
		return null;
	}

	public void resetPropertyValue(Object id) {
	}

	public void setPropertyValue(Object id, Object value) {
	}
}
