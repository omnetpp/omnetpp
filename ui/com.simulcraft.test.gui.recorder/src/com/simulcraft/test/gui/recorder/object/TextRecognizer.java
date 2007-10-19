package com.simulcraft.test.gui.recorder.object;

import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Text;

import com.simulcraft.test.gui.recorder.GUIRecorder;
import com.simulcraft.test.gui.recorder.JavaSequence;

public class TextRecognizer extends ObjectRecognizer {
    public TextRecognizer(GUIRecorder recorder) {
        super(recorder);
    }

    public JavaSequence identifyObject(Object uiObject) {
        if (uiObject instanceof Text) {
            Text text = (Text)uiObject;
            Composite container = findContainer(text);
            Label label = getPrecedingUniqueLabel(container, text);
            if (label != null)
                return makeSeq(container, expr("findTextAfterLabel("+quoteText(label.getText()) + ")", 0.8, text));
        }
        return null;
    }
}