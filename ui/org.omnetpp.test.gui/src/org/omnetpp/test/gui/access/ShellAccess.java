package org.omnetpp.test.gui.access;

import org.eclipse.swt.widgets.Shell;

public class ShellAccess extends CompositeAccess<Shell>
{
	public ShellAccess(Shell shell) {
		super(shell);
	}
	
	public Shell getShell() {
		return widget;
	}

}
