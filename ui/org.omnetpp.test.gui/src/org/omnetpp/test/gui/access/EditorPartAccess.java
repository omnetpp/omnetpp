package org.omnetpp.test.gui.access;

import org.eclipse.swt.custom.CTabItem;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.internal.EditorSite;
import org.omnetpp.test.gui.InUIThread;

public class EditorPartAccess extends CompositeAccess<Composite> {
	protected IEditorPart editorPart;

	public EditorPartAccess(IEditorPart editorPart) {
		super((Composite)((EditorSite)editorPart.getEditorSite()).getPane().getControl());
		this.editorPart = editorPart;
	}

	public IEditorPart getEditorPart() {
		return editorPart;
	}
	
	@InUIThread
	public void activatePageInMultiPageEditorByLabel(final String label) {
		CTabItem cTabItem = findDescendantCTabItemByLabel(getComposite().getParent(), label);
		clickCTabItem(cTabItem);
	}
}
