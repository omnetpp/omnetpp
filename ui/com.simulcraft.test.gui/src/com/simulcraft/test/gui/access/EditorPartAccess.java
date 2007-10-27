package com.simulcraft.test.gui.access;

import junit.framework.Assert;

import org.eclipse.swt.SWT;
import org.eclipse.ui.IEditorPart;

import com.simulcraft.test.gui.core.InUIThread;

public class EditorPartAccess
	extends WorkbenchPartAccess
{
	public EditorPartAccess(IEditorPart editorPart) {
		super(editorPart);
	}

	@Override
	public IEditorPart getWorkbenchPart() {
		return (IEditorPart)workbenchPart;
	}
	
	@Override @InUIThread
    public CTabItemAccess getCTabItem() {
		String tabLabel = getWorkbenchPart().isDirty() ? "\\*" + getPartName() : getPartName();
        return (CTabItemAccess)createAccess(findDescendantCTabItemByLabel(getCompositeInternal().getParent(), tabLabel));
    }
	
	@InUIThread
	public void saveWithHotKey() {
		pressKey('s', SWT.CONTROL);
	}

	@InUIThread
	public void closeWithHotKey() {
		pressKey(SWT.F4, SWT.CONTROL);
	}

	@InUIThread
	public StyledTextAccess findStyledText() {
		return getComposite().findStyledText();
	}
	
    @InUIThread
	public ToolBarManagerAccess getToolBarManager() {
        return new ToolBarManagerAccess(getWorkbenchPart().getEditorSite().getActionBars().getToolBarManager());
	}

	@InUIThread
    public void assertDirty() {
        Assert.assertTrue("editor is not dirty", getWorkbenchPart().isDirty());
    }

	@InUIThread
    public void assertNotDirty() {
        Assert.assertTrue("editor is dirty", !getWorkbenchPart().isDirty());
    }
}
