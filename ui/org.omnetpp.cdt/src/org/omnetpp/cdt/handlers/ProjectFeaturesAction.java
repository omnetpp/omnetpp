package org.omnetpp.cdt.handlers;

import org.eclipse.jface.action.IAction;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.ui.IActionDelegate;

/**
 * Implements the "Project Features" menu item, which opens the Project Properties dialog
 * with the Project Features page.
 *
 * @author Andras
 */
public class ProjectFeaturesAction implements IActionDelegate {
    public void run(IAction action) {
        ProjectFeaturesHandler.execute();
    }

    public void selectionChanged(IAction action, ISelection selection) {
    }
}
