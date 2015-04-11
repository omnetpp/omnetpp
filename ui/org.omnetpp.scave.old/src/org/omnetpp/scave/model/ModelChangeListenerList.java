/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.model;


public class ModelChangeListenerList {

    IModelChangeListener[] array = new IModelChangeListener[0];

    public void notifyListeners() {
        // make a copy, in case there're adds/removes during notification
        IModelChangeListener[] tmp = copyArray(array.length);
        for (int i=0; i<tmp.length; i++)
            tmp[i].handleChange();
    }

    public void add(IModelChangeListener l) {
        array = copyArray(array.length+1);
        array[array.length-1] = l;
    }

    public void remove(IModelChangeListener l) {
        for (int i=0; i<array.length; i++) {
            if (array[i]==l) {
                array[i] = array[array.length-1];
                array = copyArray(array.length-1);
                return;
            }
        }
    }

    private IModelChangeListener[] copyArray(int size) {
        IModelChangeListener[] newArray = new IModelChangeListener[size];
        System.arraycopy(array, 0, newArray, 0, Math.min(array.length, size));
        return newArray;
    }
}

