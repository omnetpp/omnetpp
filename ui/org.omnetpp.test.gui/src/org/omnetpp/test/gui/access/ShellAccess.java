package org.omnetpp.test.gui.access;

import junit.framework.Assert;

import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Shell;
import org.omnetpp.test.gui.core.InUIThread;

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
	 * for it to become active. It is recommended to begin test cases with 
	 * <code>Access.getWorkbenchWindowAccess().assertIsActiveShell();<code>
	 * to ensure the workbench window appears before the test starts, and to
	 * detect open dialogs left behind by previous test cases.
	 */
	@InUIThread
	public void assertIsActive() {
		Assert.assertTrue("not the active shell", Display.getCurrent().getActiveShell() == getShell());
	}
}
