package com.simulcraft.test.gui.access;

import junit.framework.Assert;

import org.eclipse.swt.widgets.Control;
import org.eclipse.ui.IViewPart;
import org.eclipse.ui.IWorkbenchPartSite;
import org.eclipse.ui.internal.ViewSite;

public class ViewPartAccess extends WorkbenchPartAccess {

	public ViewPartAccess(IViewPart viewPart) {
		super(viewPart);
	}

    @Override
	public IViewPart getPart() {
		return (IViewPart)workbenchPart;
	}
    
    public ToolBarAccess getViewToolbar() {
		IWorkbenchPartSite site = workbenchPart.getSite();
		Assert.assertTrue("Site is not a ViewSite", site instanceof ViewSite);
		Control toolbar = ((ViewSite)site).getPane().getToolBar();
		Assert.assertNotNull("No toolbar found", toolbar);
		return (ToolBarAccess)createAccess(toolbar);
    }
}
