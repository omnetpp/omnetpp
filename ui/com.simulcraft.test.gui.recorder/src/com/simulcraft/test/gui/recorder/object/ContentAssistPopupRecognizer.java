package com.simulcraft.test.gui.recorder.object;

import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Table;

import com.simulcraft.test.gui.recorder.GUIRecorder;
import com.simulcraft.test.gui.recorder.JavaSequence;

public class ContentAssistPopupRecognizer extends ObjectRecognizer {
    
    public ContentAssistPopupRecognizer(GUIRecorder recorder) {
        super(recorder);
    }

    public JavaSequence identifyObject(Object uiObject) {
        if (uiObject instanceof Table && isContentAssist((Table)uiObject)) {
            return makeSeq(expr("Access.findContentAssistPopup()", 0.8, uiObject));
        }
        return null;
    }

    public static boolean isContentAssist(Table table) {
        if (table.getColumnCount() != 0)
            return false;
        Shell shell = null;
        Composite parent = table.getParent();
        Composite grandParent = parent == null ? null : parent.getParent();
        if (parent.getClass() == Shell.class && parent.getChildren().length == 1)
            shell = (Shell)parent;
        if (parent.getClass() == Composite.class && parent.getChildren().length == 1 && 
            grandParent.getClass() == Shell.class && grandParent.getChildren().length == 1)
            shell = (Shell)grandParent;
        return shell != null;
    }
}