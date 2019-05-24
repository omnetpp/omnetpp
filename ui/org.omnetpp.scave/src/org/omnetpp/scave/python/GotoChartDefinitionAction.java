package org.omnetpp.scave.python;

import org.eclipse.jface.viewers.ISelection;
import org.omnetpp.scave.ScaveImages;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.actions.AbstractScaveAction;
import org.omnetpp.scave.editors.ScaveEditor;

public class GotoChartDefinitionAction extends AbstractScaveAction {

    public GotoChartDefinitionAction() {
        setText("Go to Chart Definition");
        setDescription("TODO");
        setImageDescriptor(ScavePlugin.getImageDescriptor(ScaveImages.IMG_ETOOL16_GOTOITEMDEFINITION));
    }

    @Override
    protected void doRun(ScaveEditor scaveEditor, ISelection selection) {
        scaveEditor.showAnalysisItem(scaveEditor.getActiveChartScriptEditor().getChart());
    }

    @Override
    protected boolean isApplicable(ScaveEditor editor, ISelection selection) {
        return true;
    }
}