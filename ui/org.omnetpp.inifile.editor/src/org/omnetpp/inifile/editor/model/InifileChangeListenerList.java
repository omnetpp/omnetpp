/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.inifile.editor.model;

public class InifileChangeListenerList {

    protected IInifileChangeListener[] array = new IInifileChangeListener[0];
    protected boolean enabled = true;

    /**
     * @param listener Adds a new model change listener
     */
    public void add(IInifileChangeListener listener) {
        array = copyArray(array.length+1);
        array[array.length-1] = listener;
    }

    /**
     * @param listener Removes a listener from the list
     */
    public void remove(IInifileChangeListener listener) {
        for (int i=0; i<array.length; i++) {
            if (array[i]==listener) {
                array[i] = array[array.length-1];
                array = copyArray(array.length-1);
                return;
            }
        }
    }

    private IInifileChangeListener[] copyArray(int size) {
        IInifileChangeListener[] newArray = new IInifileChangeListener[size];
        System.arraycopy(array, 0, newArray, 0, Math.min(array.length, size));
        return newArray;
    }

    /**
     * Returns a copy of the listener list, so adding/removing listeners during
     * notification is allowed.
     */
    public IInifileChangeListener[] getListeners() {
        // make a copy, just in case there are adds/removes during iteration
        // Maybe a copy on/write implementation would be more efficient
        IInifileChangeListener[] newArray = new IInifileChangeListener[array.length];
        System.arraycopy(array, 0, newArray, 0, array.length);
        return newArray;
    }

    /**
     * Check whether change notification is enabled
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
     * Fires a model change event to all listeners if event sending is enebaled
     * @param event
     */
    public void fireModelChanged() {
        if (enabled)
            for (IInifileChangeListener listener : getListeners())
                listener.modelChanged();
    }
}
