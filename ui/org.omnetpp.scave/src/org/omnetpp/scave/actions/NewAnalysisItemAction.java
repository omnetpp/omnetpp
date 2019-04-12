/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import org.eclipse.jface.viewers.IStructuredSelection;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.model.AnalysisItem;
import org.omnetpp.scave.model.commands.AddChartCommand;
import org.omnetpp.scave.model.commands.ICommand;

/**
 * Creates a new chart, chart sheet, or other analysis item.
 */
public class NewAnalysisItemAction extends AbstractScaveAction {
    private AnalysisItem elementPrototype;

    public NewAnalysisItemAction(AnalysisItem elementPrototype) {
        this.elementPrototype = elementPrototype;
    }

    @Override
    protected void doRun(ScaveEditor editor, IStructuredSelection selection) {
        AnalysisItem element = elementPrototype;

        ICommand command = new AddChartCommand(editor.getAnalysis(), (AnalysisItem)elementPrototype.pubClone());

        editor.executeCommand(command);
        editor.showAnalysisItem(element);
    }

    @Override
    protected boolean isApplicable(ScaveEditor editor, IStructuredSelection selection) {
        return true;
    }

}
