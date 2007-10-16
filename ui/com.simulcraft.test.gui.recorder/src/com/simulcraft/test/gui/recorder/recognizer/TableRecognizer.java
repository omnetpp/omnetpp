package com.simulcraft.test.gui.recorder.recognizer;

import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Table;

import com.simulcraft.test.gui.recorder.GUIRecorder;
import com.simulcraft.test.gui.recorder.JavaExpr;

public class TableRecognizer extends Recognizer {
    public TableRecognizer(GUIRecorder recorder) {
        super(recorder);
    }

    public JavaExpr identifyControl(Control control, Point point) {
        if (control instanceof Table) {
            if (findDescendantControl(control.getShell(), Table.class) == control)
                return chain(recorder.identifyControl(control.getShell()), "findTable()", 0.9);
        }
        return null;
    }

    public JavaExpr recognizeEvent(Event e) {
        if (e.type == SWT.MouseDown)
            return chain(identifyControlIn(e), "click()", 1.0);
        return null;
    }
}