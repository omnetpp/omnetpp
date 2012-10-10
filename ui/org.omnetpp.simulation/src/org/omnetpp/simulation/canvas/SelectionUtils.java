package org.omnetpp.simulation.canvas;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.runtime.IAdaptable;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.omnetpp.simulation.inspectors.IInspectorPart;

/**
 * Static utility methods for extracting objects from an IStructuredSelection
 *
 * @author Andras
 */
public class SelectionUtils {

    /**
     * Utility function: extracts cObjects from the selection and returns them.
     * Tries both instanceof and getAdapter() on elements of the selection.
     */
    public static<T> List<T> getObjects(ISelection selection, Class<T> clazz) {
        return getObjects(selection, null, clazz);
    }

    @SuppressWarnings("unchecked")
    public static<T> List<T> getObjects(ISelection selection, IInspectorPart ownerInspector, Class<T> clazz) {
        List<T> result = new ArrayList<T>();
        if (!(selection instanceof IStructuredSelection))
            return result;
        for (Object o : ((IStructuredSelection)selection).toList()) {
            if (ownerInspector == null || belongsToInspector(o, ownerInspector)) {
                if (clazz.isInstance(o))
                    result.add((T)o);
                else if (o instanceof IAdaptable) {
                    Object obj = ((IAdaptable) o).getAdapter(clazz);
                    if (obj != null)
                        result.add((T)obj);
                }
            }
        }
        return result;
    }

    @SuppressWarnings("unchecked")
    public static<T> List<T> getObjects(Object[] elements, Class<T> clazz) {
        List<T> result = new ArrayList<T>();
        for (Object o : elements) {
            if (clazz.isInstance(o))
                result.add((T)o);
            else if (o instanceof IAdaptable) {
                Object obj = ((IAdaptable) o).getAdapter(clazz);
                if (obj != null)
                    result.add((T)obj);
            }
        }
        return result;
    }

    public static boolean belongsToInspector(Object o, IInspectorPart ownerInspector) {
        return o instanceof SelectionItem && ((SelectionItem)o).getInspector() == ownerInspector;  //TODO getInspector should be in some interface (IOwnedByInspector?)
    }

    public static boolean contains(ISelection selection, Object item) {
        if (!(selection instanceof IStructuredSelection))
            return false;
        return ((IStructuredSelection)selection).toList().contains(item);
    }
}