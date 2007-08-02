package org.omnetpp.test.gui.access;

import junit.framework.Assert;

import org.eclipse.swt.widgets.Control;

public class ControlAccess<T extends Control> extends WidgetAccess<T>
{
	public ControlAccess(T control) {
		super(control);
	}

	public T getControl() {
		return widget;
	}

	public void clickWidget(int button) {
		click(button, widget.toDisplay(getCenter(widget.getBounds())));
	}
	
	public void assertEnabled() {
		Assert.assertTrue(widget.getEnabled());
	}
}
