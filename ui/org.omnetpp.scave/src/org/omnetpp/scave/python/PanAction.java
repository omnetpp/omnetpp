package org.omnetpp.scave.python;

import org.eclipse.jface.action.IAction;
import org.eclipse.jface.viewers.ISelection;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.scave.actions.AbstractScaveAction;
import org.omnetpp.scave.editors.ChartScriptEditor;
import org.omnetpp.scave.editors.ScaveEditor;

public class PanAction extends AbstractScaveAction {

    public PanAction() {
        super(IAction.AS_RADIO_BUTTON);

        setText("Pan Tool");
        setDescription("Lets you move the chart using the mouse; hold down Ctrl for zooming.");
        setImageDescriptor(ImageFactory.global().getDescriptor(ImageFactory.TOOLBAR_IMAGE_HAND));
    }

    @Override
    protected void doRun(ScaveEditor scaveEditor, ISelection selection) {
        if (isChecked()) {
            ChartScriptEditor editor = scaveEditor.getActiveChartScriptEditor();
            if (editor != null)
                editor.getMatplotlibChartViewer().pan();
        }
    }

    @Override
    protected boolean isApplicable(ScaveEditor scaveEditor, ISelection selection) {
        ChartScriptEditor editor = scaveEditor.getActiveChartScriptEditor();
        return editor != null && editor.isPythonProcessAlive();
    }
}