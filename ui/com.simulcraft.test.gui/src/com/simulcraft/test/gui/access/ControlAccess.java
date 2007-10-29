package com.simulcraft.test.gui.access;

import java.util.List;

import junit.framework.Assert;

import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Menu;
import org.omnetpp.common.util.IPredicate;

import com.simulcraft.test.gui.core.UIStep;


public class ControlAccess extends ClickableWidgetAccess
{
	public ControlAccess(Control control) {
		super(control);
	}

	public Control getControl() {
		return (Control)widget;
	}

	@UIStep
	public void assertEnabled() {
		Assert.assertTrue("control is disabled", getControl().getEnabled());
	}

	@UIStep
	public void assertDisabled() {
	    Assert.assertTrue("control is enabled", !getControl().getEnabled());
	}

	@UIStep
	public void assertHasFocus() {
		Assert.assertTrue("control has no focus", getControl().isFocusControl());
	}

	@UIStep
	public void assertHasNoFocus() {
	    Assert.assertTrue("control has focus", !getControl().isFocusControl());
	}

	@UIStep
	public void assertVisible() {
		Assert.assertTrue("control not visible", getControl().isVisible());
	}

	@UIStep
	public void assertNotVisible() {
	    Assert.assertTrue("control visible", !getControl().isVisible());
	}

	@Override @UIStep
	protected Point getAbsolutePointToClick() {
		return toAbsolute(getCenter(getControl().getBounds()));
	}

    @Override @UIStep
	protected Point toAbsolute(Point point) {
        return getControl().getParent().toDisplay(point);
    }

	@UIStep
	public Rectangle getAbsoluteBounds() {
	    Rectangle bounds = getControl().getBounds();
	    Point topLeftAbsolute = getControl().getParent().toDisplay(bounds.x, bounds.y);
        return new Rectangle(topLeftAbsolute.x, topLeftAbsolute.y, bounds.width, bounds.height);
	}

	@Override
	protected Menu getContextMenu() {
		return getControl().getMenu();
	}

	public void typeIn(String text) {
		assertHasFocus();
		pressKeySequence(text);
	}

	@UIStep
	public Control findNextControl(final IPredicate predicate) {
		// Returns the first control after this one that matches the predicate
		// TODO: should consider layout
		List<Control> objects = collectDescendantControls(getControl().getShell(), new IPredicate() {
			boolean thisWidgetSeen = false;
			public boolean matches(Object object) {
				if (object == getControl())
					thisWidgetSeen = true;
				return thisWidgetSeen && predicate.matches(object);
			}
		});
		Assert.assertTrue("No object found", objects.size() > 0);
		return objects.get(0);
	}

	@UIStep
	public Control findNextControl(final Class<? extends Control> clazz) {
		return findNextControl(new IPredicate() {
			public boolean matches(Object object) {
				return clazz.isInstance(object);
			}
		});
	}

}

