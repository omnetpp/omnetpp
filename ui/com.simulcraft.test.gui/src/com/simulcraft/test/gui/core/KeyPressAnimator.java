package com.simulcraft.test.gui.core;

import com.simulcraft.test.gui.access.Access;

import org.eclipse.jface.bindings.keys.KeyStroke;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.CCombo;
import org.eclipse.swt.custom.StyledText;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Listener;
import org.eclipse.swt.widgets.Spinner;
import org.eclipse.swt.widgets.Text;

/**
 * Whenever a key gets pressed, this class displays the keystroke 
 * in a text box on the screen. Must be installed on Display as 
 * an event filter.
 *   
 * @author Andras
 */
public class KeyPressAnimator implements Listener {
    public static int shortcutDisplayDelay = 500; // box stays up this long
    public static int typingDelay = 50; // average time between keypresses during typing
    
    private int modifierState = 0;

    public void handleEvent(Event e) {
        if (e.keyCode == SWT.SHIFT || e.keyCode == SWT.CONTROL || e.keyCode == SWT.ALT) {
            // housekeeping: we need to keep modifier states ourselves (it doesn't arrive in the event) 
            if (e.type==SWT.KeyDown) modifierState |= e.keyCode;
            if (e.type==SWT.KeyUp) modifierState &= ~e.keyCode;
        }
        else if (e.type==SWT.KeyDown && e.keyCode!=0) { // ModeSwitcher sets keyCode to 0
            // animation. we want to suppress plain typing in text editors, text fields, etc.
            Control focusControl = Display.getCurrent().getFocusControl();
            boolean inTextControl = 
                focusControl instanceof Text || focusControl instanceof StyledText || 
                focusControl instanceof Combo || focusControl instanceof CCombo || 
                focusControl instanceof Spinner;

            if (inTextControl && !needsVisualFeedback(e)) {
                // typing: just wait a little to slow down the typing
                int delay = (int) (typingDelay/2 + Math.random()*typingDelay);
                try { Thread.sleep(Access.rescaleTime(delay)); } catch (InterruptedException e1) { }
            }
            else {
                String string = KeyStroke.getInstance(modifierState, e.keyCode).format();
                string = string.replace("Numpad_", "Numpad ");
                AnimationEffects.displayTextBox(string, shortcutDisplayDelay);
            } 
        }
    }
    
    private boolean needsVisualFeedback(Event e) {
        // navigation keys etc without any modifier don't need visual feedback
        if (modifierState == SWT.NONE && 
                (e.keyCode == SWT.BS || e.keyCode == SWT.ARROW_LEFT || e.keyCode == SWT.ARROW_RIGHT 
                 || e.keyCode == SWT.ARROW_DOWN || e.keyCode == SWT.ARROW_UP))
            return false;
        // normal characters don't need visual feedback. That includes normal key (32..127), that
        // with Shift, that with AltGr (CTRL+ALT) and AltGr+Shift. That translates to 
        // normal keys and either both or none of CTRL and ALT.
        boolean ctrlPressed = (modifierState & SWT.CTRL)!=0;
        boolean altPressed = (modifierState & SWT.ALT)!=0;
        if (ctrlPressed == altPressed && e.character >= ' ')
            return false;
        return true;
    }

    protected Point getPositionNearFocusWidget() {
        Point p;
        Display display = Display.getCurrent();
        Control focusControl = display.getFocusControl();
        if (focusControl instanceof Text) {
            // for text and styled text, place the box near the text cursor
            Text text2 = (Text)focusControl;
            p = text2.toDisplay(text2.getCaretLocation());
        }
        else if (focusControl instanceof StyledText) {
            StyledText styledText = (StyledText)focusControl;
            p = styledText.toDisplay(styledText.getLocationAtOffset(styledText.getCaretOffset()));
        }
        else {
            // if mouse is inside the control, use that position; otherwise, 
            // just place the box to the right of the control
            Rectangle bounds = focusControl.getBounds();
            p = focusControl.toDisplay(new Point(bounds.x, bounds.y));
            bounds.x = p.x; 
            bounds.y = p.y;
            if (bounds.contains(display.getCursorLocation()))
                p = display.getCursorLocation();  
            else 
                p.x += bounds.width;  
        }
        return p;
    }
}

