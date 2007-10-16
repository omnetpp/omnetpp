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

    public JavaExpr identifyWidget(Control control, Point point) {
        if (control instanceof Button)
            return new JavaExpr("findButtonWithLabel(\"" + ((Button)control).getText() + "\")", 0.5);
        return null;
    }

    public JavaExpr recognize(Event e, int modifierState) {
        if (e.type == SWT.MouseDown) {
            JavaExpr javaExpr = identifyWidget((Control)e.widget, new Point(e.x, e.y));
            if (javaExpr != null)
                return new JavaExpr(javaExpr.getJavaCode()+".activateWithMouseClick()", javaExpr.getQuality());
        }
        return null;
    }
}