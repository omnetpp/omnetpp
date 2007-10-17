package com.simulcraft.test.gui.recorder.recognizer;

import org.eclipse.jface.bindings.keys.KeyStroke;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Event;

import com.simulcraft.test.gui.recorder.GUIRecorder;
import com.simulcraft.test.gui.recorder.JavaExpr;


/**
 * Responsible for recording typing
 *   
 * @author Andras
 */
public class KeyboardEventRecognizer extends Recognizer {
    private String typing = "";
    private boolean modifierJustPressed = false;

    public KeyboardEventRecognizer(GUIRecorder recorder) {
        super(recorder);
    }
    
    public JavaExpr identifyControl(Control control, Point point) {
        return null;
    }

    public JavaExpr recognizeEvent(Event e) {
        int modifierState = recorder.getKeyboardModifierState();
        
        if (e.type == SWT.KeyDown) {
            if (e.character >= ' ' && e.character < 127) {
                modifierJustPressed = false;
                typing += e.character;
                return new JavaExpr("", 0.5);  // stored it. quality=0: "don't store"
            }
            else if ((e.keyCode & SWT.MODIFIER_MASK) == 0) {
                // record non-modifier control key
                modifierJustPressed = false;
                String string = KeyStroke.getInstance(modifierState, e.keyCode).format();
                return concat(flushTyping(), new JavaExpr("pressKey(SWT." + string + ")", 0.7));
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
                return new JavaExpr("pressKey(SWT." + string + ")", 0.7);
            }
        }
        else if (e.type == SWT.MouseUp || e.type == SWT.MouseDown || e.type == SWT.MouseWheel) {
            modifierJustPressed = false;
            recorder.add(flushTyping());
        }
        return null;
    }

    private JavaExpr flushTyping() {
        if (typing.length() > 0) {
            String quoted = typing.replace("\"", "\\\"");
            typing = "";
            return new JavaExpr("type(\"" + quoted + "\")", 0.7);
        }
        return null;
    }
}

