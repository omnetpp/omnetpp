package org.omnetpp.ned2.model.notification;


/**
 * @author rhornig
 * Supports a listener list used for change and structural notification in a NEDElement tree
 */
public class NEDChangeListenerList {

	INEDChangeListener[] array = new INEDChangeListener[0];

	/**
	 * @param listener Adds a new model change listener
	 */
	public void add(INEDChangeListener listener) {
		array = copyArray(array.length+1);
		array[array.length-1] = listener;
	}

	/**
	 * @param listener Removes a listener from the list
	 */
	public void remove(INEDChangeListener listener) {
		for (int i=0; i<array.length; i++) {
			if (array[i]==listener) {
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

    public void fireModelChanged(NEDModelEvent event) {
        for (INEDChangeListener l : getListeners())
            l.modelChanged(event);
    }
}
