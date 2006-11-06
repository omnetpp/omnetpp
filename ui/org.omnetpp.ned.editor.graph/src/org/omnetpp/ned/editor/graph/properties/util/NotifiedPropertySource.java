package org.omnetpp.ned.editor.graph.properties.util;

import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.IPropertySource2;
import org.omnetpp.ned2.model.NEDElement;
import org.omnetpp.ned2.model.interfaces.INEDChangeListener;

/**
 * @author rhornig
 * A property source that is notified of the underlying model changes
 */
abstract public class NotifiedPropertySource 
                            implements IPropertySource2, INEDChangeListener {
    /**
     * Default construtor doesn't register itself as a model listener 
     */
    NotifiedPropertySource() {
    }
    
    NotifiedPropertySource(NEDElement model) {
        // register the propertysource as a listener for the model so it will be notified
        // once someone changes the underlying model
        model.addListener(this);
    }
    
    // the following methods notify the property source 
    public void attributeChanged(NEDElement node, String attr) {
        modelChanged();
    }

    public void childInserted(NEDElement node, NEDElement where, NEDElement child) {
        modelChanged();
    }

    public void childRemoved(NEDElement node, NEDElement child) {
        modelChanged();
    }
    
    abstract public boolean isPropertyResettable(Object id);
    
    abstract public boolean isPropertySet(Object id);

    abstract public Object getEditableValue();

    abstract public IPropertyDescriptor[] getPropertyDescriptors();

    abstract public Object getPropertyValue(Object id);

    abstract public void resetPropertyValue(Object id);
    
    abstract public void setPropertyValue(Object id, Object value);
    
    public void modelChanged() {
    }
}
