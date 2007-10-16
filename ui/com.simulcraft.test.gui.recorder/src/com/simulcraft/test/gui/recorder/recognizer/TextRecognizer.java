package com.simulcraft.test.gui.recorder.recognizer;

import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Text;

import com.simulcraft.test.gui.recorder.GUIRecorder;
import com.simulcraft.test.gui.recorder.JavaExpr;

public class TextRecognizer extends Recognizer {
    public TextRecognizer(GUIRecorder recorder) {
        super(recorder);
    }

    public JavaExpr identifyControl(Control control, Point point) {
        if (control instanceof Text) {
            Text text = (Text)control;
            Label label = getPrecedingUniqueLabel(control.getShell(), text);
            if (label != null)
                return chain(recorder.identifyControl(control.getShell()), "findTextAfterLabel("+quote(label.getText()) + ")", 0.8);
        }
        return null;
    }

    public JavaExpr recognizeEvent(Event e) {
        if (e.type == SWT.MouseDown && e.widget instanceof Text) {
            //Text text = (Text)e.widget;
            return chain(identifyControlIn(e), "click()", 1.0);
        }
        return null;
    }
}