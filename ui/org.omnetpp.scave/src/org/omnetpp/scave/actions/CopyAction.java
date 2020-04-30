/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import org.eclipse.core.runtime.CoreException;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.swt.dnd.Clipboard;
import org.eclipse.swt.dnd.Transfer;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.ISharedImages;
import org.omnetpp.common.ui.LocalTransfer;
import org.omnetpp.common.ui.TimeTriggeredProgressMonitorDialog2;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.editors.IDListSelection;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.editors.datatable.FilteredDataPanel;
import org.omnetpp.scave.editors.ui.ChartPage;
import org.omnetpp.scave.editors.ui.FormEditorPage;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.ModelObject;

/**
 * Copy model objects to the clipboard.
 */
public class CopyAction extends AbstractScaveAction {
    public CopyAction() {
        setText("Copy");
        setImageDescriptor(ScavePlugin.getSharedImageDescriptor(ISharedImages.IMG_TOOL_COPY));
    }

    @Override
    protected void doRun(ScaveEditor editor, ISelection selection) throws CoreException {

        if (selection instanceof IDListSelection) {
            FilteredDataPanel activePanel = editor.getBrowseDataPage().getActivePanel();
            if (activePanel != null) {
                TimeTriggeredProgressMonitorDialog2.runWithDialogInUIThread("Copying to clipboard",
                        (monitor) -> activePanel.getDataControl().copySelectionToClipboard(monitor));
            }
        }
        else {
            Object[] objects = asStructuredOrEmpty(selection).toArray();
            for (int i = 0; i < objects.length; ++i) {
                if (objects[i] instanceof ModelObject) {
                    ModelObject origObject = (ModelObject)objects[i];
                    objects[i] = origObject.dup();

                    // For Charts that are open in editors, the editor document contents
                    // authoritative over the script in the data model, so let's use that instead.
                    if (objects[i] instanceof Chart) {
                        Chart chart = (Chart)objects[i];
                        FormEditorPage page = editor.getEditorPage((Chart)origObject);
                        if (page != null)
                            chart.setScript(((ChartPage)page).getChartScriptEditor().getDocument().get());
                    }
                }
            }
            // TODO filter out non-AnalysisObject objects
            Clipboard clipboard = new Clipboard(Display.getCurrent());
            clipboard.setContents(new Object[] { objects }, new Transfer[] {LocalTransfer.getInstance()});
            clipboard.dispose();
        }
    }

    @Override
    protected boolean isApplicable(ScaveEditor editor, ISelection selection) {
        return !selection.isEmpty(); // TODO check if there are non-AnalysisObject objects in the selection
    }
}
