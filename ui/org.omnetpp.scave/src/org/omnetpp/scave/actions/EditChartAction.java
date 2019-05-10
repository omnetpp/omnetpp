/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import org.eclipse.jface.viewers.IStructuredSelection;
import org.omnetpp.scave.ScaveImages;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.editors.ui.EditChartDialog;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.ModelObject;

/**
 * Opens an edit dialog for the selected chart
 */
public class EditChartAction extends AbstractScaveAction {
    /**
     * Creates the action with an default title and icon, and without parameters.
     */
    public EditChartAction() {
        setText("Edit properties");
        setToolTipText("Edit Chart");
        setImageDescriptor(ScavePlugin.getImageDescriptor(ScaveImages.IMG_ETOOL16_PROPERTIES));
    }

    @Override
    protected void doRun(final ScaveEditor scaveEditor, final IStructuredSelection selection) {
        if (isApplicable(scaveEditor, selection)) {
            Chart editedChart = getEditedChart(scaveEditor, selection);
            EditChartDialog dialog = new EditChartDialog(scaveEditor.getSite().getShell(), editedChart, scaveEditor);
            dialog.open();
        }
    }

    @Override
    public boolean isApplicable(final ScaveEditor editor, final IStructuredSelection selection) {
        ModelObject editedObject = getEditedChart(editor, selection);
        return editedObject instanceof Chart;
    }

    private Chart getEditedChart(ScaveEditor editor, IStructuredSelection selection) {
        if (selection.size() == 1 && selection.getFirstElement() instanceof Chart)
            return (Chart)selection.getFirstElement();
        return null;
    }
}
