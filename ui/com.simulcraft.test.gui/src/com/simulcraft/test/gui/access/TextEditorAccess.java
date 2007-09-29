package com.simulcraft.test.gui.access;

import junit.framework.Assert;

import org.eclipse.ui.texteditor.ITextEditor;

import com.simulcraft.test.gui.core.InUIThread;

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
     * Checks editor contents with String.equals().
     * NOTE: this method is NOT equivalent to StyledText.assertContent, because
     * the StyledText widget doesn't store the content of collapsed folding regions.
     */
    @InUIThread
    public void assertContent(String content) {
        String documentContent = getTextEditor().getDocumentProvider().getDocument(getTextEditor().getEditorInput()).get();
        Assert.assertTrue("editor content does not match", documentContent.equals(content));
    }

    /**
     * Checks editor contents with regex match.
     * NOTE: this method is NOT equivalent to StyledText.assertContent, because
     * the StyledText widget doesn't store the content of collapsed folding regions.
     */
    @InUIThread
    public void assertContentMatches(String regex) {
        String documentContent = getTextEditor().getDocumentProvider().getDocument(getTextEditor().getEditorInput()).get();
        Assert.assertTrue("editor content does not match", documentContent.matches(regex));
    }
}
