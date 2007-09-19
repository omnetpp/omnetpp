package org.omnetpp.test.gui.access;

import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.test.gui.core.InUIThread;

public class ClickableAccess
	extends Access 
{
    public static boolean smoothMouseMovement = true;
    public static int mouseMoveDurationMillis = 500;
    public static int mouseMoveMaxSteps = 20;
    
	@InUIThread
	public void moveMouseAbsolute(Point p) {
		moveMouseAbsolute(p.x, p.y);
	}

	@InUIThread
	public void moveMouseAbsolute(int x, int y) {
	    Point p = Display.getCurrent().getCursorLocation();
	    if (p.x != x || p.y != y) {
	        if (smoothMouseMovement) {
	            System.out.println("moving mouse smoothly from "+p+" to "+new Point(x,y));
	            int steps = Math.max(Math.abs(x-p.x), Math.abs(y-p.y));
	            if (steps > mouseMoveMaxSteps) steps = mouseMoveMaxSteps;
	            double dx = (x - p.x) / (double)steps; 
	            double dy = (y - p.y) / (double)steps;
	            int stepMillis = Math.max( (int)(mouseMoveDurationMillis / steps), 1);
	            for (int i=0; i<steps; i++) {
	                try { Thread.sleep(stepMillis); } catch (InterruptedException e) {}
	                postMouseEvent(SWT.MouseMove, 0, p.x + (int)(i*dx), p.y + (int)(i*dy));
	            }
	        }
	        postMouseEvent(SWT.MouseMove, 0, x, y);
	    }
	}

	@InUIThread
	public void clickAbsolute(int button, Point point) {
		clickAbsolute(button, point.x, point.y);
	}

	@InUIThread
	public void clickAbsolute(int button, int x, int y) {
		moveMouseAbsolute(x, y);
		postMouseEvent(SWT.MouseDown, button, x, y);
		postMouseEvent(SWT.MouseUp, button, x, y);
	}

    @InUIThread
    public void clickCenterAbsolute(int button, Rectangle rectangle) {
        clickAbsolute(button, getCenter(rectangle));
    }

	@InUIThread
	public void doubleClickAbsolute(int button, int x, int y) {
		clickAbsolute(button, x, y);
		postMouseEvent(SWT.MouseDown, button, x, y);
		postMouseEvent(SWT.MouseUp, button, x, y);
	}

	@InUIThread
	public void doubleClickAbsolute(int button, Point point) {
		doubleClickAbsolute(button, point.x, point.y);
	}

	@InUIThread
	public void doubleClickCenterAbsolute(int button, Rectangle rectangle) {
		doubleClickAbsolute(button, getCenter(rectangle));
	}
}
