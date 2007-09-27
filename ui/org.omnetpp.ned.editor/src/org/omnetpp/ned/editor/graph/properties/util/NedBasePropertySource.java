package org.omnetpp.ned.editor.graph.properties.util;

import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.IPropertySource2;

import org.omnetpp.ned.model.INEDElement;

/**
 * A property source that is the base of all ned element related property sources
 *
 * @author rhornig
 */
abstract public class NedBasePropertySource implements IPropertySource2 {
    private INEDElement nedModel;

    public NedBasePropertySource(INEDElement model) {
        nedModel = model;
    }

    public Object getEditableValue() {
        return "";
    }

    public Object getPropertyValue(Object id) {
        return "";
    }
    
    public boolean isPropertyResettable(Object id) {
        return false;
    }

    public boolean isPropertySet(Object id) {
        return false;
    }

    public void resetPropertyValue(Object id) {
    }

    public void setPropertyValue(Object id, Object value) {
    }

    public abstract IPropertyDescriptor[] getPropertyDescriptors();

    INEDElement getModel() {
        return nedModel;
    }
}
