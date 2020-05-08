/*--------------------------------------------------------------*
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import org.eclipse.core.runtime.CoreException;
import org.eclipse.jface.viewers.ISelection;
import org.omnetpp.scave.ScaveImages;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.editors.ChartScriptEditor;
import org.omnetpp.scave.editors.ScaveEditor;

/**
 * Close the current editor page.
 *
 * @author andras
 */
public class ClosePageAction extends AbstractScaveAction {

    public ClosePageAction() {
        setText("Close");
        setDescription("Close current page");
        setImageDescriptor(ScavePlugin.getImageDescriptor(ScaveImages.IMG_ETOOL16_CLOSE));
    }

    @Override
    protected void doRun(ScaveEditor scaveEditor, ISelection selection) throws CoreException {
        int page = scaveEditor.getActivePage();
        scaveEditor.saveState();
        ChartScriptEditor editor = scaveEditor.getActiveChartScriptEditor();
        if (scaveEditor.askToKeepEditedTemporaryChart(editor)) {
            scaveEditor.applyChartEdits(editor);
            scaveEditor.removePage(page);
        }
    }

    @Override
    protected boolean isApplicable(ScaveEditor editor, ISelection selection) {
        return editor.isClosablePage(editor.getActivePage());
    }
}
