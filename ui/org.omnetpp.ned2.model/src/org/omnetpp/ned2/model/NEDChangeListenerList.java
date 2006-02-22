package org.omnetpp.ned2.model;


public class NEDChangeListenerList {

	NEDChangeListener[] array = new NEDChangeListener[0];

	public void add(NEDChangeListener l) {
		array = copyArray(array.length+1);
		array[array.length-1] = l;
	}

	public void remove(NEDChangeListener l) {
		for (int i=0; i<array.length; i++) {
			if (array[i]==l) {
				array[i] = array[array.length-1];
				array = copyArray(array.length-1);
				return;
			}
		}
	}

	private NEDChangeListener[] copyArray(int size) {
		NEDChangeListener[] newArray = new NEDChangeListener[size];
		System.arraycopy(array, 0, newArray, 0, Math.min(array.length, size));
		return newArray;
	}

	public NEDChangeListener[] getListeners() {
		// make a copy, just in case there are adds/removes during iteration
		NEDChangeListener[] newArray = new NEDChangeListener[array.length];
		System.arraycopy(array, 0, newArray, 0, array.length);
		return newArray;
	}
}
