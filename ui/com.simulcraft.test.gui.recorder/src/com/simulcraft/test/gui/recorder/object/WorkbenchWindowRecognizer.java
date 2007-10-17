package com.simulcraft.test.gui.recorder.object;

import java.util.List;

import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.PlatformUI;

import com.simulcraft.test.gui.recorder.GUIRecorder;
import com.simulcraft.test.gui.recorder.JavaExpr;

public class WorkbenchWindowRecognizer extends ObjectRecognizer {
    public WorkbenchWindowRecognizer(GUIRecorder recorder) {
        super(recorder);
    }

    public List<JavaExpr> identifyObject(Object uiObject) {
        if (uiObject instanceof Shell && uiObject == PlatformUI.getWorkbench().getActiveWorkbenchWindow().getShell())
            return wrap("getWorkbenchWindow()", 0.5);
        return null;
    }
}