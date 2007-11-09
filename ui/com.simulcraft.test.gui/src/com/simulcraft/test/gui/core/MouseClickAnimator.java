package com.simulcraft.test.gui.core;

import org.eclipse.core.runtime.Assert;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Listener;

/**
 * Visualizes mouse clicks by showing an inflating red circle.
 * Must be installed on Display as an event filter.
 *   
 * @author Andras
 */
public class MouseClickAnimator implements Listener {
    public static boolean mouseClickAnimation = true;
    
    public void handleEvent(Event e) {
        if (e.type == SWT.MouseDown && mouseClickAnimation) {
            Assert.isTrue(e.widget instanceof Control); // by experimental evidence ;)
            Point p = ((Control)e.widget).toDisplay(e.x, e.y);
            AnimationEffects.animateClick(p.x, p.y);
        }
    }
}

