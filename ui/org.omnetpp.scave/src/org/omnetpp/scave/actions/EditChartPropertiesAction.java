/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.omnetpp.scave.ScaveImages;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.editors.ChartScriptEditor;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.editors.ui.EditChartDialog;
import org.omnetpp.scave.model.Chart;

/**
 * Opens an edit dialog for the selected chart
 */
public class EditChartPropertiesAction extends AbstractScaveAction {
    /**
     * Creates the action with an default title and icon, and without parameters.
     */
    public EditChartPropertiesAction() {
        setText("Edit properties");
        setToolTipText("Edit Chart Properties");
        setImageDescriptor(ScavePlugin.getImageDescriptor(ScaveImages.IMG_ETOOL16_PROPERTIES));
    }

    @Override
    protected void doRun(final ScaveEditor scaveEditor, ISelection selection) {
        if (isApplicable(scaveEditor, selection)) {
            Chart editedChart = getEditedChart(scaveEditor, selection);
            EditChartDialog dialog = new EditChartDialog(scaveEditor.getSite().getShell(), editedChart, scaveEditor);
            dialog.open();
        }
    }

    @Override
    public boolean isApplicable(final ScaveEditor editor, ISelection selection) {
        return getEditedChart(editor, selection) != null;
    }

    private Chart getEditedChart(ScaveEditor editor, ISelection selection) {
        ChartScriptEditor activeChartScriptEditor = editor.getActiveChartScriptEditor();
        if (activeChartScriptEditor != null)
            return activeChartScriptEditor.getChart();

        IStructuredSelection structuredSelection = asStructuredOrEmpty(selection);
        if (structuredSelection.size() == 1 && structuredSelection.getFirstElement() instanceof Chart)
            return (Chart)structuredSelection.getFirstElement();
        return null;
    }
}
