package com.simulcraft.test.gui.access;

import java.util.List;

import junit.framework.Assert;

import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Menu;
import org.omnetpp.common.util.IPredicate;

import com.simulcraft.test.gui.core.InUIThread;


public class ControlAccess extends ClickableWidgetAccess
{
	public ControlAccess(Control control) {
		super(control);
	}

	public Control getControl() {
		return (Control)widget;
	}

	@InUIThread
	public void assertEnabled() {
		Assert.assertTrue("control is disabled", getControl().getEnabled());
	}

	@InUIThread
	public void assertHasFocus() {
		Assert.assertTrue("control has no focus", getControl().isFocusControl());
	}

	@InUIThread
	public void assertVisible() {
		Assert.assertTrue("control not visible", getControl().isVisible());
	}

	@Override @InUIThread
	protected Point getAbsolutePointToClick() {
		return getControl().getParent().toDisplay(getCenter(getControl().getBounds()));
	}

	@Override
	protected Menu getContextMenu() {
		return getControl().getMenu();
	}

	@InUIThread
	public void typeIn(String text) {
		assertHasFocus();

		for (int i = 0; i < text.length(); i++) {
			char character = text.charAt(i);
			boolean holdShift = Character.isUpperCase(character);

			// TODO: these are kind of a hack
			// but for example if we don't hold the shift down then we get ';' instead of ':'
			if (character == ':' || character == '{' || character == '}') 
				holdShift = true;
			else if (character == '\n')
				character = '\r';
			
			pressKey(Character.toLowerCase(character), holdShift ? SWT.SHIFT : SWT.NONE);
		}
	}

	@InUIThread
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

	@InUIThread
	public Control findNextControl(final Class<? extends Control> clazz) {
		return findNextControl(new IPredicate() {
			public boolean matches(Object object) {
				return clazz.isInstance(object);
			}
		});
	}

}

