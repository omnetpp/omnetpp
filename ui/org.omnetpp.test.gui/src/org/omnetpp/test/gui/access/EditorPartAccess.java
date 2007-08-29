package org.omnetpp.test.gui.access;

import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.CTabItem;
import org.eclipse.ui.IEditorPart;
import org.omnetpp.test.gui.core.InUIThread;

public class EditorPartAccess extends WorkbenchPartAccess {

	public EditorPartAccess(IEditorPart editorPart) {
		super(editorPart);
	}

	public IEditorPart getEditorPart() {
		return (IEditorPart)workbenchPart;
	}
	
	@InUIThread
	public void activatePage(final String label) {
		CTabItem cTabItem = findDescendantCTabItemByLabel(getComposite().getParent(), label);
		clickCTabItem(cTabItem);
	}
	
	@InUIThread
	public void saveWithHotKey() {
		pressKey('s', SWT.CONTROL);
	}
	
}
