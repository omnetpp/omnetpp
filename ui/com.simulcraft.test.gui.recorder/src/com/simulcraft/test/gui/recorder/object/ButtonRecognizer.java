package com.simulcraft.test.gui.recorder.object;

import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;

import com.simulcraft.test.gui.recorder.GUIRecorder;
import com.simulcraft.test.gui.recorder.JavaSequence;

public class ButtonRecognizer extends ObjectRecognizer {
    
    public ButtonRecognizer(GUIRecorder recorder) {
        super(recorder);
    }

    public JavaSequence identifyObject(Object uiObject) {
        if (uiObject instanceof Button) {
            Button button = (Button)uiObject;
            Composite container = findContainer(button);
            //FIXME check label uniquely identifies button within container
            return makeSeq(container, expr("findButtonWithLabel("+quote(button.getText()) + ")", 0.8, button));
        }
        return null;
    }
}