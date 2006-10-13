package org.omnetpp.ned2.model;

/**
 * @author rhornig
 * Supports a listener list used for change and strunctural notification in a NEDElement tree
 */
public class NEDChangeListenerList {

	INEDChangeListener[] array = new INEDChangeListener[0];

	public void add(INEDChangeListener l) {
		array = copyArray(array.length+1);
		array[array.length-1] = l;
	}

	public void remove(INEDChangeListener l) {
		for (int i=0; i<array.length; i++) {
			if (array[i]==l) {
				array[i] = array[array.length-1];
				array = copyArray(array.length-1);
				return;
			}
		}
	}

	private INEDChangeListener[] copyArray(int size) {
		INEDChangeListener[] newArray = new INEDChangeListener[size];
		System.arraycopy(array, 0, newArray, 0, Math.min(array.length, size));
		return newArray;
	}

	public INEDChangeListener[] getListeners() {
		// make a copy, just in case there are adds/removes during iteration
		INEDChangeListener[] newArray = new INEDChangeListener[array.length];
		System.arraycopy(array, 0, newArray, 0, array.length);
		return newArray;
	}

	/**
     * Send notification about an attribute change of a node to all locally registered
     * listeners 
	 */
	public void fireAttributeChanged(NEDElement node, String attr) {
		for (INEDChangeListener l : getListeners())
			l.attributeChanged(node, attr);
	}

    /**
     * Send notification about a child insertion into a node to all locally registered
     * listeners 
     */
	public void fireChildInserted(NEDElement node, NEDElement where, NEDElement child) {
		for (INEDChangeListener l : getListeners())
			l.childInserted(node, where, child);
	}

    /**
     * Send notification about a child removal from a node to all locally registered
     * listeners 
     */
	public void fireChildRemoved(NEDElement node, NEDElement child) {
		for (INEDChangeListener l : getListeners())
			l.childRemoved(node, child);
	}
}
