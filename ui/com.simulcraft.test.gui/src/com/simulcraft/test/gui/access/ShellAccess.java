package com.simulcraft.test.gui.access;

import junit.framework.Assert;

import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Shell;

import com.simulcraft.test.gui.core.InUIThread;

public class ShellAccess extends CompositeAccess
{
	public ShellAccess(Shell shell) {
		super(shell);
	}

	public Shell getShell() {
		return (Shell)widget;
	}

	@InUIThread
	public MenuAccess getMenuBar() {
		return new MenuAccess(getShell().getMenuBar());
	}

	/**
	 * Asserts that this shell is the active shell, i.e. it waits (with timeout)
	 * for it to become active.
	 */
	@InUIThread
	public void assertIsActive() {
		Assert.assertTrue("not the active shell", Display.getCurrent().getActiveShell() == getShell());
	}
}
