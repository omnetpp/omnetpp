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
    private long longDelayMillis = 500; // box stays up this long
    private long shortDelayMillis = 50; // average time between keypresses during typing
    
    private int modifierState = 0;

    public long getLongDelayMillis() {
        return longDelayMillis;
    }

    public void setLongDelayMillis(long longDelayMillis) {
        this.longDelayMillis = longDelayMillis;
    }

    public long getShortDelayMillis() {
        return shortDelayMillis;
    }

    public void setShortDelayMillis(long shortDelayMillis) {
        this.shortDelayMillis = shortDelayMillis;
    }

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
                String string = KeyStroke.getInstance(modifierState, e.keyCode).format();
                displayTextBox(string);
            } 
            else {
                // typing: just wait a little to slow down the typing
                long delay = (long) (shortDelayMillis/2 + Math.random()*shortDelayMillis);
                try { Thread.sleep(delay); } catch (InterruptedException e1) { }
            }
        }
    }

    protected void displayTextBox(String text) {
        // choose font, calculate position and size
        Display display = Display.getCurrent();
        GC gc = new GC(display);
        Font font = new Font(display, "Arial", 16, SWT.NORMAL);
        gc.setFont(font);
        Point textExtent = gc.textExtent(text);

        //Point p = getPositionNearFocusWidget();
        Point p = display.getCursorLocation();  

        Rectangle r = new Rectangle(p.x + 10, p.y + 10, textExtent.x + 20, textExtent.y + 10);

        // save original background
        Image savedBackground = new Image(display, r.width, r.height);
        gc.copyArea(savedBackground, r.x, r.y);
        gc.setClipping(r);

        // draw text box
        gc.setBackground(ColorFactory.LIGHT_YELLOW);
        gc.setForeground(ColorFactory.BLACK);
        gc.setLineWidth(2);
        gc.fillRectangle(r);
        gc.drawRectangle(r);
        gc.drawText(text, r.x + r.width/2 - textExtent.x/2, r.y + r.height/2 - textExtent.y/2);

        // wait
        try { Thread.sleep(longDelayMillis); } catch (InterruptedException e) { }

        // restore background
        gc.drawImage(savedBackground, r.x, r.y);

        // dispose
        savedBackground.dispose();
        font.dispose();
        gc.dispose();
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

