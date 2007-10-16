package com.simulcraft.test.gui.recorder.recognizer;

import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Shell;

import com.simulcraft.test.gui.recorder.GUIRecorder;
import com.simulcraft.test.gui.recorder.JavaExpr;

public class ShellRecognizer extends Recognizer {
    public ShellRecognizer(GUIRecorder recorder) {
        super(recorder);
    }

    public JavaExpr identifyWidget(Control control, Point point) {
        if (control instanceof Shell)
            return new JavaExpr("findShellWithTitle(\"" + ((Shell)control).getText() + "\")", 0.5);
        return null;
    }

    public JavaExpr recognize(Event e, int modifierState) {
        return null;
    }
}