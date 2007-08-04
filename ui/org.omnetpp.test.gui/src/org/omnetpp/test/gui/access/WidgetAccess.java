package org.omnetpp.test.gui.access;

import junit.framework.Assert;

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

	protected Event newEvent(int type) {
		Event event = super.newEvent(type);
		event.widget = widget;

		return event;
	}
}
