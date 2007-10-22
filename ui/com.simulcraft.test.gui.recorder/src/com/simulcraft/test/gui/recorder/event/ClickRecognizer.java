package com.simulcraft.test.gui.recorder.event;

import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Event;

import com.simulcraft.test.gui.recorder.GUIRecorder;
import com.simulcraft.test.gui.recorder.ICodeRewriter;
import com.simulcraft.test.gui.recorder.JavaSequence;

public class ClickRecognizer extends EventRecognizer implements ICodeRewriter {
    public ClickRecognizer(GUIRecorder recorder) {
        super(recorder);
    }

    public JavaSequence recognizeEvent(Event e) {
        if (e.type == SWT.MouseDown && e.button == 1) // left button
            return makeMethodCall(uiObject(e), expr("click()", 0.3, null));
        if (e.type == SWT.MouseDown && e.button == 3) // right button
            return makeMethodCall(uiObject(e), expr("click(RIGHT_MOUSE_BUTTON)", 0.3, null));
        if (e.type == SWT.MouseDoubleClick)
            return makeMethodCall(uiObject(e), expr("doubleClick()", 0.3, null));
        return null;
    }

    public void rewrite(JavaSequence list) {
        // replace "click, click, doubleClick" with "doubleClick"
        if (list.endEquals(-1, "doubleClick()") && 
                list.endEquals(-2, "click()") && list.endEquals(-3, "click()") &&
                list.getEnd(-1).getCalledOn()==list.getEnd(-2).getCalledOn() && 
                list.getEnd(-2).getCalledOn() == list.getEnd(-3).getCalledOn()
                )
            list.replaceEnd(-3, 2, null);
    }
}