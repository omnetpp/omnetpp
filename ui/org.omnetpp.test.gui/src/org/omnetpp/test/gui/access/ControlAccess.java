package org.omnetpp.test.gui.access;

import junit.framework.Assert;

import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Menu;

public class ControlAccess<T extends Control> extends ClickableWidgetAccess<T>
{
	public ControlAccess(T control) {
		super(control);
	}

	public T getControl() {
		return widget;
	}

	public void assertEnabled() {
		Assert.assertTrue(widget.getEnabled());
	}

	@Override
	protected Point getPointToClick() {
		return widget.getParent().toDisplay(getCenter(widget.getBounds()));
	}

	@Override
	protected Menu getContextMenu() {
		return widget.getMenu();
	}
}
