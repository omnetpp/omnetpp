package com.simulcraft.test.gui.core;

import org.eclipse.core.runtime.Assert;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Listener;
import org.omnetpp.common.color.ColorFactory;

/**
 * Visualizes mouse clicks by showing an inflating red circle.
 * Must be installed on Display as an event filter.
 *   
 * @author Andras
 */
public class MouseClickAnimator implements Listener {
    public void handleEvent(Event e) {
        if (e.type == SWT.MouseDown) {
            Assert.isTrue(e.widget instanceof Control); // by experimental evidence ;)
            Point p = ((Control)e.widget).toDisplay(e.x, e.y); 
            animateClick(p.x, p.y);
        }
    }

    @SuppressWarnings("deprecation")
    protected void animateClick(int x, int y) {
        // draw inflating red circle (red=complement of cyan)
        GC gc = new GC(Display.getCurrent());
        gc.setForeground(ColorFactory.CYAN);
        gc.setLineWidth(2);
        gc.setXORMode(true); // won't work on Mac
        for (int r = 2; r < 25; r++) {
            gc.drawOval(x-r, y-r, 2*r, 2*r);
            try { Thread.sleep(5); } catch (InterruptedException e) { break; }
            gc.drawOval(x-r, y-r, 2*r, 2*r);
        }
    }
}

