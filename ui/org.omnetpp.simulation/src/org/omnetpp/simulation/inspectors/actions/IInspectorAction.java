package org.omnetpp.simulation.inspectors.actions;

import org.eclipse.jface.action.IAction;
import org.eclipse.jface.viewers.ISelection;
import org.omnetpp.simulation.canvas.IInspectorContainer;
import org.omnetpp.simulation.inspectors.IInspectorPart;


/**
 * Action that can appear in an inspectors' floating toolbar or on a context menu.
 *
 * @author Andras
 */
public interface IInspectorAction extends IAction {

    /**
     * Sets the context for this action. The action should operate on the given inspector.
     */
    void setContext(IInspectorPart inspector);

    /**
     * Sets the context for this action. The action should operate on (or otherwise take as input)
     * the items in the selection. Note that the passed selection is not necessarily the same
     * as the selection of the inspector container, editor or the workbench.
     */
    void setContext(IInspectorContainer container, ISelection selection);

    /**
     * Refreshes the inspector's state
     */
    void update();

}
