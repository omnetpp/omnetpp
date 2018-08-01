package org.omnetpp.scave.python;

import org.eclipse.jface.action.IAction;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.scave.actions.AbstractScaveAction;
import org.omnetpp.scave.editors.ChartScriptEditor;
import org.omnetpp.scave.editors.ScaveEditor;

public class ZoomAction extends AbstractScaveAction {

    public ZoomAction() {
        super(IAction.AS_RADIO_BUTTON);

        setText("Zoom Tool");
        setDescription(
                "Lets you zoom the chart to a rectangular area. Drag with the left mouse button to zoom in, and with the right to zoom out.");
        setImageDescriptor(ImageFactory.global().getDescriptor(ImageFactory.TOOLBAR_IMAGE_ZOOM));
    }

    @Override
    protected void doRun(ScaveEditor scaveEditor, IStructuredSelection selection) {
        ChartScriptEditor editor = scaveEditor.getActiveChartScriptEditor();
        if (editor != null)
            editor.getMatplotlibChartViewer().zoom();
    }

    @Override
    protected boolean isApplicable(ScaveEditor scaveEditor, IStructuredSelection selection) {
        ChartScriptEditor editor = scaveEditor.getActiveChartScriptEditor();
        return (editor != null) && editor.isPythonProcessAlive();
    }
}