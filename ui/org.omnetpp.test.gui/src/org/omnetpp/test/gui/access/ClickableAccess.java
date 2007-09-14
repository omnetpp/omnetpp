package org.omnetpp.test.gui.access;

import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.omnetpp.test.gui.core.InUIThread;

public class ClickableAccess
	extends Access 
{
	@InUIThread
	public void moveMouseAbsolute(Point p) {
		moveMouseAbsolute(p.x, p.y);
	}

	@InUIThread
	public void moveMouseAbsolute(int x, int y) {
		postMouseEvent(SWT.MouseMove, 0, x, y);
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
