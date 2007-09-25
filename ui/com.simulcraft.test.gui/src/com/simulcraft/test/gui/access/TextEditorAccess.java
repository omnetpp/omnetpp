package com.simulcraft.test.gui.access;

import org.eclipse.ui.IEditorPart;

import com.simulcraft.test.gui.core.InUIThread;

public class TextEditorAccess extends EditorPartAccess 
{
	public TextEditorAccess(IEditorPart editorPart) {
		super(editorPart);
	}

	@InUIThread
	public void moveCursorAfter(String pattern) {
		findStyledText().moveCursorAfter(pattern);
	}

	@InUIThread
	public void typeIn(String string) {
		findStyledText().typeIn(string);
	}
}
