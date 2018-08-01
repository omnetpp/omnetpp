package org.omnetpp.scave.python;

import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.ui.ISharedImages;
import org.eclipse.ui.PlatformUI;
import org.omnetpp.scave.actions.AbstractScaveAction;
import org.omnetpp.scave.editors.ChartScriptEditor;
import org.omnetpp.scave.editors.ScaveEditor;

public class KillPythonProcessAction extends AbstractScaveAction {

    public KillPythonProcessAction() {
        setText("Kill the Python process of the chart");
        setDescription("TODO.");
        // setImageDescriptor(ImageFactory.global().getDescriptor(ImageFactory.TOOLBAR_IMAGE_ZOOMTOFIT));
        setImageDescriptor(
                PlatformUI.getWorkbench().getSharedImages().getImageDescriptor(ISharedImages.IMG_ELCL_STOP));
    }

    @Override
    protected void doRun(ScaveEditor scaveEditor, IStructuredSelection selection) {
        ChartScriptEditor editor = scaveEditor.getActiveChartScriptEditor();
        editor.killPythonProcess();
    }

    @Override
    protected boolean isApplicable(ScaveEditor scaveEditor, IStructuredSelection selection) {
        ChartScriptEditor editor = scaveEditor.getActiveChartScriptEditor();
        return editor != null && editor.isPythonProcessAlive();
    }
}