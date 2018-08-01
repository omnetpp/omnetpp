package org.omnetpp.scave.python;

import org.eclipse.jface.viewers.IStructuredSelection;
import org.omnetpp.scave.ScaveImages;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.actions.AbstractScaveAction;
import org.omnetpp.scave.editors.ChartScriptEditor;
import org.omnetpp.scave.editors.ScaveEditor;

public class ToggleAutoUpdateAction extends AbstractScaveAction {
    public ToggleAutoUpdateAction() {
        super(AS_CHECK_BOX);
        setText("Toggle auto update");
        setDescription("TODO.");
        setImageDescriptor(ScavePlugin.getImageDescriptor(ScaveImages.IMG_ETOOL16_AUTOREFRESH2));
    }

    @Override
    protected void doRun(ScaveEditor scaveEditor, IStructuredSelection selection) {
        ChartScriptEditor editor = scaveEditor.getActiveChartScriptEditor();
        if (editor != null)
            editor.setAutoUpdateEnabled(isChecked());
    }

    @Override
    protected boolean isApplicable(ScaveEditor scaveEditor, IStructuredSelection selection) {
        ChartScriptEditor editor = scaveEditor.getActiveChartScriptEditor();
        return editor != null;
    }
}