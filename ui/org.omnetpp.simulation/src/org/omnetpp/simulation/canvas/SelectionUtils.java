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
    @SuppressWarnings({ "rawtypes" })
    public static List getObjects(ISelection selection, Class clazz) {
        return getObjects(selection, null, clazz);
    }

    @SuppressWarnings({ "rawtypes", "unchecked" })
    public static List getObjects(ISelection selection, IInspectorPart ownerInspector, Class clazz) {
        List result = new ArrayList();
        if (!(selection instanceof IStructuredSelection))
            return result;
        for (Object o : ((IStructuredSelection)selection).toList()) {
            if (ownerInspector == null || belongsToInspector(o, ownerInspector)) {
                if (clazz.isInstance(o))
                    result.add(o);
                else if (o instanceof IAdaptable) {
                    Object obj = ((IAdaptable) o).getAdapter(clazz);
                    if (obj != null)
                        result.add(obj);
                }
            }
        }
        return result;
    }

    public static boolean belongsToInspector(Object o, IInspectorPart ownerInspector) {
        return o instanceof SelectionItem && ((SelectionItem)o).getInspector() == ownerInspector;  //TODO getInspector should be in some interface (IOwnedByInspector?)
    }

}