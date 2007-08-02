package org.omnetpp.test.gui.access;

import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Tree;
import org.eclipse.ui.IViewPart;
import org.eclipse.ui.internal.PartSite;

public class ViewPartAccess extends Access
{
	protected IViewPart viewPart;

	public ViewPartAccess(IViewPart viewPart) {
		this.viewPart = viewPart;
	}

	public TreeAccess findTree() {
		Composite composite = (Composite)((PartSite)viewPart.getSite()).getPane().getControl();
		return new TreeAccess((Tree)findChildControl(composite, Tree.class));
	}
}
