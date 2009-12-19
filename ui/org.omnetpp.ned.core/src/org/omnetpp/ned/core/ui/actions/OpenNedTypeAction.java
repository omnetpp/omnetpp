/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.core.ui.actions;

import org.eclipse.jface.action.IAction;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.IWorkbenchWindowActionDelegate;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.dialogs.ListDialog;
import org.omnetpp.ned.core.NEDResourcesPlugin;
import org.omnetpp.ned.core.ui.misc.NedTypeSelectionDialog;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;

/**
 * Action to open a NED type selection dialog.
 *
 * @author andras, rhornig
 */
public class OpenNedTypeAction implements IWorkbenchWindowActionDelegate {
    public void init(IWorkbenchWindow window) {
    }

    public void dispose() {
    }

    public void run(IAction action) {
        // pop up a chooser dialog
        IWorkbenchWindow window = PlatformUI.getWorkbench().getActiveWorkbenchWindow();
        NedTypeSelectionDialog dialog = new NedTypeSelectionDialog(window.getShell());
        dialog.setMessage("Select NED type to open:");
        dialog.setTitle("Open NED Type");
        if (dialog.open() == ListDialog.OK) {
            INEDTypeInfo component = dialog.getResult()[0];
            NEDResourcesPlugin.openNEDElementInEditor(component.getNEDElement());
        }
    }

    public void selectionChanged(IAction action, ISelection selection) {
    }

}
