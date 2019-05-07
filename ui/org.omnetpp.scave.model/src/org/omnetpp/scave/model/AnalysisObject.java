package org.omnetpp.scave.model;

import org.eclipse.core.runtime.ListenerList;

public abstract class AnalysisObject implements Cloneable {
    protected AnalysisObject parent;

    ListenerList<IAnalysisListener> listeners = new ListenerList<IAnalysisListener>();

    public AnalysisObject getParent() {
        return parent;
    }

    public void addListener(IAnalysisListener listener) {
        listeners.add(listener);
    }

    public void removeListener(IAnalysisListener listener) {
        listeners.remove(listener);
    }

    protected void notifyListeners() {
        notifyListeners(new AnalysisEvent(this));
    }

    protected void notifyListeners(AnalysisEvent event) {
        listeners.forEach((l) -> l.analysisChanged(event) );
        if (parent != null)
            parent.notifyListeners(event);
    }

    public AnalysisObject pubClone() {
        try {
            return clone();
        } catch (CloneNotSupportedException e) {
            // this cannot happen
            throw new RuntimeException(e);
        }
    }

    @Override
    protected AnalysisObject clone() throws CloneNotSupportedException {
        AnalysisObject clone = (AnalysisObject)super.clone();
        clone.listeners = new ListenerList<IAnalysisListener>();
        return clone;
    }
}
