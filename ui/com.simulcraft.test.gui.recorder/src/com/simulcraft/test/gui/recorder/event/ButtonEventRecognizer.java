package com.simulcraft.test.gui.recorder.event;

import java.util.List;

import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Event;

import com.simulcraft.test.gui.recorder.GUIRecorder;
import com.simulcraft.test.gui.recorder.JavaExpr;

public class ButtonEventRecognizer extends EventRecognizer {

    public ButtonEventRecognizer(GUIRecorder recorder) {
        super(recorder);
    }

    public List<JavaExpr> recognizeEvent(Event e) {
        if (e.type == SWT.MouseDown && e.widget instanceof Button) {
            Button button = (Button)e.widget;
            if ((button.getStyle() & SWT.PUSH) != 0) {
                return chain(recorder.identifyObjectIn(e), "activateWithMouseClick()", 1.0);
            }
            if ((button.getStyle() & (SWT.CHECK|SWT.RADIO)) != 0) {
                String action = button.getSelection() ? "deselectWithMouseClick()" : "selectWithMouseClick()";
                return chain(recorder.identifyObjectIn(e), action, 1.0);
            }
        }
        return null;
    }

}