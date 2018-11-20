/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

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
    protected void doRun(ScaveEditor editor, IStructuredSelection selection) {
        AnalysisItem item = getActiveNontemporaryAnalysisItem(editor, selection);
        if (item != null)
        editor.showAnalysisItem(item);
    }

    @Override
    protected boolean isApplicable(ScaveEditor editor, IStructuredSelection selection) {
        AnalysisItem item = getActiveNontemporaryAnalysisItem(editor, selection);
        return item != null;
    }

    protected AnalysisItem getActiveNontemporaryAnalysisItem(ScaveEditor editor, IStructuredSelection selection) {
        AnalysisItem item = null;
        if (selection.getFirstElement() instanceof AnalysisItem)
            item = (AnalysisItem) selection.getFirstElement();
        else if (editor.getActiveChartScriptEditor() != null)
            item = editor.getActiveChartScriptEditor().getChart();

        if (item instanceof Chart && ((Chart)item).isTemporary())
            item = null;
        return item;
    }

}
