package com.simulcraft.test.gui.access;

import org.eclipse.swt.custom.CTabItem;
import org.eclipse.swt.widgets.Control;
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
	
	public MultiPageEditorPart getMultiPageEditorPart() {
		return (MultiPageEditorPart)workbenchPart;
	}
	
	@InUIThread
	public CTabItemAccess getItem(int pageIndex) {
		CTabItem item = (CTabItem)ReflectionUtils.invokeMethod(getMultiPageEditorPart(), "getItem", pageIndex); 
		return (CTabItemAccess)createAccess(item); 
	}
	
	@InUIThread
	public ControlAccess getControl(int pageIndex) {
		Control control = (Control)ReflectionUtils.invokeMethod(getMultiPageEditorPart(), "getControl", pageIndex);
		return (ControlAccess)createAccess(control);
	}
	
	@InUIThread
	public boolean isPageActive(String label) {
		CTabItemAccess activeCTabItem = getActiveCTabItem();
		return activeCTabItem != null && label.equals(activeCTabItem.getCTabItem().getText());
	}

	@InUIThread
	public EditorPartAccess getActivePageEditor() {
		return (EditorPartAccess)createAccess(ReflectionUtils.invokeMethod(getMultiPageEditorPart(), "getActiveEditor"));
	}

	@InUIThread
	public ControlAccess getActivePageControl() {
		int activePage = (Integer)ReflectionUtils.invokeMethod(getMultiPageEditorPart(), "getActivePage");
		return activePage >= 0 ? getControl(activePage) : null;
	}
	
	@InUIThread
	public CTabItemAccess getActiveCTabItem() {
		int activePage = (Integer)ReflectionUtils.invokeMethod(getMultiPageEditorPart(), "getActivePage");
		return activePage >= 0 ? getItem(activePage) : null;
	}
	
	@InUIThread
	public void activatePageWithMouseClick(String label) {
		CTabItem cTabItem = findDescendantCTabItemByLabel(getCompositeInternal().getParent(), label);
		new CTabItemAccess(cTabItem).click(LEFT_MOUSE_BUTTON);
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
