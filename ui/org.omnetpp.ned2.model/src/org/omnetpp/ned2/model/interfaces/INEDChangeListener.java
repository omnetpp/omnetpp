package org.omnetpp.ned2.model.interfaces;

import org.omnetpp.ned2.model.NEDElement;

/**
 * Implement this to get notified about changes in a NED tree.
 * @author andras
 */
public interface INEDChangeListener {

    /**
     * Callback, invoked when an attribute changes value. 
     * The new value can be obtained using getAttribute(attr). 
     */
    public void attributeChanged(NEDElement node, String attr);

    /**
     * Callback, invoked when a child element gets added before
     * the "where" element, or appeded at the end if where==null. 
     */
	public void childInserted(NEDElement node, NEDElement where, NEDElement child);
    
    /**
     * Callback, invoked when a child element gets removed 
     */
    public void childRemoved(NEDElement node, NEDElement child);
}
