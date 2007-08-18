package org.omnetpp.test.gui.access;

import java.util.List;

import junit.framework.Assert;

import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Menu;
import org.omnetpp.common.util.IPredicate;
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
		Assert.assertTrue("control is disabled", widget.getEnabled());
	}

	@InUIThread
	public void assertHasFocus() {
		Assert.assertTrue("control has no focus", widget.isFocusControl());
	}

	@InUIThread
	public void assertVisible() {
		Assert.assertTrue("control not visible", widget.isVisible());
	}

	@Override
	protected Point getPointToClick() {
		return widget.getParent().toDisplay(getCenter(widget.getBounds()));
	}

	@Override
	protected Menu getContextMenu() {
		return widget.getMenu();
	}

	@InUIThread
	public void typeIn(String text) {
		assertHasFocus();

		for (int i = 0; i < text.length(); i++)
			pressKey(text.charAt(i));
	}

	@InUIThread
	public Control findNextControl(final IPredicate predicate) {
		// Returns the first control after this one that matches the predicate
		// TODO: should consider layout
		List<Control> objects = collectDescendantControls(widget.getShell(), new IPredicate() {
			boolean thisWidgetSeen = false;
			public boolean matches(Object object) {
				if (object == widget)
					thisWidgetSeen = true;
				return thisWidgetSeen && predicate.matches(object);
			}
		});
		Assert.assertTrue("No object found", objects.size() > 0);
		return objects.get(0);
	}

	@InUIThread
	public Control findNextControl(final Class<? extends Control> clazz) {
		return findNextControl(new IPredicate() {
			public boolean matches(Object object) {
				return clazz.isInstance(object);
			}
		});
	}

}

