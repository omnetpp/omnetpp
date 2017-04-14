/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import org.eclipse.jface.viewers.IStructuredSelection;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.editors.ui.ScaveEditorPage;

/**
 * Close the current editor page.
 *
 * @author andras
 */
public class ClosePageAction extends AbstractScaveAction {

    public ClosePageAction() {
        setText("Close");
        setDescription("Close current page");
        setImageDescriptor(ScavePlugin.getImageDescriptor("icons/full/etool16/close.png"));
    }

    @Override
    protected void doRun(ScaveEditor scaveEditor, IStructuredSelection selection) {
        ScaveEditorPage page = scaveEditor.getActiveEditorPage();
        scaveEditor.removePage(page);
    }

    @Override
    protected boolean isApplicable(ScaveEditor editor, IStructuredSelection selection) {
        ScaveEditorPage page = editor.getActiveEditorPage();
        return editor.isClosablePage(page);
    }
}
