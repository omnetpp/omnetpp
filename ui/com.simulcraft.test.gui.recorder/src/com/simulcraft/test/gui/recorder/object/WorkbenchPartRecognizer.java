package com.simulcraft.test.gui.recorder.object;

import org.eclipse.swt.custom.CTabItem;
import org.eclipse.ui.internal.EditorPane;
import org.eclipse.ui.internal.PartPane;
import org.eclipse.ui.internal.ViewPane;
import org.eclipse.ui.internal.presentations.PresentablePart;
import org.eclipse.ui.internal.presentations.util.AbstractTabItem;

import com.simulcraft.test.gui.recorder.GUIRecorder;
import com.simulcraft.test.gui.recorder.JavaSequence;

public class WorkbenchPartRecognizer extends ObjectRecognizer {
    public WorkbenchPartRecognizer(GUIRecorder recorder) {
        super(recorder);
    }

    public JavaSequence identifyObject(Object uiObject) {
        if (uiObject instanceof CTabItem) {
            CTabItem item = (CTabItem)uiObject;
            //FIXME check that label uniquely identifies item
            if (isEditorTabItem(item))
                return makeSeq(expr("workbenchWindow.findEditorPartByLabel("+quote(item.getText())+")", 0.9, item));
            if (isViewTabItem(item))
                return makeSeq(expr("workbenchWindow.findViewPartByLabel("+quote(item.getText())+")", 0.9, item));
        }
        return null;
    }

    
    @SuppressWarnings("restriction")
    private boolean isViewTabItem(CTabItem item) {
        return getPresentablePartPane(item) instanceof ViewPane;
    }

    @SuppressWarnings("restriction")
    private boolean isEditorTabItem(CTabItem item) {
        return getPresentablePartPane(item) instanceof EditorPane;
    }

    @SuppressWarnings("restriction")
    private PartPane getPresentablePartPane(CTabItem item) {
        // dirty hack, exploiting workbench internals. to be replaced if something better comes up 
        Object itemData = item.getData();
        if (itemData instanceof AbstractTabItem) {
            Object itemDataData = ((AbstractTabItem)itemData).getData();
            if (itemDataData instanceof PresentablePart)
                return ((PresentablePart)itemDataData).getPane();
        }
        return null;
    }
}