package com.simulcraft.test.gui.recorder.object;

import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.PlatformUI;

import com.simulcraft.test.gui.recorder.GUIRecorder;
import com.simulcraft.test.gui.recorder.JavaSequence;

public class WorkbenchWindowShellRecognizer extends ObjectRecognizer {
    public WorkbenchWindowShellRecognizer(GUIRecorder recorder) {
        super(recorder);
    }

    public JavaSequence identifyObject(Object uiObject) {
        if (uiObject instanceof Shell && uiObject == PlatformUI.getWorkbench().getActiveWorkbenchWindow().getShell()) {
            Shell shell = (Shell)uiObject;
            return makeSeq(expr("Access.getWorkbenchWindow().getShell()", 0.5, shell));
        }
        return null;
    }
}