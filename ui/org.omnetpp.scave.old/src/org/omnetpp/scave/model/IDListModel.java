/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.model;

import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;

/**
 * Wraps IDList with change notification capability.
 */
//FIXME move this into the engine plugin, then make SWIG classes unavailable from other packages! --Andras
public class IDListModel {

    private IDList idList = new IDList();

    private ModelChangeListenerList listeners = new ModelChangeListenerList();

    public void addListener(IModelChangeListener l) {
        listeners.add(l);
    }

    public void removeListener(IModelChangeListener l) {
        listeners.remove(l);
    }

    public void add(long x) {
        idList.add(x);
        listeners.notifyListeners();
    }

    public void clear() {
        idList.clear();
        listeners.notifyListeners();
    }

    public IDList dup() {
        return idList.dup();
    }

    /**
     * Returns the internal IDList, which MUST NOT BE CHANGED.
     */
    public IDList get() {
        return idList;
    }

    public boolean equals(Object arg0) {
        return idList.equals(arg0);
    }

    public void erase(int i) {
        idList.erase(i);
        listeners.notifyListeners();
    }

    public long get(int i) {
        return idList.get(i);
    }

    public int hashCode() {
        return idList.hashCode();
    }

    public void intersect(IDList ids) {
        idList.intersect(ids);
    }

    public boolean isEmpty() {
        return idList.isEmpty();
    }

    public void merge(IDList ids) {
        idList.merge(ids);
        listeners.notifyListeners();
    }

    public void set(IDList ids) {
        idList.set(ids);
        listeners.notifyListeners();
    }

    public void set(int i, long x) {
        idList.set(i, x);
        listeners.notifyListeners();
    }

    public long size() {
        return idList.size();
    }

    public void sortByFileAndRun(ResultFileManager mgr) {
        idList.sortByFileAndRunRef(mgr);
        listeners.notifyListeners();
    }

    public void substract(IDList ids) {
        idList.substract(ids);
        listeners.notifyListeners();
    }

    public void substract(long x) {
        idList.substract(x);
        listeners.notifyListeners();
    }

    public Long[] toArray() {
        return idList.toArray();
    }

    public String toString() {
        return idList.toString();
    }

    public boolean areAllScalars() {
        return idList.areAllScalars();
    }

    public boolean areAllVectors() {
        return idList.areAllVectors();
    }

    public IDList getSubsetByIndices(int[] array) {
        return idList.getSubsetByIndices(array);
    }

    public int itemTypes() {
        return idList.itemTypes();
    }

    public void sortByDirectory(ResultFileManager mgr, boolean ascending) {
        idList.sortByDirectory(mgr, ascending);
        listeners.notifyListeners();
    }

    public void sortByFileName(ResultFileManager mgr, boolean ascending) {
        idList.sortByFileName(mgr, ascending);
        listeners.notifyListeners();
    }

    public void sortByModule(ResultFileManager mgr, boolean ascending) {
        idList.sortByModule(mgr, ascending);
        listeners.notifyListeners();
    }

    public void sortByName(ResultFileManager mgr, boolean ascending) {
        idList.sortByName(mgr, ascending);
        listeners.notifyListeners();
    }

    public void sortByRun(ResultFileManager mgr, boolean ascending) {
        idList.sortByRun(mgr, ascending);
        listeners.notifyListeners();
    }

    public void sortScalarsByValue(ResultFileManager mgr, boolean ascending) {
        idList.sortScalarsByValue(mgr, ascending);
        listeners.notifyListeners();
    }

    public void sortVectorsByLength(ResultFileManager mgr, boolean ascending) {
        idList.sortVectorsByLength(mgr, ascending);
        listeners.notifyListeners();
    }

    public void sortVectorsByMean(ResultFileManager mgr, boolean ascending) {
        idList.sortVectorsByMean(mgr, ascending);
        listeners.notifyListeners();
    }

    public void sortVectorsByStdDev(ResultFileManager mgr, boolean ascending) {
        idList.sortVectorsByStdDev(mgr, ascending);
        listeners.notifyListeners();
    }
}
