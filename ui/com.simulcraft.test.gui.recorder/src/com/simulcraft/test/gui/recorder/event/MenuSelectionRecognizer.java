package com.simulcraft.test.gui.recorder.event;

import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.MenuItem;

import com.simulcraft.test.gui.recorder.GUIRecorder;
import com.simulcraft.test.gui.recorder.JavaSequence;

public class MenuSelectionRecognizer extends EventRecognizer {
    public MenuSelectionRecognizer(GUIRecorder recorder) {
        super(recorder);
    }

    public JavaSequence recognizeEvent(Event e) {
        if (e.type == SWT.Selection && e.widget instanceof MenuItem)
            return makeSeq(uiObject(e), expr("activateWithMouseClick()", 0.8, null));
        return null;
    }
}