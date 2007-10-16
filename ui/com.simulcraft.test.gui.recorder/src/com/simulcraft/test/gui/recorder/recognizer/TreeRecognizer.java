package com.simulcraft.test.gui.recorder.recognizer;

import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Tree;

import com.simulcraft.test.gui.recorder.GUIRecorder;
import com.simulcraft.test.gui.recorder.JavaExpr;

public class TreeRecognizer extends Recognizer {
    public TreeRecognizer(GUIRecorder recorder) {
        super(recorder);
    }

    public JavaExpr identifyControl(Control control, Point point) {
        if (control instanceof Tree) {
            if (findDescendantControl(control.getShell(), Tree.class) == control)
                return chain(recorder.identifyControl(control.getShell()), "findTree()", 0.9);
        }
        return null;
    }

    public JavaExpr recognizeEvent(Event e) {
        if (e.type == SWT.MouseDown)
            return chain(identifyControlIn(e), "click()", 1.0);
        return null;
    }
}