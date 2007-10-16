package com.simulcraft.test.gui.recorder.recognizer;

import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Label;

import com.simulcraft.test.gui.recorder.GUIRecorder;
import com.simulcraft.test.gui.recorder.JavaExpr;

public class ComboRecognizer extends Recognizer {
    public ComboRecognizer(GUIRecorder recorder) {
        super(recorder);
    }

    public JavaExpr identifyControl(Control control, Point point) {
        if (control instanceof Combo) {
            Combo text = (Combo)control;
            Label label = getPrecedingUniqueLabel(control.getShell(), text);
            if (label != null)
                return chain(recorder.identifyControl(control.getShell()), "findComboAfterLabel("+quote(label.getText()) + ")", 0.8);
        }
        return null;
    }

    public JavaExpr recognizeEvent(Event e) {
        if (e.type == SWT.MouseDown && e.widget instanceof Combo) {
            //Combo combo = (Combo)e.widget;
            return chain(identifyControlIn(e), "click()", 1.0);
        }
        return null;
    }
}