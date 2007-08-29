package org.omnetpp.test.gui.access;

import junit.framework.Assert;

import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.CTabItem;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Widget;
import org.omnetpp.test.gui.core.InUIThread;

public class WidgetAccess<T extends Widget> extends Access 
{
	protected T widget;

	public WidgetAccess(T widget) {
		Assert.assertTrue(widget != null);
		this.widget = widget;
	}

	public T getWidget() {
		return widget;
	}

	public Event newEvent(int type) {
		return newEvent(widget, type);
	}

	@InUIThread
	public void pressKey(int keyCode) {
		pressKey(keyCode, SWT.None);
	}

	@InUIThread
	public void pressKey(int keyCode, int modifierKeys) {
		pressKey((char)0, keyCode, modifierKeys);
	}

	@InUIThread
	public void pressKey(char character) {
		pressKey(character, SWT.None);
	}

	@InUIThread
	public void pressKey(char character, int modifierKeys) {
		pressKey(character, 0, modifierKeys);
	}

	@InUIThread
	public void pressKey(char character, int keyCode, int modifierKeys) {
		Event event;

		if (modifierKeys != 0) {
			if ((modifierKeys & SWT.SHIFT) != 0) {
				event = newEvent(SWT.KeyDown);
				event.keyCode = SWT.SHIFT;
				postEvent(event);
			}

			if ((modifierKeys & SWT.CONTROL) != 0) {
				event = newEvent(SWT.KeyDown);
				event.keyCode = SWT.CONTROL;
				postEvent(event);
			}

			if ((modifierKeys & SWT.ALT) != 0) {
				event = newEvent(SWT.KeyDown);
				event.keyCode = SWT.ALT;
				postEvent(event);
			}
		}

		event = newEvent(SWT.KeyDown);
		event.character = character;
		event.keyCode = keyCode;
		postEvent(event);

		event = newEvent(SWT.KeyUp);
		event.keyCode = keyCode;
		event.character = (char)keyCode;
		postEvent(event);

		if (modifierKeys != 0) {
			if ((modifierKeys & SWT.SHIFT) != 0) {
				event = newEvent(SWT.KeyUp);
				event.keyCode = SWT.SHIFT;
				postEvent(event);
			}

			if ((modifierKeys & SWT.CONTROL) != 0) {
				event = newEvent(SWT.KeyUp);
				event.keyCode = SWT.CONTROL;
				postEvent(event);
			}

			if ((modifierKeys & SWT.ALT) != 0) {
				event = newEvent(SWT.KeyUp);
				event.keyCode = SWT.ALT;
				postEvent(event);
			}
		}
	}

	protected void postMouseEvent(int type, int button, int x, int y) {
		Event event = newEvent(type); // e.g. SWT.MouseMove
		event.button = button;
		event.x = x;
		event.y = y;
		event.count = 1;
		postEvent(event);
	}

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

	@InUIThread
	public void clickCTabItem(CTabItem cTabItem) {
		click(LEFT_MOUSE_BUTTON, cTabItem.getParent().toDisplay(getCenter(cTabItem.getBounds())));
	}
}
