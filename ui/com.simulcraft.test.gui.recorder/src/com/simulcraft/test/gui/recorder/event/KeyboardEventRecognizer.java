package com.simulcraft.test.gui.recorder.event;

import org.eclipse.jface.bindings.keys.KeyStroke;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Event;

import com.simulcraft.test.gui.recorder.GUIRecorder;
import com.simulcraft.test.gui.recorder.JavaSequence;


/**
 * Responsible for recording typedChars
 *   
 * @author Andras
 */
public class KeyboardEventRecognizer extends EventRecognizer {
    private String typedChars = "";
    private Control typingFocus = null;

    public KeyboardEventRecognizer(GUIRecorder recorder) {
        super(recorder);
    }
    
    public JavaSequence recognizeEvent(Event e) {
        int modifierState = recorder.getKeyboardModifierState();
        Control focusControl = Display.getCurrent().getFocusControl();
//        if (focusControl != typingFocus || e.type == SWT.MouseDown /*|| 
//                e.type == SWT.MouseWheel || e.type == SWT.Selection*/) {
//            flushTyping();
//            typingFocus = focusControl;
//            System.out.println("BUUU");
//        }
        
        if (e.type == SWT.KeyDown) {
            if (e.character >= ' ' && e.character < 127) {
                typedChars += e.character;
                return new JavaSequence();
            }
            else if ((e.keyCode & ~SWT.MODIFIER_MASK) != 0) {
                // record non-modifier control key
                flushTyping();
                return makeSeq(focusControl, expr(toPressKeyInvocation(e, modifierState), 0.7, null));
            }
            else {
                // modifier KeyDown -- ignore
                return new JavaSequence();
            }
        }
        return null;
    }

    public static String toPressKeyInvocation(Event e, int modifierState) {
        String modifierString = KeyStroke.getInstance(modifierState, 0).toString();
        modifierString = modifierString.replaceAll("\\+$", "").replace("+", " | ").replaceAll("\\b([A-Z])", "SWT.$1");
        String keyString = KeyStroke.getInstance(0, e.keyCode).toString();
        keyString = keyString.length()==1 ? ("'"+keyString+"'") : ("SWT."+keyString);
        if (modifierState == 0)
            return "pressKey(" + keyString + ")";
        else 
            return "pressKey(" + keyString + ", " + modifierString + ")";
    }
    
    protected void flushTyping() {
        if (typedChars.length() > 0) {
            recorder.add(makeSeq(typingFocus, expr("typeIn(" + toJavaLiteral(typedChars) + ")", 0.7, null)));
            typedChars = "";
        }
    }
}

