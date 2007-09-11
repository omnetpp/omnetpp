package org.omnetpp.test.gui.access;

import org.eclipse.swt.SWT;
import org.eclipse.ui.IEditorPart;
import org.omnetpp.test.gui.core.InUIThread;

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
		pressKey(SWT.CONTROL + SWT.F4);
	}

	@InUIThread
	public StyledTextAccess findStyledText() {
		return getComposite().findStyledText();
	}
}
