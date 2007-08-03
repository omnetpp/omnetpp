package org.omnetpp.test.gui.access;

import junit.framework.Assert;

import org.eclipse.swt.custom.CTabItem;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.internal.EditorSite;

public class EditorPartAccess extends Access {
	protected IEditorPart editorPart;

	public EditorPartAccess(IEditorPart editorPart) {
		Assert.assertTrue(editorPart != null);
		this.editorPart = editorPart;
	}

	public void activatePageInMultiPageEditorByLabel(final String label) {
		CTabItem cTabItem = findDescendantCTabItemByLabel(getRootControl().getParent(), label);
		clickCTabItem(cTabItem);
	}
	
	protected Composite getRootControl() {
		return (Composite)((EditorSite)editorPart.getEditorSite()).getPane().getControl();
	}
}
