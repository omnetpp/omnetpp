package com.simulcraft.test.gui.recorder.recognizer;

import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Event;

import com.simulcraft.test.gui.recorder.GUIRecorder;
import com.simulcraft.test.gui.recorder.JavaExpr;

public class ButtonRecognizer extends Recognizer {
    public ButtonRecognizer(GUIRecorder recorder) {
        super(recorder);
    }

    public JavaExpr identifyControl(Control control, Point point) {
        if (control instanceof Button) {
            Button button = (Button)control;
            return chain(recorder.identifyControl(control.getShell()), "findButtonWithLabel("+quote(button.getText()) + ")", 0.8);
        }
        return null;
    }

    public JavaExpr recognizeEvent(Event e) {
        if (e.type == SWT.MouseDown && e.widget instanceof Button) {
            Button button = (Button)e.widget;
            if ((button.getStyle() & SWT.PUSH) != 0) {
                return chain(identifyControlIn(e), "activateWithMouseClick()", 1.0);
            }
            if ((button.getStyle() & (SWT.CHECK|SWT.RADIO)) != 0) {
                String action = button.getSelection() ? "deselectWithMouseClick()" : "selectWithMouseClick()";
                return chain(identifyControlIn(e), action, 1.0);
            }
        }
        return null;
    }
}