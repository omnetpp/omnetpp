package com.simulcraft.test.gui.recorder.event;

import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Event;

import com.simulcraft.test.gui.recorder.GUIRecorder;
import com.simulcraft.test.gui.recorder.JavaSequence;

public class ComboSelectionRecognizer extends EventRecognizer {
    public ComboSelectionRecognizer(GUIRecorder recorder) {
        super(recorder);
    }

    public JavaSequence recognizeEvent(Event e) {
        if (e.type == SWT.Selection && e.widget instanceof Combo) {
            Combo combo = (Combo)e.widget;
            return makeSeq(combo, expr("selectItem("+quoteRegexText(combo.getText())+")", 0.8, null));
        }
        return null;
    }
}

