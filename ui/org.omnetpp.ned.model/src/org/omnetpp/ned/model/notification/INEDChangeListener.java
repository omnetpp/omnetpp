package org.omnetpp.ned.model.notification;

/**
 * Implement this to get notified about changes in a NED tree.
 * @author andras
 */
public interface INEDChangeListener {

    /**
     * Callback invoked when a change occurs in the model.
     */
    void modelChanged(NEDModelEvent event);
    
}
