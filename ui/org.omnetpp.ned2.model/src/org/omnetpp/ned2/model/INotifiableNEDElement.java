package org.omnetpp.ned2.model;


/**
 * An object that can fire NED change notifications
 * @author andras
 */
public interface INotifiableNEDElement {

    /**
     * Invoked when an attribute changes value. 
     * The new value can be obtained using getAttribute(attr). 
     */
    public void fireAttributeChanged(NEDElement node, String attr);

    /**
     * Invoked when a child element gets added before
     * the "where" element, or appeded at the end if where==null. 
     */
	public void fireChildInserted(NEDElement node, NEDElement where, NEDElement child);
    
    /**
     * Invoked when a child element gets removed 
     */
    public void fireChildRemoved(NEDElement node, NEDElement child);
}
