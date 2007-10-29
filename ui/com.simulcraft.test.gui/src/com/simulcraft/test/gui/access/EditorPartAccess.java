package com.simulcraft.test.gui.access;

import junit.framework.Assert;

import org.eclipse.swt.SWT;
import org.eclipse.ui.IEditorPart;

import com.simulcraft.test.gui.core.UIStep;

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
	
	@Override @UIStep
    public CTabItemAccess getCTabItem() {
		String tabLabel = getWorkbenchPart().isDirty() ? "\\*" + getPartName() : getPartName();
        return (CTabItemAccess)createAccess(findDescendantCTabItemByLabel(getCompositeInternal().getParent(), tabLabel));
    }
	
	@UIStep
	public void saveWithHotKey() {
		pressKey('s', SWT.CONTROL);
	}

	@UIStep
	public void closeWithHotKey() {
		pressKey(SWT.F4, SWT.CONTROL);
	}

	@UIStep
	public StyledTextAccess findStyledText() {
		return getComposite().findStyledText();
	}
	
    @UIStep
	public ToolBarManagerAccess getToolBarManager() {
        return new ToolBarManagerAccess(getWorkbenchPart().getEditorSite().getActionBars().getToolBarManager());
	}

	@UIStep
    public void assertDirty() {
        Assert.assertTrue("editor is not dirty", getWorkbenchPart().isDirty());
    }

	@UIStep
    public void assertNotDirty() {
        Assert.assertTrue("editor is dirty", !getWorkbenchPart().isDirty());
    }
}
