package org.omnetpp.test.gui.access;

import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.internal.PartSite;
import org.omnetpp.test.gui.core.InUIThread;

public class WorkbenchPartAccess extends CompositeAccess<Composite> {
	protected IWorkbenchPart workbenchPart;

	public WorkbenchPartAccess(IWorkbenchPart workbenchPart) {
		super((Composite)((PartSite)workbenchPart.getSite()).getPane().getControl());
		this.workbenchPart = workbenchPart;
	}

	public IWorkbenchPart getWorkbenchPart() {
		return workbenchPart;
	}

	@InUIThread
	public void activateWithMouseClick() {
		clickCTabItem(findDescendantCTabItemByLabel(getComposite().getParent(), workbenchPart.getSite().getRegisteredName()));
	}
	
}
