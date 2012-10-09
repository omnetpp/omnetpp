package org.omnetpp.simulation.inspectors.actions;

import org.eclipse.jface.action.IAction;
import org.omnetpp.simulation.inspectors.IInspectorPart;


/**
 * Action that can appear in an inspectors' floating toolbar or context menu.
 *
 * @author Andras
 */
public interface IInspectorAction extends IAction {

    /**
     * Sets the inspector associated with this action.
     */
    void setInspectorPart(IInspectorPart inspector);

    /**
     * Refreshes the inspector's state
     */
    void update();
}
