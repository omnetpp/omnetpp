package org.omnetpp.test.gui.access;

import junit.framework.Assert;

import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.Tree;
import org.eclipse.ui.IViewPart;
import org.eclipse.ui.internal.PartSite;
import org.omnetpp.test.gui.InUIThread;

public class ViewPartAccess extends Access
{
	protected IViewPart viewPart;

	public ViewPartAccess(IViewPart viewPart) {
		Assert.assertTrue(viewPart != null);
		this.viewPart = viewPart;
	}

	public TreeAccess findTree() {
		return new TreeAccess((Tree)findDescendantControl(getRootControl(), Tree.class));
	}

	@InUIThread
	public TableAccess findTable() {
		return new TableAccess((Table)findDescendantControl(getRootControl(), Table.class));
	}

	@InUIThread
	public void activateWithMouseClick() {
		clickCTabItem(findDescendantCTabItemByLabel(getRootControl().getParent(), viewPart.getSite().getRegisteredName()));
	}

	protected Composite getRootControl() {
		return (Composite)((PartSite)viewPart.getSite()).getPane().getControl();
	}
}
