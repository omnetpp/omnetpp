package org.omnetpp.test.gui.access;

import junit.framework.Assert;

import org.eclipse.swt.widgets.Widget;

public class WidgetAccess
	extends ClickableAccess 
{
	protected Widget widget;

	public WidgetAccess(Widget widget) {
		Assert.assertTrue(widget != null);
		this.widget = widget;
	}

	public Widget getWidget() {
		return widget;
	}
}
