package com.simulcraft.test.gui.recorder.event;

import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Event;

import com.simulcraft.test.gui.recorder.GUIRecorder;
import com.simulcraft.test.gui.recorder.JavaSequence;

public class ClickRecognizer extends EventRecognizer {
    public ClickRecognizer(GUIRecorder recorder) {
        super(recorder);
    }

    public JavaSequence recognizeEvent(Event e) {
        if (e.type == SWT.MouseDown && e.button == 1) // left button
            return makeSeq(uiObject(e), expr("click()", 0.3, null));
        if (e.type == SWT.MouseDown && e.button == 3) // right button
            return makeSeq(uiObject(e), expr("click(RIGHT_MOUSE_BUTTON)", 0.3, null));
        if (e.type == SWT.MouseDoubleClick)
            return makeSeq(uiObject(e), expr("doubleClick()", 0.3, null));
        return null;
    }
}