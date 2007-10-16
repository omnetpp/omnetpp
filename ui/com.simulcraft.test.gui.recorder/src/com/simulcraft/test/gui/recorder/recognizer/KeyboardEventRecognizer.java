package com.simulcraft.test.gui.recorder.recognizer;

import org.eclipse.jface.bindings.keys.KeyStroke;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Event;

import com.simulcraft.test.gui.recorder.GUIRecorder;
import com.simulcraft.test.gui.recorder.IRecognizer;
import com.simulcraft.test.gui.recorder.Step;


/**
 * Responsible for recording typing
 *   
 * @author Andras
 */
public class KeyboardEventRecognizer implements IRecognizer {
    private String typing = "";
    private boolean modifierJustPressed = false;
    
    public KeyboardEventRecognizer(GUIRecorder recorder) {
    }

    public Step identifyWidget(Control control, Point point) {
        return null;
    }

    public Step recognize(Event e, int modifierState) {
        if (e.type == SWT.KeyDown) {
            if (e.character >= ' ' && e.character < 127) {
                modifierJustPressed = false;
                typing += e.character;
            }
            else if ((e.keyCode & SWT.MODIFIER_MASK) == 0) {
                // record non-modifier control key
                modifierJustPressed = false;
                Step typingStep = flushTyping();
                String typingJavaCode = typingStep == null ? "" : typingStep.getJavaCode();
                
                String string = KeyStroke.getInstance(modifierState, e.keyCode).format();
                return new Step(typingJavaCode + "pressKey(SWT." + string + ");", 0.7);
            }
            else {
                // modifier KeyDown -- ignore
                modifierJustPressed = true;
            }
        }
        else if (e.type == SWT.KeyUp) {
            // record if modifier was pressed then released without anything in between
            if (modifierJustPressed) {
                modifierJustPressed = false;
                String string = KeyStroke.getInstance(modifierState, e.keyCode).format();
                return new Step("pressKey(SWT." + string + ");", 0.7);
            }
        }
        else if (e.type == SWT.MouseUp || e.type == SWT.MouseDown || e.type == SWT.MouseWheel) {
            modifierJustPressed = false;
            return flushTyping();
        }
        return null;
    }

    private Step flushTyping() {
        if (typing.length() > 0) {
            String quoted = typing.replace("\"", "\\\"");
            typing = "";
            return new Step("type(\"" + quoted + "\");", 0.7);
        }
        return null;
    }
}

