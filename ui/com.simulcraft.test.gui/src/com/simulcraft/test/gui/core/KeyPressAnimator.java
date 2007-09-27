package com.simulcraft.test.gui.core;

import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.bindings.keys.KeyStroke;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Listener;
import org.omnetpp.common.color.ColorFactory;

/**
 * Whenever a key gets pressed, this class displays the keystroke 
 * in a text box on the screen. Must be installed on Display as 
 * an event filter.
 *   
 * @author Andras
 */
public class KeyPressAnimator implements Listener {
    public void handleEvent(Event e) {
        Assert.isTrue(e.type==SWT.KeyDown);
        String string = KeyStroke.getInstance(e.stateMask & SWT.KEY_MASK, e.keyCode).toString();
        //String string = new NativeKeyFormatter().format(e.keyCode);
        displayTextBox(string);
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

