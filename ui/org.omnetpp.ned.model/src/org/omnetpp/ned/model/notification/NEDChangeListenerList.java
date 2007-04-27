package org.omnetpp.ned.model.notification;


/**
 * Supports a listener list used for change and structural notification in a INEDElement tree
 * @author rhornig
 */
public class NEDChangeListenerList {

	protected INEDChangeListener[] array = new INEDChangeListener[0];
    protected boolean enabled = true;

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

	/**
	 * @return The listener list's copy so adding/removing listeners during
     * notification is allowed.
	 */
	public INEDChangeListener[] getListeners() {
		// make a copy, just in case there are adds/removes during iteration
        // Maybe a copy on/write implementation would be more efficient
		INEDChangeListener[] newArray = new INEDChangeListener[array.length];
		System.arraycopy(array, 0, newArray, 0, array.length);
		return newArray;
	}

    /**
     * Check whether change notification is enabled
     * @return
     */
    public boolean isEnabled() {
        return enabled;
    }

    /**
     * Enable or disable change notification
     * @param notifyEnabled The new state requested.
     * @return The original notification state before the call.
     */
    public boolean setEnabled(boolean notifyEnabled) {
        boolean oldState = this.enabled;
        this.enabled = notifyEnabled;
        return oldState;
    }

    /**
     * Fires a model change event to all listeners if event sending is enabled
     * @param event
     */
    public void fireModelChanged(NEDModelEvent event) {
        if (enabled)
            for (INEDChangeListener listener : getListeners())
                listener.modelChanged(event);
    }
}
