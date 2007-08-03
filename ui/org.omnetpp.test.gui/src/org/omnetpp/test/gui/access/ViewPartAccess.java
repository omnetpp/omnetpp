package org.omnetpp.test.gui.access;

import junit.framework.Assert;

import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.Tree;
import org.eclipse.ui.IViewPart;
import org.eclipse.ui.internal.PartSite;

public class ViewPartAccess extends Access
{
	protected IViewPart viewPart;

	public ViewPartAccess(IViewPart viewPart) {
		Assert.assertTrue(viewPart != null);
		this.viewPart = viewPart;
	}

	public TreeAccess findTree() {
		return new TreeAccess((Tree)findChildControl(getRootControl(), Tree.class));
	}

	public TableAccess findTable() {
		return new TableAccess((Table)findChildControl(getRootControl(), Table.class));
	}

	public void activateWithMouseClick() {
		clickCTabItem(findDescendantCTabItemByLabel(getRootControl().getParent(), viewPart.getTitle()));
	}

	protected Composite getRootControl() {
		return (Composite)((PartSite)viewPart.getSite()).getPane().getControl();
	}
}
