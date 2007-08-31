package org.omnetpp.test.gui.access;

import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.omnetpp.test.gui.core.InUIThread;

public class ClickableAccess
	extends Access 
{
	@InUIThread
	public void moveMouse(Point p) {
		moveMouse(p.x, p.y);
	}

	@InUIThread
	public void moveMouse(int x, int y) {
		postMouseEvent(SWT.MouseMove, 0, x, y);
	}

	@InUIThread
	public void click(int button, Point point) {
		click(button, point.x, point.y);
	}

	@InUIThread
	public void click(int button, int x, int y) {
		moveMouse(x, y);
		postMouseEvent(SWT.MouseDown, button, x, y);
		postMouseEvent(SWT.MouseUp, button, x, y);
	}

	@InUIThread
	public void doubleClick(int button, int x, int y) {
		click(button, x, y);
		postMouseEvent(SWT.MouseDown, button, x, y);
		postMouseEvent(SWT.MouseUp, button, x, y);
	}

	@InUIThread
	public void doubleClick(int button, Point point) {
		doubleClick(button, point.x, point.y);
	}

	@InUIThread
	public void clickCenter(int button, Rectangle rectangle) {
		click(button, getCenter(rectangle));
	}

	@InUIThread
	public void doubleClickCenter(int button, Rectangle rectangle) {
		doubleClick(button, getCenter(rectangle));
	}
}
