package org.omnetpp.ned.editor.graph.misc;

import org.eclipse.gef.EditPart;
import org.eclipse.gef.EditPartViewer;
import org.eclipse.gef.ui.parts.SelectionSynchronizer;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.interfaces.IModelProvider;

/**
 * Special syncronizer that syncs any selected editpart to the first editpart wich is found among
 * the ancestors. Returns null if no suitable editpart found in the viewer
 *
 * @author rhornig
 */
public class NedSelectionSynchronizer extends SelectionSynchronizer {

    /**
     * Maps the given editpart from one viewer to an editpart in another viewer.
     * It returns null if there is no corresponding part. Travels along the ancestors
     * towards root, and returns the first matching ancestor.
     *
     * @param viewer the viewer being mapped to
     * @param part a part from another viewer
     * @return <code>null</code>, or a corresponding editpart
     */
    @Override
    protected EditPart convert(EditPartViewer viewer, EditPart part) {
        if (part instanceof IModelProvider) {
            INEDElement model = ((IModelProvider)part).getNedModel();
            while (model != null) {
                EditPart ep = (EditPart)viewer.getEditPartRegistry().get(model);
                if (ep != null)
                    return ep;
                // not matching, check the parent too
                model = model.getParent();
            }
        }
        return null;
    }
}
