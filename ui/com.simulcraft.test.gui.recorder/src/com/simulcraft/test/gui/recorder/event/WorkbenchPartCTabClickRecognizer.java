package com.simulcraft.test.gui.recorder.event;

import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.CTabFolder;
import org.eclipse.swt.custom.CTabItem;
import org.eclipse.swt.widgets.Event;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PlatformUI;

import com.simulcraft.test.gui.recorder.GUIRecorder;
import com.simulcraft.test.gui.recorder.JavaSequence;
import com.simulcraft.test.gui.recorder.object.WorkbenchPartCTabRecognizer;

public class WorkbenchPartCTabClickRecognizer extends EventRecognizer {
    public WorkbenchPartCTabClickRecognizer(GUIRecorder recorder) {
        super(recorder);
    }

    public JavaSequence recognizeEvent(Event e) {
        if (e.type == SWT.MouseDown && e.button == 1 && e.widget instanceof CTabFolder) {
            Object uiObject = recorder.resolveUIObject(e);
            if (uiObject instanceof CTabItem) {
                CTabItem item = (CTabItem) uiObject;
                IWorkbenchWindow workbenchWindow = PlatformUI.getWorkbench().getActiveWorkbenchWindow();
                if (WorkbenchPartCTabRecognizer.isViewTabItem(item))
                    return makeMethodCall(workbenchWindow, expr("findViewPartByTitle("+quoteLabel(item.getText())+").activateByMouseClick()", 0.9, null));
                if (WorkbenchPartCTabRecognizer.isEditorTabItem(item))
                    return makeMethodCall(workbenchWindow, expr("findEditorPartByTitle("+quoteLabel(item.getText())+").activateByMouseClick()", 0.9, null));
            }
        }
        return null;
    }
}