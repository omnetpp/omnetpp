package org.omnetpp.scave.model;

import org.eclipse.core.runtime.ListenerList;

public abstract class ModelObject implements Cloneable {
    protected ModelObject parent;

    ListenerList<IAnalysisListener> listeners = new ListenerList<IAnalysisListener>();

    public ModelObject getParent() {
        return parent;
    }

    public void addListener(IAnalysisListener listener) {
        listeners.add(listener);
    }

    public void removeListener(IAnalysisListener listener) {
        listeners.remove(listener);
    }

    protected void notifyListeners() {
        notifyListeners(new ModelChangeEvent(this));
    }

    protected void notifyListeners(ModelChangeEvent event) {
        listeners.forEach((l) -> l.analysisChanged(event) );
        if (parent != null)
            parent.notifyListeners(event);
    }

    public ModelObject dup() {
        try {
            return clone();
        } catch (CloneNotSupportedException e) {
            // this cannot happen
            throw new RuntimeException(e);
        }
    }

    @Override
    protected ModelObject clone() throws CloneNotSupportedException {
        ModelObject clone = (ModelObject)super.clone();
        clone.listeners = new ListenerList<IAnalysisListener>();
        return clone;
    }
}
