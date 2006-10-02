package org.omnetpp.ned.editor.graph.properties;

import org.omnetpp.common.properties.PropertySource;
import org.omnetpp.ned2.model.INEDChangeListener;
import org.omnetpp.ned2.model.NEDElement;

/**
 * @author rhornig
 * A Property source for NED elemnts which can be notified by the model
 */
public abstract class NedPropertySource extends PropertySource implements INEDChangeListener {

	
    public NedPropertySource(NEDElement model) {
    	super();
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
    
    /**
     * Change notification from the supporting model
     */
    public void modelChanged() {
    }
	
}	


