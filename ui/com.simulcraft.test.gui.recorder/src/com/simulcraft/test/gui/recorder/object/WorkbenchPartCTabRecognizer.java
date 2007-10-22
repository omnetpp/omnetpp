package com.simulcraft.test.gui.recorder.object;

import org.eclipse.swt.custom.CTabItem;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.internal.EditorPane;
import org.eclipse.ui.internal.PartPane;
import org.eclipse.ui.internal.ViewPane;
import org.eclipse.ui.internal.presentations.PresentablePart;
import org.eclipse.ui.internal.presentations.util.AbstractTabItem;

import com.simulcraft.test.gui.recorder.GUIRecorder;
import com.simulcraft.test.gui.recorder.JavaSequence;

public class WorkbenchPartCTabRecognizer extends ObjectRecognizer {
    public WorkbenchPartCTabRecognizer(GUIRecorder recorder) {
        super(recorder);
    }

    public JavaSequence identifyObject(Object uiObject) {
        if (uiObject instanceof CTabItem) {
            CTabItem item = (CTabItem)uiObject;
            //FIXME check that label uniquely identifies item
            IWorkbenchWindow workbenchWindow = PlatformUI.getWorkbench().getActiveWorkbenchWindow();
            if (isEditorTabItem(item))
                return makeMethodCall(workbenchWindow, expr("findEditorPartByTitle("+quoteLabel(item.getText())+").getCTabItem()", 0.9, item));
            if (isViewTabItem(item))
                return makeMethodCall(workbenchWindow, expr("findViewPartByTitle("+quoteLabel(item.getText())+").getCTabItem()", 0.9, item));
        }
        return null;
    }

    
    @SuppressWarnings("restriction")
    public static boolean isViewTabItem(CTabItem item) {
        return getPresentablePartPane(item) instanceof ViewPane;
    }

    @SuppressWarnings("restriction")
    public static boolean isEditorTabItem(CTabItem item) {
        return getPresentablePartPane(item) instanceof EditorPane;
    }

    @SuppressWarnings("restriction")
    private static PartPane getPresentablePartPane(CTabItem item) {
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