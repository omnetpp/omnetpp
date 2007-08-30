package org.omnetpp.test.gui.access;

import org.eclipse.swt.custom.CTabItem;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.internal.PartSite;
import org.omnetpp.test.gui.core.InUIThread;

public class WorkbenchPartAccess
	extends Access
{
	protected IWorkbenchPart workbenchPart;

	public WorkbenchPartAccess(IWorkbenchPart workbenchPart) {
		this.workbenchPart = workbenchPart;
	}

	public IWorkbenchPart getWorkbenchPart() {
		return workbenchPart;
	}
	
	public CompositeAccess getComposite() {
		return (CompositeAccess)createAccess(getCompositeInternal());
	}

	protected Composite getCompositeInternal() {
		return (Composite)((PartSite)workbenchPart.getSite()).getPane().getControl();
	}

	@InUIThread
	public TreeAccess findTree() {
		return getComposite().findTree();
	}

	@InUIThread
	public void activateWithMouseClick() {
		CTabItem cTabItem = findDescendantCTabItemByLabel(getCompositeInternal().getParent(), workbenchPart.getSite().getRegisteredName());
		new CTabItemAccess(cTabItem).click();
	}
}
