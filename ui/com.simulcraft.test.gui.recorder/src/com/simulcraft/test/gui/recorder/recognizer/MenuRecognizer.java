package com.simulcraft.test.gui.recorder.recognizer;

import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Event;

import com.simulcraft.test.gui.recorder.GUIRecorder;
import com.simulcraft.test.gui.recorder.JavaExpr;

public class MenuRecognizer extends Recognizer {
    public MenuRecognizer(GUIRecorder recorder) {
        super(recorder);
    }

    public JavaExpr identifyControl(Control control, Point point) {
        return null;
    }

    public JavaExpr recognizeEvent(Event e) {
        //FIXME won't work: we don't get the mouse events for the menus!!! add SelectionListeners to MenuItems instead?
        if (e.type == SWT.MouseDown) {
            //System.out.println(e.widget.getClass().getSimpleName());
            //...
        }
        return null;
    }
}