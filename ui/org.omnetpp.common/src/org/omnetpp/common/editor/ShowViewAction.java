/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.editor;

import org.eclipse.jface.action.Action;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.views.IViewDescriptor;
import org.omnetpp.common.CommonPlugin;


/**
 * Action to show a given view.
 * @author Andras
 */
public class ShowViewAction extends Action {
    private String viewID;

    /**
     * A generic view displayer. the action's id is the same as the view id
     * @param viewID to be shown
     */
    public ShowViewAction(String viewID) {
        setId(viewID);
        IViewDescriptor viewDesc = PlatformUI.getWorkbench().getViewRegistry().find(viewID);
        if (viewDesc == null)
            throw new IllegalArgumentException("No such view registered: "+viewID);
        this.viewID = viewID;
        setText("Show " + viewDesc.getLabel());
        setToolTipText("Open the "+viewDesc.getLabel()+" view");
        setImageDescriptor(viewDesc.getImageDescriptor());
    }

    @Override
    public void run() {
        try {
            IWorkbenchPage workbenchPage = PlatformUI.getWorkbench().getActiveWorkbenchWindow().getActivePage();
            workbenchPage.showView(viewID);
        }
        catch (PartInitException e) {
            CommonPlugin.logError(e);
        }
    }
}
