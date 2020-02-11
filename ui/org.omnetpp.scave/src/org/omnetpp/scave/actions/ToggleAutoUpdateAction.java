package org.omnetpp.scave.actions;

import org.eclipse.jface.viewers.ISelection;
import org.omnetpp.scave.ScaveImages;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.editors.ChartScriptEditor;
import org.omnetpp.scave.editors.ScaveEditor;

public class ToggleAutoUpdateAction extends AbstractScaveAction {
    public ToggleAutoUpdateAction() {
        super(AS_CHECK_BOX);
        setText("Auto Update");
        setImageDescriptor(ScavePlugin.getImageDescriptor(ScaveImages.IMG_ETOOL16_AUTOREFRESH2));
    }

    @Override
    protected void doRun(ScaveEditor scaveEditor, ISelection selection) {
        ChartScriptEditor editor = scaveEditor.getActiveChartScriptEditor();
        if (editor != null)
            editor.setAutoUpdateChart(isChecked());
    }

    @Override
    protected boolean isApplicable(ScaveEditor scaveEditor, ISelection selection) {
        ChartScriptEditor editor = scaveEditor.getActiveChartScriptEditor();
        return editor != null;
    }
}