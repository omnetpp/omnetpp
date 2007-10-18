package com.simulcraft.test.gui.recorder.object;

import org.eclipse.swt.widgets.Shell;

import com.simulcraft.test.gui.recorder.GUIRecorder;
import com.simulcraft.test.gui.recorder.JavaSequence;

public class ShellRecognizer extends ObjectRecognizer {
    public ShellRecognizer(GUIRecorder recorder) {
        super(recorder);
    }

    public JavaSequence identifyObject(Object uiObject) {
        if (uiObject instanceof Shell) {
            Shell shell = (Shell)uiObject;
            //FIXME check label uniquely identifies button within container
            return makeSeq(expr("findShellWithTitle(" + quote(shell.getText()) + ")", 0.5, shell));
        }
        return null;
    }
}