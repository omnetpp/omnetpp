package com.simulcraft.test.gui.recorder;

import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Event;

/**
 * Recognizes a GUI event, and returns it as an IStep.
 * @author Andras
 */
public interface IRecognizer {
    /** 
     * Returns some code that identifies the given widget.
     * Point is relative to the control.
     */
    JavaExpr identifyWidget(Control control, Point point);

    /**
     * Returns some code which produce the event when played back.
     */
    JavaExpr recognize(Event e, int modifierState);
}
