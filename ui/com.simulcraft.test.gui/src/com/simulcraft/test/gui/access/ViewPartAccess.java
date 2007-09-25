package com.simulcraft.test.gui.access;

import org.eclipse.ui.IViewPart;

public class ViewPartAccess extends WorkbenchPartAccess {

	public ViewPartAccess(IViewPart viewPart) {
		super(viewPart);
	}

	public IViewPart getViewPart() {
		return (IViewPart)workbenchPart;
	}
}
