package org.omnetpp.ned.editor.graph.model;

import java.util.Iterator;

import org.omnetpp.ned2.model.NEDElement;



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

	public void fireAttributeChanged(NEDElement node, String attr) {
		for (INEDChangeListener l : getListeners())
			l.attributeChanged(node, attr);
	}

	public void fireChildInserted(NEDElement node, NEDElement where, NEDElement child) {
		for (INEDChangeListener l : getListeners())
			l.childInserted(node, where, child);
	}

	public void fireChildRemoved(NEDElement node, NEDElement child) {
		for (INEDChangeListener l : getListeners())
			l.childRemoved(node, child);
	}
}
