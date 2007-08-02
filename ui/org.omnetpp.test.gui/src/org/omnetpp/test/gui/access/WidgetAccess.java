package org.omnetpp.test.gui.access;

import junit.framework.Assert;

import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Widget;

public class WidgetAccess<T extends Widget> extends Access
{
	protected T widget;
	
	public WidgetAccess(T widget)
	{
		Assert.assertTrue(widget != null);
		this.widget = widget;
	}

	public T getWidget() {
		return widget;
	}

	public void clickCenter(int button, Rectangle rectangle) {
		click(button, getCenter(rectangle));
	}

	public void click(int button, Point point) {
		click(button, point.x, point.y);
	}

	public void click(int button, int x, int y) {
		Event event = newEvent(SWT.MouseMove);
		event.x = x;
		event.y = y;
		postEvent(event);

		event = newEvent(SWT.MouseDown);
		event.button = button;
		event.x = x;
		event.y = y;
		event.count = 1;
		postEvent(event);

		event = newEvent(SWT.MouseUp);
		event.button = button;
		event.x = x;
		event.y = y;
		event.count = 1;
		postEvent(event);
	}
	
	protected Event newEvent(int type) {
		Event event = new Event();
		event.type = type;
		event.widget = widget;
		event.display = Display.getCurrent();
		
		return event;
	}
}
