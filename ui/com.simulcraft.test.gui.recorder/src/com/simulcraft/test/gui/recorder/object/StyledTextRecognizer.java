package com.simulcraft.test.gui.recorder.object;

import org.eclipse.swt.custom.StyledText;
import org.eclipse.swt.widgets.Composite;

import com.simulcraft.test.gui.recorder.GUIRecorder;
import com.simulcraft.test.gui.recorder.JavaSequence;

public class StyledTextRecognizer extends ObjectRecognizer {
    public StyledTextRecognizer(GUIRecorder recorder) {
        super(recorder);
    }

    public JavaSequence identifyObject(Object uiObject) {
        if (uiObject instanceof StyledText) {
            StyledText styledText = (StyledText)uiObject;
            Composite container = findContainer(styledText);
            dumpWidgetHierarchy(container);
            if (findDescendantControl(container, StyledText.class) == uiObject)
                return makeSeq(container, expr("findSyledText()", 0.8, styledText));
        }
        return null;
    }
}