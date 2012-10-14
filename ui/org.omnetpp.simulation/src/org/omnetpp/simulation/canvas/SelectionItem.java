package org.omnetpp.simulation.canvas;

import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.IAdaptable;
import org.omnetpp.simulation.inspectors.IInspectorPart;

/**
 * TODO
 *
 * @author Andras
 */
public class SelectionItem implements IAdaptable {
    private IInspectorPart inspector;
    private Object object;  // object within the inspector

    public SelectionItem(IInspectorPart inspector, Object object) {
        Assert.isTrue(inspector != null && object != null);
        this.inspector = inspector;
        this.object = object;
    }

    public IInspectorPart getInspector() {
        return inspector;
    }

    public Object getObject() {
        return object;
    }

    @Override
    @SuppressWarnings("rawtypes")
    public Object getAdapter(Class adapter) {
        if (adapter.isInstance(object))
            return object;
        return null;
    }

    @Override
    public int hashCode() {
        return inspector.hashCode() + 31*object.hashCode();
    }

    @Override
    public boolean equals(Object o) {
        if (this == o)
            return true;
        if (o == null || getClass() != o.getClass())
            return false;
        SelectionItem other = (SelectionItem) o;
        if (inspector != other.inspector && !inspector.equals(other.inspector))
            return false;
        if (object != other.object && !object.equals(other.object))
            return false;
        return true;
    }
}