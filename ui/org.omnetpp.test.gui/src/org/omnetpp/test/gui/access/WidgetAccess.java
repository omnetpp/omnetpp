package org.omnetpp.test.gui.access;

import junit.framework.Assert;

import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.widgets.Widget;
import org.omnetpp.test.gui.core.InUIThread;

public class WidgetAccess extends Access 
{
	protected Widget widget;

	public WidgetAccess(Widget widget) {
		Assert.assertTrue(widget != null);
		this.widget = widget;
	}

	public Widget getWidget() {
		return widget;
	}

//	public Event newEvent(int type) {
//		return newEvent(widget, type);
//	}

	@InUIThread
	public void click(int button, int x, int y) {
		postMouseEvent(SWT.MouseMove, button, x, y);
		postMouseEvent(SWT.MouseDown, button, x, y);
		postMouseEvent(SWT.MouseUp, button, x, y);
	}

	@InUIThread
	public void doubleClick(int button, int x, int y) {
		click(button, x, y);
		postMouseEvent(SWT.MouseDown, button, x, y);
		postMouseEvent(SWT.MouseDoubleClick, button, x, y);
		postMouseEvent(SWT.MouseUp, button, x, y);
	}

	@InUIThread
	public void click(int button, Point point) {
		click(button, point.x, point.y);
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
