package com.simulcraft.test.gui.access;

import static java.lang.Math.PI;
import static java.lang.Math.abs;
import static java.lang.Math.max;
import static java.lang.Math.sin;
import static java.lang.Math.sqrt;

import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.common.color.ColorFactory;

import com.simulcraft.test.gui.core.InUIThread;

public class ClickableAccess
	extends Access 
{
    public static boolean smoothMouseMovement = true;
    public static int mouseMoveDurationMillis = 500;
    public static int mouseMoveMaxSteps = 30;

    // mouse trajectory: two functions fx(t),fy(t) needed, f(0)=0 and f(1)=1 for both, 0<=t<1
    interface Ft01 { double f(double t); };
    
    //private Ft01 xt = new Ft01() { public double f(double t) { return pow(t, 0.2); } };
    //private Ft01 yt = new Ft01() { public double f(double t) { return pow(t, 0.2); } };

    //private Ft01 xt = new Ft01() { public double f(double t) { return sin(t*PI/2) + sin(t*2*PI)/20; } };
    //private Ft01 yt = new Ft01() { public double f(double t) { return sin(t*PI/2) - sin(t*2*PI)/20; } };

    //private Ft01 xt = new Ft01() { public double f(double t) { return sin(t*PI/2) + sin(t*PI)/10; } };
    //private Ft01 yt = new Ft01() { public double f(double t) { return sin(t*PI/2); } };

    private Ft01 xt = new Ft01() { public double f(double t) { return sin(t*PI/2); } };
    private Ft01 yt = new Ft01() { public double f(double t) { return sqrt(sin(t*PI/2)); } };

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
                int steps = max(abs(x-p.x), abs(y-p.y));
	            if (steps > mouseMoveMaxSteps) steps = mouseMoveMaxSteps;
	            int stepMillis = max( (int)(mouseMoveDurationMillis / steps), 1);
	            double dt = 1.0 / steps;
	            for (double t = 0; t < 1.0; t += dt) {
	                postMouseEvent(SWT.MouseMove, 0, p.x + (int)(xt.f(t)*(x-p.x)), p.y + (int)(yt.f(t)*(y-p.y)));
	                try { Thread.sleep(stepMillis); } catch (InterruptedException e) { break; }
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
		
		animateClick(x, y);
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
        animateClick(x, y);
	}

	@InUIThread
	public void doubleClickAbsolute(int button, Point point) {
		doubleClickAbsolute(button, point.x, point.y);
	}

	@InUIThread
	public void doubleClickCenterAbsolute(int button, Rectangle rectangle) {
		doubleClickAbsolute(button, getCenter(rectangle));
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
