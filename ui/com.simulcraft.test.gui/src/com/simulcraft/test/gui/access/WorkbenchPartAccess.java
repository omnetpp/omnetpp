package com.simulcraft.test.gui.access;

import junit.framework.Assert;

import org.eclipse.swt.custom.CTabItem;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.IWorkbenchPartSite;
import org.eclipse.ui.internal.PartSite;
import org.eclipse.ui.part.MultiPageEditorSite;

import com.simulcraft.test.gui.core.InUIThread;

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
		IWorkbenchPartSite site = workbenchPart.getSite();

		if (site instanceof PartSite)
			return (Composite)((PartSite)site).getPane().getControl();
		else if (site instanceof MultiPageEditorSite)
			return (Composite)((PartSite)((MultiPageEditorSite)site).getMultiPageEditor().getSite()).getPane().getControl();

		Assert.fail("Unknown site " + site);
		return null;
	}

	@InUIThread
	public TreeAccess findTree() {
		return getComposite().findTree();
	}

    @InUIThread
    public TableAccess findTable() {
        return getComposite().findTable();
    }

	@InUIThread
	public void activateWithMouseClick() {
		getCTabItem().click();
	}

    @InUIThread
    public CTabItemAccess getCTabItem() {
        return (CTabItemAccess)createAccess(findDescendantCTabItemByLabel(getCompositeInternal().getParent(), workbenchPart.getSite().getRegisteredName()));
    }

    @InUIThread
    public void ensureActivated() {
        CTabItem cTabItem = getCTabItem().getCTabItem();
        if (cTabItem.getParent().getSelection() != cTabItem)
            activateWithMouseClick();
    }
}
