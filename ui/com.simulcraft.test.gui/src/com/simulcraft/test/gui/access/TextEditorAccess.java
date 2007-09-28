package com.simulcraft.test.gui.access;

import junit.framework.Assert;

import com.simulcraft.test.gui.core.InUIThread;

import org.eclipse.jface.text.IDocument;
import org.eclipse.ui.texteditor.ITextEditor;

public class TextEditorAccess extends EditorPartAccess 
{
	public TextEditorAccess(ITextEditor editorPart) {
		super(editorPart);
	}
	
	public ITextEditor getTextEditor() {
	    return (ITextEditor)getEditorPart();
	}

	@InUIThread
	public void moveCursorAfter(String pattern) {
		findStyledText().moveCursorAfter(pattern);
	}

	@InUIThread
	public void typeIn(String string) {
		findStyledText().typeIn(string);
	}
	
    /**
     * NOTE: this is not the same as StyledText.assertContent (styled text does not contains the collapsed
     * folding regions), this one DOES!
     */
    @InUIThread
    public void assertContent(String content) {
        String documentContent = getTextEditor().getDocumentProvider().getDocument(getTextEditor().getEditorInput()).get();
        Assert.assertTrue("editor content does not match", documentContent.equals(content));
    }
}
