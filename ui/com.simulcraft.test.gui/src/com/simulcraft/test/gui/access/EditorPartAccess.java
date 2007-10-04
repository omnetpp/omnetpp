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

	public IEditorPart getEditorPart() {
		return (IEditorPart)workbenchPart;
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
    public void assertDirty() {
        Assert.assertTrue("editor is not dirty", getEditorPart().isDirty());
    }

	@InUIThread
    public void assertNotDirty() {
        Assert.assertTrue("editor is dirty", !getEditorPart().isDirty());
    }
}
