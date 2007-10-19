package com.simulcraft.test.gui.recorder.object;

import org.eclipse.core.runtime.Assert;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IViewPart;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PlatformUI;

import com.simulcraft.test.gui.recorder.GUIRecorder;
import com.simulcraft.test.gui.recorder.JavaSequence;


public class WorkbenchPartRecognizer extends ObjectRecognizer {
    public WorkbenchPartRecognizer(GUIRecorder recorder) {
        super(recorder);
    }

    public JavaSequence identifyObject(Object uiObject) {
        if (uiObject instanceof IWorkbenchPart) {
            IWorkbenchPart workbenchPart = (IWorkbenchPart)uiObject;
            IWorkbenchWindow workbenchWindow = PlatformUI.getWorkbench().getActiveWorkbenchWindow();
            if (workbenchPart instanceof IEditorPart)
                return makeSeq(workbenchWindow, expr("findEditorPartByTitle("+quoteLabel(workbenchPart.getTitle())+")", 0.9, workbenchPart));
            else if (workbenchPart instanceof IViewPart)
                return makeSeq(workbenchWindow, expr("findViewPartByTitle("+quoteLabel(workbenchPart.getTitle())+")", 0.9, workbenchPart));
            else if (workbenchPart != null)
                Assert.isTrue(false, "Unknown workbenchpart: " + workbenchPart);
        }
        return null;
    }

}