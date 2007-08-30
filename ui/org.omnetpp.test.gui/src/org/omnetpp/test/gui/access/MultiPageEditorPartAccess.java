package org.omnetpp.test.gui.access;

import org.eclipse.swt.custom.CTabItem;
import org.eclipse.ui.part.MultiPageEditorPart;
import org.omnetpp.common.util.ReflectionUtils;
import org.omnetpp.test.gui.core.InUIThread;
import org.omnetpp.test.gui.core.NotInUIThread;


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
	public EditorPartAccess getActivePage() {
		return (EditorPartAccess)createAccess(ReflectionUtils.invokeMethod(getMultiPageEditorPart(), "getActiveEditor"));
	}

	@InUIThread
	public void activatePageWithMouseClick(final String label) {
		CTabItem cTabItem = findDescendantCTabItemByLabel(getCompositeInternal().getParent(), label);
		new CTabItemAccess(cTabItem).click(LEFT_MOUSE_BUTTON);
	}

	@NotInUIThread
	public EditorPartAccess activatePage(final String label) {
		activatePageWithMouseClick(label);
		return getActivePage();
	}
}
