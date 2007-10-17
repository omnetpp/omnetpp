package com.simulcraft.test.gui.recorder.recognizer;

import java.util.List;

import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Label;

import com.simulcraft.test.gui.recorder.GUIRecorder;
import com.simulcraft.test.gui.recorder.JavaExpr;

public class ComboRecognizer extends ObjectRecognizer {
    public ComboRecognizer(GUIRecorder recorder) {
        super(recorder);
    }

    public List<JavaExpr> identifyObject(Object uiObject) {
        if (uiObject instanceof Combo) {
            Combo combo = (Combo)uiObject;
            Label label = getPrecedingUniqueLabel(combo.getShell(), combo);
            if (label != null)
                return chain(recorder.identifyObject(combo.getShell()), "findComboAfterLabel("+quote(label.getText()) + ")", 0.8);
        }
        return null;
    }
}