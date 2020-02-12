/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import org.eclipse.core.runtime.CoreException;
import org.eclipse.jface.viewers.ISelection;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.editors.ui.ChartsPage;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model2.ScaveModelUtil;

/**
 * Initiates the renaming of the selected chart.
 */
public class RenameChartAction extends AbstractScaveAction {

    public RenameChartAction() {
        setText("Rename");
        setDescription("Initiates the renaming of the selected chart");
    }

    @Override
    protected void doRun(ScaveEditor scaveEditor, ISelection selection) throws CoreException {
        Chart chart = ScaveModelUtil.getChartFromSingleSelection(selection);
        ChartsPage chartsPage = scaveEditor.getChartsPage();
        chartsPage.getViewer().startDirectRename(chart);
    }

    @Override
    protected boolean isApplicable(ScaveEditor editor, ISelection selection) {
        return editor.getActiveEditorPage() == editor.getChartsPage()
                && ScaveModelUtil.getChartFromSingleSelection(selection) != null;
    }

}
