package com.simulcraft.test.gui.access;

import junit.framework.Assert;

import org.eclipse.swt.custom.CTabItem;
import org.eclipse.swt.widgets.Composite;
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

	public IWorkbenchPart getPart() {
		return workbenchPart;
	}
	
	protected PartSite getPartSite() {
		IWorkbenchPartSite site = workbenchPart.getSite();

		if (site instanceof PartSite)
			return ((PartSite)site);
		else if (site instanceof MultiPageEditorSite)
			return (PartSite)((MultiPageEditorSite)site).getMultiPageEditor().getSite();

		Assert.fail("Unknown site " + site);
		return null;
	}
	
	/**
	 * Returns the name of the part. This is the name visible on tabs.
	 */
	public String getPartName() {
		return getPartSite().getPartReference().getPartName();
	}
	
	public CompositeAccess getComposite() {
		return (CompositeAccess)createAccess(getCompositeInternal());
	}

	protected Composite getCompositeInternal() {
		return (Composite)getPartSite().getPane().getControl();
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
        return (CTabItemAccess)createAccess(findDescendantCTabItemByLabel(getCompositeInternal().getParent(), getPartName()));
    }
    
    @InUIThread
    public boolean isActivated() {
        CTabItem cTabItem = getCTabItem().getWidget();
        IWorkbenchPart workbenchPart = getPart();
        return cTabItem.getParent().getSelection() == cTabItem &&
        		workbenchPart.getSite().getPage().getActivePart() == workbenchPart;
    }
    
    @InUIThread
    public void assertActivated() {
    	Assert.assertTrue("The workbench part '" + getPart().getSite().getRegisteredName() + "' is not activated", isActivated());
    }
    
    @InUIThread
    public void ensureActivated() {
        if (!isActivated())
            activateWithMouseClick();
    }
}
