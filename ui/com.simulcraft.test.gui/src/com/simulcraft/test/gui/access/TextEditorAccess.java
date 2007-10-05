package com.simulcraft.test.gui.access;

import junit.framework.Assert;

import org.eclipse.ui.texteditor.ITextEditor;
import org.omnetpp.common.util.StringUtils;

import com.simulcraft.test.gui.core.InUIThread;

public class TextEditorAccess extends EditorPartAccess 
{
	public TextEditorAccess(ITextEditor editorPart) {
		super(editorPart);
	}
	
    @Override
	public ITextEditor getPart() {
	    return (ITextEditor)workbenchPart;
	}

	@InUIThread
	public void moveCursorAfter(String pattern) {
		findStyledText().moveCursorAfter(pattern);
	}

	@InUIThread
	public void typeIn(String string) {
		findStyledText().typeIn(string);
	}

    @InUIThread
    public String getTextContent() {
        return getPart().getDocumentProvider().getDocument(getPart().getEditorInput()).get();
    }

    /**
     * Checks editor contents with String.equals().
     * NOTE: this method is NOT equivalent to StyledText.assertContent, because
     * the StyledText widget doesn't store the content of collapsed folding regions.
     */
    @InUIThread
    public void assertContent(String content) {
        String documentContent = getTextContent();
        Assert.assertTrue("editor content does not match", documentContent.equals(content));
    }

    /**
     * Checks editor contents after normalizing all whitespace.
     * NOTE: this method is NOT equivalent to StyledText.assertContent, because
     * the StyledText widget doesn't store the content of collapsed folding regions.
     */
    @InUIThread
    public void assertContentIgnoringWhiteSpace(String content) {
        String documentContent = getTextContent();
        Assert.assertTrue("editor content does not match", StringUtils.areEqualIgnoringWhiteSpace(documentContent, content));
    }
    
    /**
     * Checks editor contents with regex match.
     * NOTE: this method is NOT equivalent to StyledText.assertContent, because
     * the StyledText widget doesn't store the content of collapsed folding regions.
     */
    @InUIThread
    public void assertContentMatches(String regex) {
        String documentContent = getTextContent();
        Assert.assertTrue("editor content does not match", documentContent.matches(regex));
    }
}
