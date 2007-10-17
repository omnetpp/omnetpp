package com.simulcraft.test.gui.recorder.event;

import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Event;

import com.simulcraft.test.gui.recorder.GUIRecorder;
import com.simulcraft.test.gui.recorder.JavaSequence;

public class ButtonEventRecognizer extends EventRecognizer {

    public ButtonEventRecognizer(GUIRecorder recorder) {
        super(recorder);
    }

    public JavaSequence recognizeEvent(Event e) {
        if (e.type == SWT.MouseDown && e.widget instanceof Button) {
            Button button = (Button)e.widget;
            if ((button.getStyle() & SWT.PUSH) != 0) {
                return chainE(e, "activateWithMouseClick()", 1.0);
            }
            if ((button.getStyle() & (SWT.CHECK|SWT.RADIO)) != 0) {
                String action = button.getSelection() ? "deselectWithMouseClick()" : "selectWithMouseClick()";
                return chainE(e, action, 1.0);
            }
        }
        return null;
    }

}