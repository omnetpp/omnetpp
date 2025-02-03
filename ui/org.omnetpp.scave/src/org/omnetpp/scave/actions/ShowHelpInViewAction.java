/*--------------------------------------------------------------*
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import org.eclipse.core.runtime.CoreException;
import org.eclipse.help.ui.internal.views.HelpView;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.ui.ISharedImages;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.PlatformUI;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.editors.ScaveEditor;

/**
 * Shows the Help View in the workspace and opens a documentation section in it.
 */
public class ShowHelpInViewAction extends AbstractScaveAction {
	private static final String HELP_VIEW_ID = "org.eclipse.help.ui.HelpView";
	protected String helpHref;

	public ShowHelpInViewAction(String title, String helpHref) {
        setText(title);
        setToolTipText(title);
        setImageDescriptor(PlatformUI.getWorkbench().getSharedImages().getImageDescriptor(ISharedImages.IMG_LCL_LINKTO_HELP));
        this.helpHref = helpHref;
    }

    @Override
    protected void doRun(ScaveEditor editor, ISelection selection) throws CoreException {
        IWorkbenchWindow window = PlatformUI.getWorkbench().getActiveWorkbenchWindow();
        if (window != null) {
            IWorkbenchPage page = window.getActivePage();
            if (page != null) {
                try {
                    HelpView helpView = (HelpView)page.showView(HELP_VIEW_ID);
                    helpView.showHelp(helpHref);
                } catch (PartInitException e) {
                    ScavePlugin.logError(e);
                }
            }
        }
    }

    @Override
    protected boolean isApplicable(ScaveEditor editor, ISelection selection) {
        return true;
    }
}
