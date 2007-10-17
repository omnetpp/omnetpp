package com.simulcraft.test.gui.recorder.object;

import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Label;

import com.simulcraft.test.gui.recorder.GUIRecorder;
import com.simulcraft.test.gui.recorder.JavaSequence;

public class ComboRecognizer extends ObjectRecognizer {
    public ComboRecognizer(GUIRecorder recorder) {
        super(recorder);
    }

    public JavaSequence identifyObject(Object uiObject) {
        if (uiObject instanceof Combo) {
            Combo combo = (Combo)uiObject;
            Label label = getPrecedingUniqueLabel(combo.getShell(), combo);
            if (label != null)
                return makeSeq(combo.getShell(), expr("findComboAfterLabel("+quote(label.getText()) + ")", 0.8, combo));
        }
        return null;
    }
}