package com.simulcraft.test.gui.core;

import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.common.color.ColorFactory;

import com.simulcraft.test.gui.access.ClickableAccess;

public class AnimationEffects  {
    
    public static void displayTextBox(String text, long delayMillis) {
    	displayTextBox(text, ColorFactory.BLACK, 16, delayMillis);
    }
    
    public static void displayTextBox(String text, Color textColor, int fontSize, long delayMillis) {
        // choose font, calculate position and size
        Display display = Display.getCurrent();
        GC gc = new GC(display);
        Font font = new Font(display, "Arial", fontSize, SWT.NORMAL);
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
        gc.setForeground(textColor);
        gc.drawText(text, r.x + r.width/2 - textExtent.x/2, r.y + r.height/2 - textExtent.y/2);

        // wait
        try { Thread.sleep(delayMillis); } catch (InterruptedException e) { }

        // restore background
        gc.drawImage(savedBackground, r.x, r.y);

        // dispose
        savedBackground.dispose();
        font.dispose();
        gc.dispose();
    }
    
    public static void displayError(Throwable error, long delayMillis) {
        displayTextBox(error.toString(), ColorFactory.RED, 12, delayMillis);
    }
    
    @SuppressWarnings("deprecation")
    public static void animateClick(int x, int y) {
        // draw inflating red circle 
        GC gc = new GC(Display.getCurrent());
        gc.setForeground(ColorFactory.CYAN); // complement of red
        gc.setLineWidth(2);
        gc.setXORMode(true); // won't work on Mac
        for (int r = 2; r < 25; r++) {
            gc.drawOval(x-r, y-r, 2*r, 2*r);
            try { Thread.sleep(5); } catch (InterruptedException e) { break; }
            gc.drawOval(x-r, y-r, 2*r, 2*r);
        }
    }

    public static void beginAnimateDragDrop(int x1, int y1, int x2, int y2) {
        // note: normal drag&drop animation (the SWT one) is missing because we don't call readAndDispatch() between posting mouse events
        drawXorLine(x1, y1, x2, y2);
        displayTextBox("dragging...", 500);
        ClickableAccess.mouseMoveDurationMillis *= 2;
    }

    public static void endAnimateDragDrop(int x1, int y1, int x2, int y2) {
        ClickableAccess.mouseMoveDurationMillis /= 2;
        drawXorLine(x1, y1, x2, y2);
        displayTextBox("drop", 300);
        animateClick(x2, y2);
    }

    @SuppressWarnings("deprecation")
    private static void drawXorLine(int x1, int y1, int x2, int y2) {
        // draw red line 
        GC gc = new GC(Display.getCurrent());
        gc.setForeground(ColorFactory.CYAN); // complement of red
        gc.setLineWidth(2);
        gc.setXORMode(true); // won't work on Mac
        gc.drawLine(x1, y1, x2, y2);
    }

}

