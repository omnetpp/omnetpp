/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.model.AnalysisItem;
import org.omnetpp.scave.model.Chart;

/**
 * Opens an analysis item on the Charts page.
 */
public class GotoItemDefinitionAction extends AbstractScaveAction {
    public GotoItemDefinitionAction() {
    }

    @Override
    protected void doRun(ScaveEditor editor, ISelection selection) {
        AnalysisItem item = getActiveNontemporaryAnalysisItem(editor, selection);
        if (item != null)
        editor.showAnalysisItem(item);
    }

    @Override
    protected boolean isApplicable(ScaveEditor editor, ISelection selection) {
        AnalysisItem item = getActiveNontemporaryAnalysisItem(editor, selection);
        return item != null;
    }

    protected AnalysisItem getActiveNontemporaryAnalysisItem(ScaveEditor editor, ISelection selection) {
        if (!(selection instanceof IStructuredSelection))
            return null;
        IStructuredSelection structuredSelection = (IStructuredSelection)selection;

        AnalysisItem item = null;
        if (structuredSelection.getFirstElement() instanceof AnalysisItem)
            item = (AnalysisItem) structuredSelection.getFirstElement();
        else if (editor.getActiveChartScriptEditor() != null)
            item = editor.getActiveChartScriptEditor().getChart();

        if (item instanceof Chart && ((Chart)item).isTemporary())
            item = null;
        return item;
    }

}
