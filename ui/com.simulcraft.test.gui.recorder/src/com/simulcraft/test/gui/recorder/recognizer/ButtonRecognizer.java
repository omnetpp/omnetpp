/**
 * 
 */
package com.simulcraft.test.gui.recorder.recognizer;

import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Event;

import com.simulcraft.test.gui.recorder.IRecognizer;
import com.simulcraft.test.gui.recorder.Step;

public class ButtonRecognizer implements IRecognizer {
    public Step identifyWidget(Control control, Point point) {
        if (control instanceof Button)
            return new Step("findButtonWithLabel(\"" + ((Button)control).getText() + "\")", 0.5);
        return null;
    }

    public Step recognize(Event e, int modifierState) {
        if (e.type == SWT.MouseDown) {
            Step step = identifyWidget((Control)e.widget, new Point(e.x, e.y));
            if (step != null)
                return new Step(step.getJavaCode()+".activateWithMouseClick()", step.getQuality());
        }
        return null;
    }
}