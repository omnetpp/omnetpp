package org.omnetpp.scave.actions;

import org.eclipse.jface.action.IAction;
import org.eclipse.jface.viewers.ISelection;
import org.omnetpp.scave.ScaveImages;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.editors.ScaveEditor;

public class ToggleShowSourceAction extends AbstractScaveAction {

    public ToggleShowSourceAction() {
        super(IAction.AS_CHECK_BOX);
        setText("Show source");
        setDescription("Show chart source");
        setImageDescriptor(ScavePlugin.getImageDescriptor(ScaveImages.IMG_ETOOL16_EDIT));
    }

    @Override
    protected void doRun(ScaveEditor scaveEditor, ISelection selection) {
        scaveEditor.getActiveChartScriptEditor().setShowSource(isChecked());
    }

    @Override
    protected boolean isApplicable(ScaveEditor editor, ISelection selection) {
        return true;
    }

}