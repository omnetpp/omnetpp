package org.omnetpp.test.gui.access;

import junit.framework.Assert;

import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Menu;
import org.omnetpp.test.gui.InUIThread;

public class ControlAccess<T extends Control> extends ClickableWidgetAccess<T>
{
	public ControlAccess(T control) {
		super(control);
	}

	public T getControl() {
		return widget;
	}

	@InUIThread
	public void assertEnabled() {
		Assert.assertTrue(widget.getEnabled());
	}

	@InUIThread
	public void assertHasFocus() {
		Assert.assertTrue(widget.isFocusControl());
	}

	@InUIThread
	public void assertVisible() {
		Assert.assertTrue(widget.isVisible());
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
