/**
 * 
 */
package com.simulcraft.test.gui.recorder.recognizer;

import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Shell;

import com.simulcraft.test.gui.recorder.IRecognizer;
import com.simulcraft.test.gui.recorder.Step;

public class ShellRecognizer implements IRecognizer {
    public Step identifyWidget(Control control, Point point) {
        if (control instanceof Shell)
            return new Step("findShellWithTitle(\"" + ((Shell)control).getText() + "\")", 0.5);
        return null;
    }

    public Step recognize(Event e, int modifierState) {
        return null;
    }
}