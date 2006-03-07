package org.omnetpp.ned.editor.graph.properties;

import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.IPropertySource2;
import org.omnetpp.ned2.model.INEDChangeListener;
import org.omnetpp.ned2.model.NEDElement;

abstract public class AbstractNedPropertySource 
                            implements IPropertySource2, INEDChangeListener {
    
    NEDElement nedModel;
    
    AbstractNedPropertySource(NEDElement model) {
        nedModel = model;
        // register the propertysource as a listenr for the model so it will be notified
        // once someone chanes the underlying model
        nedModel.addListener(this);
    }

    // the following methods noify the 
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
    
    abstract public void modelChanged();

}
