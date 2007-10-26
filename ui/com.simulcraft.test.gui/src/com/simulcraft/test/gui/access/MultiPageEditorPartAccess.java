package com.simulcraft.test.gui.access;

import junit.framework.Assert;

import org.eclipse.swt.custom.CTabItem;
import org.eclipse.swt.widgets.Control;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.part.MultiPageEditorPart;
import org.omnetpp.common.util.ReflectionUtils;

import com.simulcraft.test.gui.core.InUIThread;
import com.simulcraft.test.gui.core.NotInUIThread;


public class MultiPageEditorPartAccess
	extends EditorPartAccess
{
	public MultiPageEditorPartAccess(MultiPageEditorPart multiPageEditorPart) {
		super(multiPageEditorPart);
	}
	
    @Override
	public MultiPageEditorPart getWorkbenchPart() {
		return (MultiPageEditorPart)workbenchPart;
	}
    
    @InUIThread
    public void assertActivePage(String label) {
    	Assert.assertTrue("Page is not active: " + label, isPageActive(label));
    }
    
	@InUIThread
	public CTabItemAccess getCTabItem(int pageIndex) {
		CTabItem item = (CTabItem)ReflectionUtils.invokeMethod(getWorkbenchPart(), "getItem", pageIndex); 
		return (CTabItemAccess)createAccess(item); 
	}
	
	@InUIThread
	public ControlAccess getControl(int pageIndex) {
		Control control = (Control)ReflectionUtils.invokeMethod(getWorkbenchPart(), "getControl", pageIndex);
		return (ControlAccess)createAccess(control);
	}

	@InUIThread
	public EditorPartAccess getEditor(int pageIndex) {
	    IEditorPart part = (IEditorPart)ReflectionUtils.invokeMethod(getWorkbenchPart(), "getEditor", pageIndex);
	    return (EditorPartAccess)createAccess(part);
	}

	@InUIThread
	public boolean isPageActive(String label) {
		CTabItemAccess activeCTabItem = getActiveCTabItem();
		return activeCTabItem != null && activeCTabItem.getWidget().getText().matches(label);
	}

	@InUIThread
	public EditorPartAccess getActivePageEditor() {
		return (EditorPartAccess)createAccess(ReflectionUtils.invokeMethod(getWorkbenchPart(), "getActiveEditor"));
	}

	@InUIThread
	public ControlAccess getActivePageControl() {
		int activePage = (Integer)ReflectionUtils.invokeMethod(getWorkbenchPart(), "getActivePage");
		return activePage >= 0 ? getControl(activePage) : null;
	}
	
	@InUIThread
	public CTabItemAccess getActiveCTabItem() {
		int activePage = (Integer)ReflectionUtils.invokeMethod(getWorkbenchPart(), "getActivePage");
		return activePage >= 0 ? getCTabItem(activePage) : null;
	}
	
	public void activatePageWithMouseClick(String label) {
	    getCTabItem(label).click();
	}

    @InUIThread
	public CTabItemAccess getCTabItem(String label) {
        return (CTabItemAccess) createAccess(findDescendantCTabItemByLabel(getCompositeInternal().getParent(), label));
    }
	
	@NotInUIThread
	public EditorPartAccess activatePageEditor(String label) {
		activatePageWithMouseClick(label);
		return getActivePageEditor();
	}
	
	@NotInUIThread
	public ControlAccess activatePageControl(String label) {
		activatePageWithMouseClick(label);
		return getActivePageControl();
	}
	
	@NotInUIThread
	public EditorPartAccess ensureActiveEditor(String label) {
		if (!isPageActive(label))
			activatePageWithMouseClick(label);
		return getActivePageEditor();
	}

	@NotInUIThread
	public ControlAccess ensureActivePage(String label) {
		if (!isPageActive(label))
			activatePageWithMouseClick(label);
		return getActivePageControl();
	}
}
