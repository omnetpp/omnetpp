package com.simulcraft.test.gui.recorder.event;

import org.eclipse.jface.bindings.keys.KeyStroke;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Event;

import com.simulcraft.test.gui.recorder.GUIRecorder;
import com.simulcraft.test.gui.recorder.ICodeRewriter;
import com.simulcraft.test.gui.recorder.JavaExpr;
import com.simulcraft.test.gui.recorder.JavaSequence;


/**
 * Responsible for recording typing. Seems like hotkeys are NOT delivered here, because
 * they get translated earlier on.
 *   
 * @author Andras
 */
public class KeyboardEventRecognizer extends EventRecognizer implements ICodeRewriter {
    public KeyboardEventRecognizer(GUIRecorder recorder) {
        super(recorder);
    }
    
    public JavaSequence recognizeEvent(Event e) {
        int modifierState = recorder.getKeyboardModifierState();
        if (e.type == SWT.KeyDown && (e.keyCode & ~SWT.MODIFIER_MASK) != 0) {
            Control focusControl = Display.getCurrent().getFocusControl();
            if (focusControl == null)
                return makeSeq(expr("Access." + toPressKeyInvocation(e, modifierState), 0.7, null));
            else
                return makeSeq(focusControl, expr(toPressKeyInvocation(e, modifierState), 0.7, null));
        }
        return null;
    }

    public static String toPressKeyInvocation(Event e, int modifierState) {
        if (e.keyCode >= 32 && e.keyCode < 127 && (modifierState & ~SWT.SHIFT)==0) {
            return "pressKey('" + e.character + "')";
        }
        else {
            return toPressKeyInvocation(e.keyCode, modifierState);
        }
    }

    public static String toPressKeyInvocation(int keyCode, int modifierState) {
        String modifierString = KeyStroke.getInstance(modifierState, 0).toString();
        modifierString = modifierString.replaceAll("\\+$", "").replace("+", " | ").replaceAll("\\b([A-Z])", "SWT.$1");
        String keyString = KeyStroke.getInstance(0, keyCode).toString();
        keyString = keyString.length()==1 ? ("'"+keyString+"'") : ("SWT."+keyString);
        if (modifierState == 0)
            return "pressKey(" + keyString + ")";
        else 
            return "pressKey(" + keyString + ", " + modifierString + ")";
    }

    public void rewrite(JavaSequence list) {
        // merge separate keypresses into typing
        if (list.endMatches(-1, "pressKey\\('.'\\)") && list.endMatches(-2, "pressKey\\('.'\\)") &&
                list.getEnd(-1).getCalledOn()==list.getEnd(-2).getCalledOn()) { 
            String arg1 = list.getEnd(-1).getJavaCode().replaceFirst("pressKey\\('(.)'\\)", "$1");
            String arg2 = list.getEnd(-2).getJavaCode().replaceFirst("pressKey\\('(.)'\\)", "$1");

            JavaExpr newExpr = expr("typeIn("+toJavaLiteral(arg2+arg1)+")", 1.0, null);
            newExpr.setCalledOn(list.getEnd(-1).getCalledOn());
            list.replaceEnd(-2, 2, makeSeq(newExpr));
        }
        if (list.endMatches(-1, "pressKey\\('.'\\)") && list.endMatches(-2, "typeIn\\(\".*\"\\)") &&
                list.getEnd(-1).getCalledOn()==list.getEnd(-2).getCalledOn()) { 
            String arg1 = list.getEnd(-1).getJavaCode().replaceFirst("pressKey\\('(.)'\\)", "$1");
            String arg2 = list.getEnd(-2).getJavaCode().replaceFirst("typeIn\\(\"(.*)\"\\)", "$1");

            JavaExpr newExpr = expr("typeIn("+toJavaLiteral(arg2+arg1)+")", 1.0, null);
            newExpr.setCalledOn(list.getEnd(-1).getCalledOn());
            list.replaceEnd(-2, 2, makeSeq(newExpr));
        }
    }
}

