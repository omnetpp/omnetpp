package com.simulcraft.test.gui.access;

import junit.framework.Assert;

import org.eclipse.swt.custom.CTabItem;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.IWorkbenchPartSite;
import org.eclipse.ui.internal.PartSite;
import org.eclipse.ui.part.MultiPageEditorSite;

import com.simulcraft.test.gui.core.UIStep;

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

	@UIStep
	public TreeAccess findTree() {
		return getComposite().findTree();
	}

    @UIStep
    public TableAccess findTable() {
        return getComposite().findTable();
    }

	@UIStep
	public void activateWithMouseClick() {
		getCTabItem().click();
	}

    @UIStep
    public CTabItemAccess getCTabItem() {
        return (CTabItemAccess)createAccess(findDescendantCTabItemByLabel(getCompositeInternal().getParent(), getPartName()));
    }
    
    @UIStep
    public boolean isActivated() {
        CTabItem cTabItem = getCTabItem().getWidget();
        IWorkbenchPart workbenchPart = getWorkbenchPart();
        return cTabItem.getParent().getSelection() == cTabItem &&
        		workbenchPart.getSite().getPage().getActivePart() == workbenchPart;
    }
    
    @UIStep
    public void assertActivated() {
    	Assert.assertTrue("The workbench part '" + getWorkbenchPart().getSite().getRegisteredName() + "' is not activated", isActivated());
    }
    
    @UIStep
    public void ensureActivated() {
        if (!isActivated())
            activateWithMouseClick();
    }
}
