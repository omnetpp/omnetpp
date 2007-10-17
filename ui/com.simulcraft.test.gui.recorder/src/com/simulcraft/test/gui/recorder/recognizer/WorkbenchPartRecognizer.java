package com.simulcraft.test.gui.recorder.recognizer;

import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.CTabFolder;
import org.eclipse.swt.custom.CTabItem;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Event;
import org.eclipse.ui.internal.EditorPane;
import org.eclipse.ui.internal.PartPane;
import org.eclipse.ui.internal.ViewPane;
import org.eclipse.ui.internal.presentations.PresentablePart;
import org.eclipse.ui.internal.presentations.util.AbstractTabItem;

import com.simulcraft.test.gui.recorder.GUIRecorder;
import com.simulcraft.test.gui.recorder.JavaExpr;

public class WorkbenchPartRecognizer extends Recognizer {
    public WorkbenchPartRecognizer(GUIRecorder recorder) {
        super(recorder);
    }

    public JavaExpr identifyControl(Control control, Point point) {
        if (control instanceof CTabFolder && point != null) {
            CTabFolder tabFolder = (CTabFolder)control;
            CTabItem item = tabFolder.getItem(point);
            if (item != null) {
                //FIXME check that label uniquely identifies item
                if (isEditorTabItem(item))
                    return new JavaExpr("workbenchWindow.findEditorPartByLabel("+quote(item.getText())+")", 0.9);
                if (isViewTabItem(item))
                    return new JavaExpr("workbenchWindow.findViewPartByLabel("+quote(item.getText())+")", 0.9);
            }
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

    public JavaExpr recognizeEvent(Event e) {
        if (e.type == SWT.MouseDown)
            return chain(identifyControlIn(e), "click()", 1.0);
        return null;
    }
}