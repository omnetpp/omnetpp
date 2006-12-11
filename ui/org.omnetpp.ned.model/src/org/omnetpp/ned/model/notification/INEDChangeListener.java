package org.omnetpp.ned.model.notification;

/**
 * Implement this to get notified about changes in a NED tree.
 * @author andras
 */
public interface INEDChangeListener {

    /**
     * @param event Callback invoked when a change occures in the model
     */
    public void modelChanged(NEDModelEvent event);
    
}
