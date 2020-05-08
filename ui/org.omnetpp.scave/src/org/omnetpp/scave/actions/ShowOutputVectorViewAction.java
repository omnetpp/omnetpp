/*--------------------------------------------------------------*
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.jface.dialogs.ErrorDialog;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.PlatformUI;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.views.VectorBrowserView;


public class ShowOutputVectorViewAction extends AbstractScaveAction {
    public ShowOutputVectorViewAction() {
        setText("Show Output Vector View");
        setToolTipText("Show Output Vector View");
    }

    @Override
    protected void doRun(ScaveEditor scaveEditor, ISelection selection) throws CoreException {
        try {
            IWorkbenchPage workbenchPage = PlatformUI.getWorkbench().getActiveWorkbenchWindow().getActivePage();
            workbenchPage.showView(VectorBrowserView.ID);
        }
        catch (PartInitException e) {
            IStatus error = ScavePlugin.getErrorStatus(0, "Failed to open Output Vector View", e);
            ErrorDialog dialog = new ErrorDialog(scaveEditor.getSite().getShell(), null, null, error, IStatus.ERROR);
            dialog.open();
        }
    }

    @Override
    protected boolean isApplicable(ScaveEditor editor, ISelection selection) {
        return true;
    }
}
