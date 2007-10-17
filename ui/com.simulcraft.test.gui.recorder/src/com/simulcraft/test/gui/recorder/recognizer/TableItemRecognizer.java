package com.simulcraft.test.gui.recorder.recognizer;

import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.TableItem;

import com.simulcraft.test.gui.recorder.GUIRecorder;
import com.simulcraft.test.gui.recorder.JavaExpr;

public class TableItemRecognizer extends Recognizer {
    public TableItemRecognizer(GUIRecorder recorder) {
        super(recorder);
    }

    public JavaExpr identifyControl(Control control, Point point) {
        if (control instanceof Table && point != null) {
            Table table = (Table)control;
            TableItem item = table.getItem(point);
            if (item != null) {
                //FIXME check item label uniquely identifies label
                return chain(recorder.identifyControl(control, null), "findTableItemByContent("+quote(item.getText())+")", 1.1); // to beat TableRecognizer
            }
        }
        return null;
    }

    public JavaExpr recognizeEvent(Event e) {
        if (e.type == SWT.MouseDown)
            return chain(identifyControlIn(e), "click()", 1.0);
        return null;
    }
}