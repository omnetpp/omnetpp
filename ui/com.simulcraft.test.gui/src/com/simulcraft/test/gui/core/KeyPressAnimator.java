package com.simulcraft.test.gui.core;

import org.eclipse.jface.bindings.keys.KeyStroke;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.CCombo;
import org.eclipse.swt.custom.StyledText;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Listener;
import org.eclipse.swt.widgets.Text;
import org.omnetpp.common.color.ColorFactory;

/**
 * Whenever a key gets pressed, this class displays the keystroke 
 * in a text box on the screen. Must be installed on Display as 
 * an event filter.
 *   
 * @author Andras
 */
public class KeyPressAnimator implements Listener {
    private int modifierState = 0;
    
    public void handleEvent(Event e) {
        if (e.keyCode == SWT.SHIFT || e.keyCode == SWT.CONTROL || e.keyCode == SWT.ALT) {
            // housekeeping: we need to keep modifier states ourselves (it doesn't arrive in the event) 
            if (e.type==SWT.KeyDown) modifierState |= e.keyCode;
            if (e.type==SWT.KeyUp) modifierState &= ~e.keyCode;
        }
        else if (e.type==SWT.KeyDown) {
            // animation. we want to suppress plain typing in text editors, text fields, etc.
            Control focusControl = Display.getCurrent().getFocusControl();
            boolean inTextControl = 
                focusControl instanceof Text || focusControl instanceof StyledText || 
                focusControl instanceof Combo || focusControl instanceof CCombo;

            if (!inTextControl || e.character < ' ' || e.character >= 128) {
                String string = KeyStroke.getInstance(modifierState, e.keyCode).toString();
                string = string.replaceFirst("ARROW_", "").replaceFirst("\\bCR$", "Enter").replaceFirst("\\bBS$", "Backspace"); // refine output a little
                displayTextBox(string);
            } 
            else {
                // typing: just wait a little to slow down the typing
                try { Thread.sleep(50); } catch (InterruptedException e1) { }
            }
        }
    }
    
    protected void displayTextBox(String text) {
        Display display = Display.getCurrent();
        Point cursor = display.getCursorLocation();
        GC gc = new GC(display);

        Font font = new Font(display, "Arial", 20, SWT.NORMAL);
        gc.setFont(font);
        Point textExtent = gc.textExtent(text);
        Rectangle r = new Rectangle(cursor.x + 32, cursor.y - 10, textExtent.x + 20, textExtent.y + 10);

        Image savedBackground = new Image(display, r.width, r.height);
        gc.copyArea(savedBackground, r.x, r.y);
        gc.setClipping(r);
        
        gc.setBackground(ColorFactory.LIGHT_YELLOW);
        gc.setForeground(ColorFactory.BLACK);
        gc.setLineWidth(2);
        gc.fillRectangle(r);
        gc.drawRectangle(r);
        gc.drawText(text, r.x + r.width/2 - textExtent.x/2, r.y + r.height/2 - textExtent.y/2);
        
        try { Thread.sleep(300); } catch (InterruptedException e) { }
        
        gc.drawImage(savedBackground, r.x, r.y);

        savedBackground.dispose();
        font.dispose();
        gc.dispose();
    }
}

