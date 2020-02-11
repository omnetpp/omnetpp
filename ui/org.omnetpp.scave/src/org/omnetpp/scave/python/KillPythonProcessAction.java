package org.omnetpp.scave.python;

import org.eclipse.jface.viewers.ISelection;
import org.eclipse.ui.ISharedImages;
import org.eclipse.ui.PlatformUI;
import org.omnetpp.scave.actions.AbstractScaveAction;
import org.omnetpp.scave.editors.ChartScriptEditor;
import org.omnetpp.scave.editors.ScaveEditor;

public class KillPythonProcessAction extends AbstractScaveAction {

    public KillPythonProcessAction() {
        setText("Kill the Python Process of the Chart");
        setImageDescriptor(PlatformUI.getWorkbench().getSharedImages().getImageDescriptor(ISharedImages.IMG_ELCL_STOP));
    }

    @Override
    protected void doRun(ScaveEditor scaveEditor, ISelection selection) {
        ChartScriptEditor editor = scaveEditor.getActiveChartScriptEditor();
        editor.killPythonProcess();
    }

    @Override
    protected boolean isApplicable(ScaveEditor scaveEditor, ISelection selection) {
        ChartScriptEditor editor = scaveEditor.getActiveChartScriptEditor();
        return editor != null && editor.isPythonProcessAlive();
    }
}